/* The BotNET project began on 08/03/2001 under the codename 'aibot'
 *
 * This program was written by Boaz Barak <bozy@newmail.net>
 * This program is under the GNU Public License.
 * 
 */

#include "../include/bot.h"

const int portsrange = 2500 - 2010;
static int keeplist;
static char *g_nick, *g_find;
static FILE *mfp;

int ndone = 1;
pthread_mutex_t datamutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t datacond = PTHREAD_COND_INITIALIZER;

char join_channels[MAXCHANS][SSIZE];
char db[MAXLINE], dbport[SSIZE];

char botnick[LINE],server[MAXLINE],port[SSIZE];
struct channel schannels[MAXCHANS];

static char tcmd[MAXLINE];
static int usedports[2500 - 2010];
static char *commands[30] = { ":identify", ":set", ":mode", ":kick", ":invite", ":join", ":part", ":memo", ":adduser", ":deluser", ":status", ":addinfo", ":delinfo", ":get", ":list", ":getlist", ":register", ":access", ":raw", NULL };

int main(int argc,char **argv) {
	int i;
	int Nvalue = 0, Rvalue = 0;
	pid_t pid;
	char c;
	char prog[MAXLINE];
	char *nvalue, *svalue, *pvalue, *Dvalue, *Pvalue, *cvalue, *Fvalue;

	nvalue = svalue = pvalue = Dvalue = Pvalue = cvalue = Fvalue = NULL;
	dbconnect = 1;
	srand(time(0));
	while( (c = getopt(argc,argv,"n:s:p:D:P:F:c:NR")) != -1) {
		switch(c) {
			case 'n':
				nvalue = optarg;
				break;
			case 's':
				svalue = optarg;
				break;
			case 'p':
				pvalue = optarg;
				break;
			case 'D':
				Dvalue = optarg;
				break;
			case 'P':
				Pvalue = optarg;
				break;
			case 'N':
				Nvalue = 1;
				break;
			case 'R':
				Rvalue = 1;
				break;
			case 'c':
				cvalue = optarg;
				break;
			case 'F':
				Fvalue = optarg;
				break;
			default:
				fprintf(stderr,"Invalid option -%c\n",optopt);
				exit(-1);
				break;
		}
	}
	if(nvalue != NULL) strcpy(botnick,nvalue);
	else strcpy(botnick,BOTNICK);
	if(svalue != NULL) strcpy(server,svalue);
	else strcpy(server,SERVER);
	if(pvalue != NULL) strcpy(port,pvalue);
	else strcpy(port,PORT);
	if(Dvalue != NULL && !Nvalue) strncpy(db,Dvalue,sizeof(db));
	else strncpy(db,DATABASE,sizeof(db));
	if(Pvalue != NULL) strncpy(dbport,Pvalue,sizeof(dbport));
	else strncpy(dbport,SERVPORT,sizeof(dbport));
	if(cvalue != NULL) strcpy(join_channels[0],cvalue);
	else strcpy(join_channels[0],CHANNEL);
	if(Rvalue)
		strcpy(botnick,randnick());
	if(Fvalue != NULL) {
		snprintf(prog,sizeof(prog),"%s/bin/bot",INSTALLDIR);
		for(i = 1;i <= atoi(Fvalue);i++) {
			if( (pid = fork()) == 0)
				if(strlen(db) > 2 && !Nvalue)
					execl(prog, "BotNET", "-R", "-D", db, (char*) 0);
				else
					execl(prog, "BotNET", "-R");
			sleep(1);
		}
	}
	printf("BotNET %s %s. (%s)\n",VERSION,(strlen(db)>2&&!Nvalue)?"Running in BotNET Mode":"Running in Stand-alone mode",botnick);
	for(i = 0;i < portsrange;i++)
		usedports[i] = 0;
	counter = 0;
	start();
	/*printf("Launching into background.\n");
	if( (pid=fork())==0) launch();*/

	return 0;
}
void start(void) {
	int n,j,i = 0;
	struct addrinfo hints,*res,*res2;
	struct sockaddr_in myaddr;
	struct hostent *hptr;
	fd_set rset;

	if(MAXCHANS > 20) {
		fprintf(stderr,"MAXCHANS cannot exceed the limit of '20' <include/config.h>\n");
		exit(-1);
	}
	for(i = 1;i <= MAXCHANS-1;i++)
		strcpy(join_channels[i],"");
	if( (sockfd=socket(AF_INET,SOCK_STREAM,0))<0) {
			perror("socket creation error");
			exit(-1);
	}
	signal(SIGPIPE,saveme);
	bzero(&hints,sizeof(hints));
	//hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_IP;
	bzero(&myaddr,sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(atoi(port));
	bind(sockfd,(struct sockaddr*)&myaddr,sizeof(myaddr));
	//bind(sockfd,&hints.ai_addr,sizeof(hints.ai_addr));
	n = getaddrinfo(server,port,&hints,&res);
	printf("Connecting to %s:%s ",server,port);
	delaydots(11);
	if(connect(sockfd,res->ai_addr,res->ai_addrlen)<0) {
		fprintf(stderr,"error connecting to server: %s\n",server);
		exit(-1);
	}
	printf("Connected.\n");
	FD_ZERO(&rset);
	for(;;) {
		FD_SET(sockfd,&rset);
		if(select(sockfd+1,&rset,NULL,NULL,NULL)<0) {
			perror("select error");
			exit(-1);
		}
		if(FD_ISSET(sockfd,&rset)) {
			printf("Registering to server and attempting to join channel %s ",join_channels[0]);
			delaydots(11);
			dsend(sockfd,"USER %s\n",USER);
			dsend(sockfd,"NICK %s\n",botnick);
			dsend(sockfd,"MODE %s +i\n",botnick);
			i = 0;
			strtolower2(botnick);
			makelower(join_channels,MAXCHANS);
			dbfd = -1;
			break;
		}
	}
			if(strlen(db) > 0 && dbconnect) {
				if( (dbfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
					perror("socket creation error");
					exit(-1);
				}
				bzero(&hints,sizeof(hints));
				//hints.ai_flags = AI_CANONNAME;
				hints.ai_family = AF_INET;
				hints.ai_socktype = SOCK_STREAM;
				//hints.ai_protocol = IPPROTO_IP;
				n = getaddrinfo(db,dbport,&hints,&res2);
				printf("Connecting to Database %s:%s ",db,dbport);
				delaydots(11);
				if(connect(dbfd,res2->ai_addr,res2->ai_addrlen)<0) {
					printf("error connecting to database server: %s\n",db);
					dbfd = -1;
					strncpy(db,"",sizeof(db));
					printf("Switching to Stand-alone mode.\n");
				} else
					dbsend(dbfd,"UPDATE_ME;\n");
				printf("Connected.\n");
				dbconnect = 0;
			}
			printf("All done.\n");
			printf("Launching into background.\n");
			if(fork() == 0) {
				launch();
			} else {
				return;
			}
	/*strtolower2(botnick);
	makelower(join_channels,MAXCHANS); */

	return;
}
char *getnick(char cmd[MAXLINE]) {
	char *ptr,*nick;
	
	strncpy(tcmd,cmd,sizeof(tcmd));
	ptr = strtok(tcmd,"!");
	nick = (ptr+1);
	
	return nick;
}
char *getchannel(char cargs[50][MAXLINE]) {
	char *chan;

	chan = cargs[2];

	return strdup(chan);
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
	for(i=0;i<=MAXUSERS-1;i++)
		users[i].flags = NORMAL;
	
	return;
}
int useraccess(char *nick) {
	int i;
	int access = 0;

	for(i=0;i<=MAXUSERS-1;i++)
		if(users[i].flags==OPER || users[i].flags==ADMIN)
			if(!strcmp(users[i].name,nick)) {
				access = 1;
				break;
			}

	return access;
}
int onlyuseraccess(char *nick) {
	int i;
	int access = 0;

	for(i=0;i<=MAXUSERS-1;i++)
		if(users[i].flags==OPER)
			if(!strcmp(users[i].name,nick)) {
				access = 1;
				break;
			}

	return access;
}
void adduser(char user[LINE],char pass[LINE]) {
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
		update_file((long)record,usersfp,"/db/users",1);
		snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,user);
		remove(mffile);
		return record;
	}
}
void update_file(long record,FILE *fp,char filename[LINE*2],int reopen) {
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

	if(reopen == 1)
		if( (fp=fopen(dbffile,"r+"))==NULL)
			qerr("Unable to open database file: %s\n",dbffile);

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
		usleep(7000);
		printf(".");
		fflush(stdout);
	}
	printf("\n");

	return;
}
ssize_t writen(int fd, const void *vptr, size_t n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if( (nwritten = write(fd,ptr,nleft)) <= 0) {
			if(errno == EINTR)
				nwritten = 0;
			else
				return (-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
void runthread(char *n,void* (*fcn)(void *arg)) {
	pthread_t tid;
	THREADDATA *td;

	td = (THREADDATA*)malloc(sizeof(THREADDATA)+LINE*2);
	td->n = n;
	strncpy(td->botnick,botnick,sizeof(botnick));
	//td->botnick = botnick;

	pthread_create(&tid,NULL,fcn,(void*)td);

	return;
}
ssize_t dsend(int sockxfd,const char *fmt,...) {
	pid_t pid;
        ssize_t n;
	//pthread_t tid;
        va_list args;
        char buf[1024];
	//XSENDDATA data;

        va_start(args,fmt);
        vsnprintf(buf,sizeof(buf),fmt,args);
        va_end(args);

        /*n=send(sockxfd,buf,strlen(buf),0);
        data.buf = buf;
	data.sockfd = sockxfd;
	pthread_create(&tid,NULL,xsenddata,&data);*/
	
	n=writen(sockxfd,buf,strlen(buf));

        return 1;
}
ssize_t ddsend(int sockxfd,const char *fmt,...) {
	/* use this function only in a threaded procedure */
	usleep(HELP_DELAY);
	/* delay avoids msg clogging by some ircd settings */
	return dsend(sockxfd,fmt);
} // delayed dsend()
void slowdsend(int sockxfd,const char *fmt,...) {
	va_list args;
	char buf[1024];
	pthread_t tid;
	SLOWDATA *sd; 
	
	va_start(args,fmt);
	vsnprintf(buf,sizeof(buf),fmt,args);
	va_end(args);

	/*
	sd = (SLOWDATA*)malloc(sizeof(SLOWDATA)+LINE*2);
	sd->sockfd = sockxfd;
	sd->buf = buf;

	pthread_create(&tid,NULL,sendfunc,(void*)sd);
	*/
	usleep(HELP_DELAY / 2);
	writen(sockxfd,buf,strlen(buf));

	return;
}
void *sendfunc(void *arg) {
	SLOWDATA *sd;
	SLOWDATA newsd;
	
	sd = (SLOWDATA*)arg;
	newsd.sockfd = sd->sockfd;
	newsd.buf = sd->buf;
	free(sd);

	pthread_mutex_lock(&datamutex);
	while(ndone == 0)
		pthread_cond_wait(&datacond,&datamutex);
	ndone = 0;
	writen(newsd.sockfd,newsd.buf,strlen(newsd.buf));
	//usleep(1900000); // 1.9s
	ndone = 1;
	pthread_cond_signal(&datacond);
	pthread_mutex_unlock(&datamutex);

	return (NULL);
}
void *xsenddata(void *arg) {
	XSENDDATA *data = (XSENDDATA*)arg;

	writen(data->sockfd,data->buf,strlen(data->buf));

	return (NULL);
}
int dbsend(int sockxfd,const char *fmt,...) {
	int n;
	va_list args;
	char buf[1024];

	if(!strcmp(db,"")) return 0;

	va_start(args,fmt);
	vsnprintf(buf,sizeof(buf),fmt,args);
	va_end(args);

	n=write(sockxfd,buf,strlen(buf)+1);

	return n;
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
ssize_t drecvn(int sockfd,char buf[MAXLINE+1]) {
        ssize_t n;

        n=recv(sockfd,buf,MAXLINE,0);
        buf[n-2] = 0;
	dsend(sockfd,"\n");

        return n-2;
}
int args(char cmd[2048],char cargs[50][MAXLINE]) {
        int i,j,k,cwords;
        char word[MAXLINE+1];
        char c;
	
	i=cwords=0;
        while( (c=cmd[i++])!='\0')
                if(isspace(c)) cwords++;
        // cwords++;
	if(cwords > 50) cwords = 50;
        for(i=1,k=0;i<=cwords;i++) {
                for(j=0;!isspace(cmd[k]) && j < MAXLINE;)
                        word[j++] = cmd[k++];
                word[j] = '\0';
		if(j == 0) {
			strncpy(cargs[i-1]," ",sizeof(cargs[i-1]));
			return cwords;
		}
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
	int endsect = 0;
	char buf[MAXLINE], tmpbuf[MAXLINE], ffile[MAXLINE], ffile2[MAXLINE];
	FILE *tmp, *tmp2;

	snprintf(ffile,sizeof(ffile),"%s/db/%s",INSTALLDIR,"001");
	snprintf(ffile2,sizeof(ffile2),"%s/db/%s",INSTALLDIR,"002");
	creat(ffile,0600);
	creat(ffile2,0600);
	if( (tmp = fopen(ffile,"r+")) == NULL) return;
	if( (tmp2 = fopen(ffile2,"r+")) == NULL) return;

	rewind(fp);
	rewind(tmp);
	rewind(tmp2);
	while(fgets(buf,MAXLINE,fp)!=NULL) {
		if(i != record)
			if(!endsect)
				fputs(buf,tmp);
			else
				fputs(buf,tmp2);
		else if(i == record) {
			rewind(tmp);
			while(fgets(tmpbuf,MAXLINE,tmp)!=NULL)
				fputs(tmpbuf,tmp2);
			fputs(line,tmp2);
			endsect = 1;
		}
		i++;
	}
	fflush(tmp);
	fflush(tmp2);
	rewind(fp);
	rewind(tmp2);
	while(fgets(buf,MAXLINE,tmp2)!=NULL)
		fputs(buf,fp);

	// clean
	rewind(fp);
	fclose(tmp);
	fclose(tmp2);
	remove(ffile);
	remove(ffile2);

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
	pid_t pid;
	char prog[MAXLINE];

	snprintf(prog,sizeof(prog),"%s/bin/bot",INSTALLDIR);
	if( (pid = fork()) == 0) execl(prog, "BotNET", (char*) 0);

	exit(-1);
}
char *killstring(char string[1024+SSIZE],char erase[LINE]) {
        int i;
        char newstr[1024+SSIZE];

	if(strstr(string,erase) != NULL)
	        for(i = 0;i <= (strlen(string) - strlen(erase))-2;i++)
			newstr[i] = string[i];
	else return strdup(string);
	newstr[i] = '\0';
		
        return strdup(newstr);
}
char *randnick(void) {
	long num;
	char *str,*tstr;

	str = (char*)malloc(10);
	num = 50000+rand()%(100000-50000+1);
	strcpy(str,"Bot-");
	snprintf(tstr,sizeof(str),"%x",num);
	strcat(str,tstr);

	return str;
}
char *strtoupper(char *str) {
	char *ptr;

	for(ptr = str;*ptr != '\0';ptr++,str++)
		*ptr = toupper(*str);
	ptr = str;
	
	return ptr;
}
char *strtolower2(char *str) {
	char *ptr;

	for(ptr = str;*ptr != '\0';ptr++,str++)
		*ptr = tolower(*str);
	ptr = str;
	
	return ptr;
}
char *lowfunc(char *str) {
	int i = 0;
	char new[MAXLINE];

	while(*str != '\0')
		new[i++] = tolower(*str++);
	new[i] = '\0';

	return strdup(new);
}
void strtolower(char **str) {
	char *ptr;
	char strsave[MAXLINE];

	strncpy(strsave,*str,sizeof(strsave));

	ptr = lowfunc(strsave);
	*str = ptr;

	return;
}
int cmdexist(char *cmd) {
	int i,test;
	char *str;

	i = test = 0;
	while( (str = commands[i++]) != NULL)
		if(!strcmp(str,cmd)) {
			test = 1;
			break;
		}

	return test;
}
void makelower(char cargs[50][MAXLINE],int cwords) {
	int i;

	for(i = 0;i <= cwords-1;i++)
		strtolower2(cargs[i]);

	return;
}
void addchantoarr(char channel[SSIZE]) {
	int i;

	for(i = 0;i <= MAXCHANS-1;i++) {
		if(!strcmp(join_channels[i],"")) {
			strcpy(join_channels[i],channel);
			break;
		}
	}
	for(i = 0;i <= MAXCHANS-1;i++)
		if(!strcmp(schannels[i].name,"")) {
			strncpy(schannels[i].name,channel,sizeof(schannels[i].name));
			break;
		}

	return;
}
void delchanfromarr(char channel[SSIZE]) {
	int i;

	for(i = 0;i <= MAXCHANS-1;i++)
		if(strcmp(join_channels[i],""))
			if(!strcmp(join_channels[i],channel)) {
				strcpy(join_channels[i],"");
				break;
			}
	for(i = 0;i <= MAXCHANS-1;i++)
		if(strcmp(schannels[i].name,""))
			if(!strcmp(schannels[i].name,channel)) {
				strcpy(schannels[i].name,"");
				break;
			}

	return;
}
int chanin(char channel[SSIZE]) {
	int i;

	if(strlen(channel) <= 0 || !strcmp(channel,botnick)) return 0;
	for(i = 0;i <= MAXCHANS-1;i++)
		if(strlen(join_channels[i]) > 0 && join_channels[i] != NULL) {
			if(!strcmp(channel,join_channels[i]))
				return 1;
		}
	return 0;
}
char *DelNumWords(char *str, int num) {
	int i, len;
	char *ptr;
	char strstr[128];

	for(i = 1;i <= num;i++) {
		strncpy(strstr,str,sizeof(strstr));
		len = strlen(strtok(strstr," ")) + 1;
		ptr = str + len;
		str += len;
	}

	return ptr;
}
long getsize(char *filename) {
	long size;
	char c;
	FILE *fp;

	fp = fopen(filename,"r");

	size = 0L;
	while(fread(&c,1,1,fp) > 0)
		size++;

	fclose(fp);

	return size;
}
int getfreeport(void) {
	int i, port;
	const int startport = 2010;

	for(i = 0;i < portsrange;i++)
		if(usedports[i] == 0) {
			usedports[i] = startport + i;
			return usedports[i];
		}
	for(i = 0;i < portsrange;i++)
		if(usedports[i] != 0) {
			usedports[i] = startport + i;
			usedports[i+1] = 0;
			return usedports[i];
		}

	return 0; // false: impossibility
}
void clearport(int port) {
	int i;

	for(i = 0;i < portsrange;i++)
		if(usedports[i] == port) {
			usedports[i] = 0;
			return;
		}

	return;
}
void dccsend(char *nick,char *filename) {
	int i,nport;
	long filesize, ip;
	int connfd;
	const int on = 1;
	pthread_t tid;
	char buf[MAXLINE];
	struct sockaddr_in myaddr;
	SENDDATA sdata;

	filesize = getsize(filename);
	sdata.filename = filename;

resend:
	if(sdata.listenfd) close(sdata.listenfd);
	nport = getfreeport();
	ip = getlongip();

	sdata.listenfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&myaddr,sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(nport);
	setsockopt(sdata.listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(bind(sdata.listenfd,(struct sockaddr*)&myaddr,sizeof(myaddr)) < 0)
		goto resend;

	listen(sdata.listenfd,0);

	// initiate CTCP DCC request
	dsend(sockfd,"PRIVMSG %s :\001DCC SEND %s %ld %d %ld\001\n",nick,replace(strrchr(filename,'/')+1,'_'),ip,nport,filesize);

	// spawn thread to send file data
	pthread_create(&tid,NULL,&senddata,&sdata);

	//clearport(nport);

	return;
}
static void *senddata(void *arg) {
	int connfd;
	size_t nbytes;
	char c;
	char buf[1024];
	FILE *fp;
	SENDDATA *sdata = (SENDDATA*)arg;

	if( (fp = fopen(sdata->filename,"r")) == NULL)
		return (NULL);
	
	connfd = accept(sdata->listenfd,(struct sockaddr*)NULL,NULL);
	close(sdata->listenfd);

	while( (nbytes = fread(buf,1,1024,fp)) != 0)
		send(connfd,buf,nbytes,0);
	shutdown(connfd,SHUT_WR);
	//fclose(fp);
	//close(connfd);

	return (NULL);
}
long getlongip(void) {
	socklen_t len;
	struct sockaddr_in myaddr;

	len = 128;
	getsockname(sockfd,(struct sockaddr*)&myaddr,&len);

	return (long)htonl(myaddr.sin_addr.s_addr);
}
void sendlist(char *nick,char *find) {
	char mffile[MAXLINE];
	g_nick = nick;
	g_find = find;
	keeplist = 0;
	if(strcmp(g_find,"%all%")) {
		dsend(sockfd,"PRIVMSG %s :Listing up to 10 files based on the search \"%s\"\n",nick,g_find);
		dsend(sockfd,"PRIVMSG %s :------------->\n",nick);
		pfile(MP3DIR); // location of mp3 files
		if(!keeplist)
			dsend(sockfd,"PRIVMSG %s :Sorry, no files were found.\n",nick);
		dsend(sockfd,"PRIVMSG %s :-------------> Done.\n",nick);
	} else {
		snprintf(mffile,sizeof(mffile),"%s/db/list.txt",INSTALLDIR);
		creat(mffile,0600);
		mfp = fopen(mffile,"r+");
		if(pfile(MP3DIR)) {
			fclose(mfp);
			dccsend(nick,mffile);
		} else
			dsend(sockfd,"NOTICE %s :I am not sharing any files in %s\n",nick,MP3DIR);
	}

	return;
}
int pfile(char *name) {
	char str[128];
	char *name_bak;
	FILE *fp;
	struct stat stbuf;

	strncpy(str,name,sizeof(str));
	name_bak = str;
	strtolower2(name_bak);
	stat(name,&stbuf);

	if(name == MP3DIR)
		if( (stbuf.st_mode & S_IFMT) == S_IFDIR)
			dirwalk(name,pfile);
		else return 0; // error
	if(strcmp(g_find,"%all%")) {
		if(keeplist < 10) { // maximum songs to list
			if(strcmp(g_find,"*")) {
				if(strstr(name_bak,g_find) != NULL) {
					dsend(sockfd,"PRIVMSG %s :%s\n",g_nick,name+strlen(MP3DIR)+1);
					keeplist++;
				}
			} else {
				dsend(sockfd,"PRIVMSG %s :%s\n",g_nick,name+strlen(MP3DIR)+1);
				keeplist++;
			}
		}
	} else {
		fputs(name+strlen(MP3DIR)+1,mfp);
		fputc('\n',mfp);
	}

	return 1;
}
int dirwalk(char *dir,int (*fcn)(char*)) {
	char name[128];
	struct dirent *dp;
	DIR *dfd;

	dfd = opendir(dir);
	while( (dp = readdir(dfd)) != NULL) {
		if(!strcmp(dp->d_name,".") || !strcmp(dp->d_name,".."))
			continue;
		snprintf(name,sizeof(name),"%s/%s",dir,dp->d_name);
		(*fcn)(name);
	}
	closedir(dfd);

	return 1;
}
int adminaccess(char *nick) {
	int i, access;

	access = 0;
	for(i = 0;i <= MAXUSERS-1;i++)
		if(users[i].flags == ADMIN)
			if(!strcmp(users[i].name,nick)) {
				access = 1;
				break;
			}

	return access;
}
int addadmin(char *nick) {
	int i;

	if(adminaccess(nick)) return 0;
	
	for(i = 0;i <= MAXUSERS-1;i++)
		if(users[i].flags == NORMAL || users[i].flags == OPER) {
			users[i].flags = ADMIN;
			strncpy(users[i].name,nick,sizeof(users[i].name));
			break;
		}

	return 1;
}
char *replace(char *str,char c) {
	char *ptr, *ret;

	ptr = (char*)malloc(MAXLINE);
	ret = ptr;
	while(*str) {
		if(*str == ' ')
			*ptr = c;
		else
			*ptr = *str;
		ptr++;
		str++;
	}
	*ptr = '\0';

	return ret;
}
void saveme(int signo) {
	return;
}
void botnetrc(void) {
	char line[MAXLINE];
	FILE *fp;
	
	if( (fp = fopen("../botnetrc","r")) == NULL )
		return;

	while( fgets(line, MAXLINE, fp) != NULL )
	{
		dsend(sockfd, line);
	}
	fclose( fp );
}
void server_init(void) {
	int i = 0;

	while(strcmp(join_channels[i],""))
		dsend(sockfd,"JOIN %s\n",join_channels[i++]);

	botnetrc();
}
