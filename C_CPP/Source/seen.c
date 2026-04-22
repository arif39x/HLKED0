#include "../include/bot.h"

char join_channels[MAXCHANS][SSIZE];

char botnick[LINE];

static struct seen_nick snicks[MAXSEEN];
struct channel schannels[MAXCHANS];

void seen(int sockfd,char snick[SSIZE],char channel[SSIZE]) {
	char *timestr;
	int i,found;
	time_t lasttime;
	char savenick[SSIZE];
	struct channel *thischan;

	found = 0;
	strncpy(savenick,snick,sizeof(savenick));
	strtolower(&snick);
	timestr = (char*)malloc(LINE*2);
	thischan = getchan(channel);
	if(!strcmp(snick,botnick)) {
		dsend(sockfd,"PRIVMSG %s :Trying to be funny now, aren't we?\n",channel);
		return;
	}
	for(i=0;i<=MAXSEEN-1;i++)
		if(thischan->snicks[i].parttime >= 0)
			if(!strcmp(thischan->snicks[i].name,snick)) {
				if(thischan->snicks[i].parttime > 0) lasttime = time(0) - thischan->snicks[i].parttime;
				found = 1;
				break;
			}
	if(!found) {
		dsend(sockfd,"PRIVMSG %s :I have not seen %s before, but then again, i'm blind.\n",channel,savenick);
		return;
	} else if(thischan->snicks[i].parttime != 0) {
		timestr = dhms(lasttime);
		dsend(sockfd,"PRIVMSG %s :I last saw %s %s ago.\n",channel,savenick,timestr);
	} else
		dsend(sockfd,"PRIVMSG %s :%s is currently on the channel!\n",channel,savenick);
	
	return;
}
void addtostruct(char snick[SSIZE],char channel[SSIZE]) {
	int i;
	struct channel *thischan;
	
	thischan = getchan(channel);
	for(i=0;i<=MAXSEEN-1;i++)
		if(thischan->snicks[i].parttime < 0) {
			strncpy(thischan->snicks[i].name,snick,sizeof(thischan->snicks[i].name));
			thischan->snicks[i].parttime = 0;
			break;
		}

	return;
}
void initseenstructs(void) {
	int i,j;

	strncpy(schannels[0].name,join_channels[0],sizeof(schannels[0].name));
	for(i = 1;i <= MAXCHANS-1;i++)
		strcpy(schannels[i].name,"");
	for(j=0;j<=MAXCHANS-1;j++)
		for(i=0;i<=MAXSEEN-1;i++)
			schannels[j].snicks[i].parttime = (-1);

	return;
}
void killseen(char snick[SSIZE],char channel[SSIZE]) {
	int i;
	struct channel *thischan;

	thischan = getchan(channel);
	for(i=0;i<=MAXSEEN-1;i++)
		if(thischan->snicks[i].parttime >= 0)
			if(!strcmp(thischan->snicks[i].name,snick)) {
				thischan->snicks[i].parttime = (-1);
				break;
			}

	return;
}
void setseen(char snick[SSIZE],char channel[SSIZE]) {
	int i;
	struct channel *thischan;

	thischan = getchan(channel);
	for(i=0;i<=MAXSEEN-1;i++)
		if(thischan->snicks[i].parttime >= 0)
			if(!strcmp(thischan->snicks[i].name,snick)) {
				thischan->snicks[i].parttime = time(0);
				break;
			}

	return;
}
struct channel *getchan(char channel[SSIZE]) {
	int i;
	struct channel *thischan;

	thischan = malloc(sizeof(struct channel));
	bzero(&thischan,sizeof(thischan));
	for(i = 0;i <= MAXCHANS-1;i++)
		if(strcmp(schannels[i].name,"")) {
			if(!strcmp(schannels[i].name,channel)) {
				thischan = &schannels[i];
				return thischan;
			}
		}

	return thischan;
}
int isinseen(char snick[SSIZE],char channel[SSIZE]) {
	int i,found = 0;
	struct channel *thischan;

	thischan = getchan(channel);
	for(i=0;i<=MAXSEEN-1;i++) {
		if(thischan->snicks[i].parttime >= 0)
    	        	if(!strcmp(thischan->snicks[i].name,snick)) {
				found = 1;
				break;
			}
	}

	return found;
}
void cleanallseen(char snick[SSIZE]) {
	int i,j;

	for(j=0;j<=MAXCHANS-1;j++)
		for(i=0;i<=MAXSEEN-1;i++)
			if(!strcmp(schannels[j].snicks[i].name,snick))
				schannels[j].snicks[i].parttime = time(0);

	return;
}
