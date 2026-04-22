/* BotNET Communication Stream (botserv)
 *
 * This program simply allows clients to exchange data anonymously
 * The default port can be configured in config.h
 *
 */

#include "../include/bot.h"

static int clients[100];
static char tcmd[MAXLINE];

void start(void);
int countbots(void);
ssize_t dsendall(int,const char*,...);

int main(int argc,char **argv) {
	pid_t pid;

	if( (pid = fork()) == 0) start();

	return;
}
void start(void) {
	int i,connfd,listenfd,maxfd,socketfd,nready;
	int j,k;
	char c;
	socklen_t clilen;
	ssize_t n;
	char cmd[1024+SSIZE], tmp[MAXLINE], user[LINE], from[LINE];
	fd_set rset,allset;
	struct sockaddr_in cliaddr,servaddr;

	if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket creation error");
		exit(-1);
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(SERVPORT));
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0) {
		perror("bind error");
		exit(-1);
	}
	if(listen(listenfd,100) < 0) {
		perror("listen error");
		exit(-1);
	}
	maxfd = listenfd;
	for(i = 0;i <= 100;i++)
		clients[i] = -1;
	FD_SET(listenfd,&allset);

	for(;;) {
		rset = allset;
		if( (nready = select(maxfd+1,&rset,NULL,NULL,NULL)) < 0) {
			perror("select error");
			exit(-1);
		}
		if(FD_ISSET(listenfd,&rset)) {
			clilen = sizeof(cliaddr);
			if( (connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen)) < 0)
				perror("accept error");
			for(i = 0;i <= 100;i++)
				if(clients[i] < 0) {
					clients[i] = connfd;
					break;
				}
			FD_SET(connfd,&allset);
			if(connfd > maxfd) maxfd = connfd;
			if(--nready <= 0) continue;
		}
		for(i = 0;i <= 100;i++) {
			if( (socketfd = clients[i]) < 0) continue;
			if(FD_ISSET(socketfd,&rset)) {
				if( (n = dbrecv(socketfd,cmd)) < 0) {
					close(socketfd);
					FD_CLR(socketfd,&allset);
					clients[i] = -1;
				} else {
					dsendall(socketfd,"%s\n",cmd);
					j = k = 0;
					while( (c=cmd[j++]) != ';')
						tmp[k++] = c;
					tmp[k] = '\0';
					k = 0;
					if(!strcmp(tmp,"NUMBOTS?")) {
						while( (c=cmd[j++]) != ';')
							user[k++] = c;
						user[k] = '\0';
						k = 0;
						while( (c=cmd[j++]) != ';')
							from[k++] = c;
						from[k] = '\0';
						dsendall(-1,"NUMBOTS;%s;%s;%d;\n",user,from,countbots());
					}
					if(--nready <= 0) break;
					else continue;
				}
			}
		}
	}

	return;
}
char *getnick(char cmd[MAXLINE]) {
	char *ptr,*nick;
	
	strncpy(tcmd,cmd,sizeof(tcmd));
	ptr = strtok(tcmd,"!");
	nick = (ptr+1);
	
	return nick;
}
int rightpass(char username[],char password[],FILE *fp) {
        int i,j,found=0;
        char c;
        char line[LINE*2],user[LINE],pass[LINE];

        rewind(fp);
        while(fgets(line,LINE*2+1,fp)!=NULL&&found==0) {
                i = (-1);
                j = 0;
                while((c=line[++i])!=':')
                        user[i] = c;
                user[i++] = 0; /* null terminate */
                while((c=line[i++])!='\n')
                        pass[j++] = c;
                pass[j] = 0;
                if((strcmp(user,username)==0)&&(strcmp(pass,password)==0))
                        found = 1; /* right password: function returns true */
        }
        rewind(fp);
        return found;
}
int userexist(char user[SSIZE]) {
	int i;
	char c;
	char str[SSIZE],line[MAXLINE];

	rewind(usersfp);
	while(fgets(line,MAXLINE,usersfp)!=NULL) {
		i=0;
		while( (c=line[i])!=':')
			str[i++] = c;
		str[i] = 0;
		strtolower2(user);
		strtolower2(str);
		if(!strcmp(user,str)) return 1; /* user exists: return true */
	}
	rewind(usersfp);

	return 0;
}
void prep(void) {
	int i;
	char ffile[MAXLINE];
	
	snprintf(ffile,sizeof(ffile),"%s/db/users",INSTALLDIR);
	if( (usersfp=fopen(ffile,"r+"))==NULL)
		qerr("Unable to open database file: %s\n",ffile);
	for(i=0;i<=100-1;i++)
		users[i].flags = NORMAL;
	
	return;
}
int useraccess(char *nick) {
	int i;
	int access = 0;

	for(i=0;i<=100-1;i++)
		if(users[i].flags==OPER)
			if(!strcmp(users[i].name,nick)) {
				access = 1;
				break;
			}

	return access;
}
void adduser(char *user,char *pass) {
	fseek(usersfp,0L,SEEK_END);
	fputs(user,usersfp);
	putc(':',usersfp);
	fputs(pass,usersfp);
	putc('\n',usersfp);
	rewind(usersfp);

	return;
}
int deluser(char *user) {
	int i,record;
	char buf[LINE],mffile[MAXLINE];
	char *ptr;

	rewind(usersfp);
	i = record = 0;
	while(fgets(buf,sizeof(buf),usersfp)!=NULL) {
		i++;
		ptr = strtok(buf,":");
		strtolower2(user);
		strtolower2(ptr);
		if(!strcmp(user,ptr)) {
			record = i;
			break;
		}
	}

	if(!record) return (-1);
	else {
		update_file(record,usersfp,"db/users",1);
		snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,user);
		remove(mffile);
		return record;
	}
}
void update_file(long record,FILE *fp,char filename[SSIZE],int reopen) {
	long i = 0L;
	char buf[MAXLINE],ffile[LINE],dbffile[MAXLINE],cmd[MAXLINE+LINE];
	FILE *tfp;

	snprintf(ffile,sizeof(ffile),"%s/temp",INSTALLDIR);
	creat(ffile,0600);
	if( (tfp=fopen(ffile,"r+"))==NULL)
		qerr("Unable to open temporary file: write access is required.\n");
	
	rewind(fp);
	while(fgets(buf,sizeof(buf),fp)!=NULL)
		if(++i != record)
			fputs(buf,tfp);
	fclose(tfp);
	fclose(fp);
	snprintf(dbffile,sizeof(dbffile),"%s/%s",INSTALLDIR,filename);
	copyfile(ffile,dbffile);
	remove(ffile);

	if(reopen)
		if( (fp=fopen(dbffile,"r+"))==NULL)
			qerr("Unable to open database file: %s\n",filename);

	return;
}
void setpasswd(char *user,char *pass) {
	deluser(user);
	adduser(user,pass);

	return;
}
void qerr(const char *fmt,...) {
        va_list args;

        va_start(args,fmt);
        fprintf(stderr,"FATAL: ");
        vfprintf(stderr,fmt,args);
        fprintf(stderr,"\n");
        va_end(args);

        exit(-1);
}
void delaydots(int n) {
	int i;

	for(i=1;i<=n;i++) {
		usleep(10000);
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	return;
}
ssize_t dsend(int sockfd,const char *fmt,...) {
        ssize_t n;
        va_list args;
        char buf[128];

        va_start(args,fmt);
        vsnprintf(buf,sizeof(buf),fmt,args);
        va_end(args);

        n=send(sockfd,buf,strlen(buf),0);

        return n;
}
ssize_t drecvn(int sockfd,char buf[MAXLINE+1]) {
        ssize_t n;

        n=recv(sockfd,buf,MAXLINE,0);
        buf[n-2] = 0;
	dsend(sockfd,"\n");

        return n-2;
}
int args(char cmd[MAXLINE],char cargs[50][MAXLINE]) {
        int i,j,k,cwords;
        char word[MAXLINE+1];
        char c;

        i=cwords=0;
        while( (c=cmd[i++])!=0)
                if(isspace(c)) cwords++;
        cwords++;
        for(i=1,k=0;i<=cwords;i++) {
                for(j=0;!isspace(cmd[k]);) 
                        word[j++] = cmd[k++];
                word[j] = '\0';
                strncpy(cargs[i-1],word,sizeof(cargs[i-1]));
                k++;
        }
	
	return cwords;
}
int putword(char *word,FILE *file) {
	int i;
	char c;

	i = 0;
	while( (c=word[i++])!='\0')
		if(putc(c,file)==EOF)
			return (EOF);

	return 0;
}
void trim(char s[MAXLINE],char c) {
	int i,j;

	for(i=j=0;s[i]!='\0';i++)
		if(s[i] != c)
			s[j++] = s[i];
	s[j] = '\0';
	
	return;
}
void addlinetofile(FILE *fp,char line[MAXLINE],int record) {
	int i = 1;
	char buf[MAXLINE];

	rewind(fp);
	while(fgets(buf,MAXLINE,fp)!=NULL)
		if(i++ == record) {
			fputs(line,fp);
			break;
		}

	return;
}
void copyfile(char from[MAXLINE],char to[MAXLINE]) {
	char buf[MAXLINE];
	FILE *fromfp,*tofp;

	if( (fromfp = fopen(from,"r+")) == NULL)
		return;
	
	remove(to);
	creat(to,0600);
	if( (tofp = fopen(to,"r+")) == NULL)
		return;

	while(fgets(buf,MAXLINE,fromfp) != NULL)
		fputs(buf,tofp);

	fclose(fromfp);
	fclose(tofp);

	return;
}
char *dhms(time_t ticks) {
	int days,hours,minutes,seconds;
	char str[LINE*2];
	
	seconds = (int)ticks;
	
	minutes = seconds / 60;
	seconds -= minutes*60;

	hours = minutes / 60;
	minutes -= hours*60;

	days = hours / 24;
	hours -= days*24;

	snprintf(str,sizeof(str),"%d day%s, %d hour%s, %d minute%s and %d second%s",days,(days!=1)?"s":"",hours,(hours!=1)?"s":"",minutes,(minutes!=1)?"s":"",seconds,(seconds!=1)?"s":"");

	return strdup(str);
}
void respawn(int signo) {
	char prog[MAXLINE];

	snprintf(prog,sizeof(prog),"%s/bin/bot",INSTALLDIR);
	execl(prog, "BotNET", (char*) 0);

	return;
}
ssize_t dsendall(int connfd,const char *fmt,...) {
	        /* connfd represents the connfd to discard from dsend() */
	        int i;
	        ssize_t n;
	        va_list args;
	        char buf[1024];

	        va_start(args,fmt);
		vsnprintf(buf,sizeof(buf),fmt,args);
	        va_end(args);

	        for(i = 0;i <= 100;i++)
                if(clients[i] >= 0 && clients[i] != connfd)
	                n = send(clients[i],buf,strlen(buf),0);

	        return n;
}
int countbots(void) {
	int i,count;

	count = 0;
	for(i = 0;i <= 100;i++) {
		if(clients[i] < 0) continue;
		else count++;
	}

	return count;
}
ssize_t drecv(int sockfd, void *vptr, size_t maxlen) {
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for(n = 1;n < maxlen;n++) {
again:
		if( (rc = read(sockfd, &c, 1)) == 1) {
			*ptr++ = c;
			if(c == '\n') break;
		} else if(rc == 0) {
			if(n == 1) return 0;
			else break;
		} else {
			if(errno == EINTR)
				goto again;
			return -1;
		}
	}
	*ptr = 0;
	
	return n;
}
ssize_t dbrecv(int sockxfd,char buf[MAXLINE+1]) {
	ssize_t n;

	n=recv(sockxfd,buf,MAXLINE,0);
	buf[n-1] = 0;

	return n-1;
}
char *strtolower2(char *str) {
	char *ptr;

	for(ptr = str;*ptr != '\0';ptr++,str++)
		*ptr = tolower(*str);
	ptr = str;

	return ptr;
}
