#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <time.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "config.h"

#define	VERSION		"1.0"
#define	MAXLINE		140
#define	LINE		30
#define	SSIZE		15

#define	NORMAL		0
#define	OPER		1
#define	ADMIN		2

#define	MAXSEEN		5000
#define	INSTALLDIR	".."

void qerr(const char*,...);
void delaydots(int);
int dbsend(int,const char*,...);
int args(char[],char[][]);
int putword(char*,FILE*);
void trim(char[],char);
void addlinetofile(FILE*,char[],int);
void copyfile(char[],char[]);
char *dhms(time_t);
void respawn(int);
char *killstring(char[],char[]);
char *randnick(void);
char *strtoupper(char*);
void strtolower(char**);
char *lowfunc(char*);
char *strtolower2(char*);
int cmdexist(char*);
void makelower(char[][],int);
void addchantoarr(char[]);
void delchanfromarr(char[]);
int chanin(char[]);
char *DelNumWords(char*,int);
long getsize(char*);
int getfreeport(void);
void clearport(int);
void dccsend(char*,char*);
void runthread(char*,void* (*fcn)(void*));
ssize_t ddsend(int,const char*,...);
void slowdsend(int,const char*,...);
void *sendfunc(void*);
static void *senddata(void*);
void *xsenddata(void*);
long getlongip(void);
int pfile(char*);
int dirwalk(char*,int (*fcn)(char*));
int adminaccess(char*);
int addadmin(char*);
char *replace(char*,char);
void saveme(int);
void botnetrc(void);
void server_init(void);

void launch(void);
void start(void);
void prep(void);
char *getnick(char[]);
char *getchannel(char[][]);
int userexist(char[]);
int rightpass(char[],char[],FILE*);
int useraccess(char*);
int onlyuseraccess(char*);
void adduser(char[],char[]);
int deluser(char*);
void update_file(long,FILE*,char[],int);
void setpasswd(char*,char*);

void sendmemo(char[],char[],char[],int);
void readmemo(int,char[],char[],int);
void listmemos(int,char[]);
void deletememo(int,char[],char[],int);
void checknew(int,char[]);
int getrecordofdoublememo(char[]);

void seen(int,char[],char[]);
void addtostruct(char[],char[]);
void initseenstructs(void);
void killseen(char[],char[]);
void setseen(char[],char[]);
struct channel *getchan(char[]);
int isinseen(char[],char[]);
void cleanallseen(char[]);

void *showhelpall(void *arg);
void displayhelp(int,char*);
void showhelp(char*,int,char[][]);

char *parsemsg(char*);
void cleanmsg(char*);
void update_me(void);

void botinfo(char*);

void addinfo(char*,char[]);
void delinfo(char*);
char *readinfo(char*);

struct nick {
	char name[SSIZE];
	int flags;
};
struct seen_nick {
	char name[SSIZE];
	time_t parttime;
};
struct channel {
	char name[SSIZE];
	struct seen_nick snicks[MAXSEEN];
};
typedef struct t_SENDDATA {
	int listenfd;
	char *filename;
} SENDDATA;
typedef struct t_XSENDDATA {
	int sockfd;
	char *buf;
} XSENDDATA;
typedef struct t_THREADDATA {
	char *n;
	char botnick[LINE];
} THREADDATA;
typedef struct t_SLOWDATA {
	int sockfd;
	char *buf;
} SLOWDATA;
int sockfd,dbfd,dbconnect,counter;
FILE *usersfp;

struct nick users[MAXUSERS];
