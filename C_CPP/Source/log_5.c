#include "../include/bot.h"

char join_channels[MAXCHANS][SSIZE];

int sockfd, logsw;
FILE *usersfp, *logfp;

char botnick[LINE],server[LINE],port[SSIZE];

struct nick users[MAXUSERS];

char *parsemsg(char *msgcmd) {
	int i,j,cwords;
	time_t ticks;
	char *nick, *chan, *kicknick, *timestr, *curtime, *ptr;
	char cargs[50][MAXLINE];
	char msg[2048-LINE], ret[2048], ret2[2048+LINE];

	memset(ret,'\0',sizeof(ret));
	cwords = args(msgcmd,cargs);
	ticks = time(NULL);
	
	if(!strcmp(cargs[1],"PRIVMSG")) {
		if(chanin(cargs[2])) {
			nick = getnick(msgcmd);
			chan = getchannel(cargs);
			memset(msg,'\0',sizeof(msg));
			if(strstr(cargs[3],"ACTION") == NULL) {
				for(i = 0, j = 3;i <= cwords-3-1;i++, j++) {
					if(j == 3) {
						if(cargs[j][0] == ' ')
							cargs[j][0] = '\0';
						else cargs[j][0] = ' ';
					}
					strncat(msg,cargs[j],sizeof(msg));
					if(i < cwords-3-1) strncat(msg," ",sizeof(msg));
				}
				snprintf(ret,sizeof(ret),"(%s)/<%s>%s\n",chan,nick,msg);
			} else {
				for(i = 0, j = 4;i <= cwords-4-1;i++, j++) {
					if(j == 4)
						if(cargs[j][0] == ' ')
							cargs[j][0] = '\0';
					strncat(msg,cargs[j],sizeof(msg));
					if(i < cwords-4-1) strncat(msg," ",sizeof(msg));
				}
				cleanmsg(msg);
				snprintf(ret,sizeof(ret),"(%s)/* %s %s\n",chan,nick,msg);
			}
		}
	} else if(!strcmp(cargs[1],"PART")) {
		if(chanin(cargs[2])) {
			nick = getnick(msgcmd);
			chan = getchannel(cargs);
			snprintf(ret,sizeof(ret),"*** %s has left channel %s\n",nick,chan);
		}
	} else if(!strcmp(cargs[1],"JOIN")) {
		nick = getnick(msgcmd);
		chan = getchannel(cargs);
		trim(chan,':');
		snprintf(ret,sizeof(ret),"*** %s has joined channel %s\n",nick,chan);
	} else if(!strcmp(cargs[1],"QUIT")) {
		nick = getnick(msgcmd);
		memset(msg,'\0',sizeof(msg));
		for(i = 0, j = 2;i <= cwords-2-1;i++, j++) {
			strncat(msg,cargs[j],sizeof(msg));
			if(i < cwords-2-1) strncat(msg," ",sizeof(msg));
		}
		msg[0] = ' ';
		snprintf(ret,sizeof(ret),"*** %s has quit IRC (%s)\n",nick,msg);
	} else if(!strcmp(cargs[1],"MODE")) {
		nick = getnick(msgcmd);
		chan = getchannel(cargs);
		memset(msg,'\0',sizeof(msg));
		for(i = 0, j = 3;i <= cwords-3-1;i++, j++) {
			if(j == 3)
				if(cargs[j][0] == ' ')
					cargs[j][0] = '\0';
			strncat(msg,cargs[j],sizeof(msg));
			if(i < cwords-3-1) strncat(msg," ",sizeof(msg));
		}
		snprintf(ret,sizeof(ret),"(%s)/* %s sets mode: %s\n",chan,nick,msg);
		cleanmsg(ret);
		strncat(ret,"\n",sizeof(ret));
	} else if(!strcmp(cargs[1],"KICK")) {
		nick = getnick(msgcmd);
		chan = getchannel(cargs);
		memset(msg,'\0',sizeof(msg));
		kicknick = cargs[3];
		for(i = 0,j = 4;i <= cwords-4-1;i++, j++) {
			if(j == 3) {
				if(cargs[j][0] = ' ')
					cargs[j][0] = '\0';
				else cargs[j][0] = ' ';
			}
			strncat(msg,cargs[j],sizeof(msg));
			if(i < cwords-4-1) strncat(msg," ",sizeof(msg));
		}
		msg[0] = ' ';
		snprintf(ret,sizeof(ret),"*** %s was kicked off %s by %s (%s)\n",kicknick,chan,nick,msg);
	}
	curtime = ctime(&ticks);
	ptr = strtok(DelNumWords(curtime,3), " ");
	ptr[5] = '\0';
	strncpy(ret2, "[", sizeof(ret2));
	strncat(ret2, ptr, sizeof(ret2));
	strncat(ret2, "] ", sizeof(ret2));
	strncat(ret2, ret, sizeof(ret2));
		
	return strdup(ret2);
}
void cleanmsg(char *msg) {
	int i;

	for(i = 0;i <= strlen(msg)-1;i++)
		if(!isprint( *(msg+i) )) *(msg+i) = ' ';

	return;
}
