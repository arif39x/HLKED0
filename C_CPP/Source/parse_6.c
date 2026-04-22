#include "../include/bot.h"

char join_channels[MAXCHANS][SSIZE];

char botnick[LINE],server[LINE],port[SSIZE];

int sockfd;
FILE *usersfp;

struct nick users[MAXUSERS];

void parse(int dbfd,char cmd[1024+SSIZE]) {
	int i,j;
	char c;
	char tmp[MAXLINE],mffile[MAXLINE];
	char user[LINE],pass[LINE],memo[1024],from[LINE],record[SSIZE];

	i = j = 0;
	while( (c=cmd[i++]) != ';')
		tmp[j++] = c;
	tmp[j] = '\0';
	j = 0;
	if(!strcmp(tmp,"ADDUSER")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			pass[j++] = c;
		pass[j] = '\0';
		if(!userexist(user)) adduser(user,pass);
	} else if(!strcmp(tmp,"DELUSER")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		if(userexist(user)) deluser(user);
	} else if(!strcmp(tmp,"SET_PASSWD")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			pass[j++] = c;
		pass[j] = '\0';
		deluser(user);
		adduser(user,pass);
	} else if(!strcmp(tmp,"MEMO_SEND")) {
		while( (c=cmd[i++]) != ';') 
			from[j++] = c;
		from[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			memo[j++] = c;
		memo[j] = '\0';
		sendmemo(user,memo,from,1);
	} else if(!strcmp(tmp,"MEMO_READ")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			record[j++] = c;
		record[j] = '\0';
		readmemo(sockfd,record,user,1);
	} else if(!strcmp(tmp,"MEMO_DELETE")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			record[j++] = c;
		record[j] = '\0';
		deletememo(sockfd,user,record,1);
	} else if(!strcmp(tmp,"MEMO_CLEAR_ALL")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,user);
		remove(mffile);
	} else if(!strcmp(tmp,"GLOBAL_COMMAND")) {
		while( (c=cmd[i++]) != ';')
			from[j++] = c;
		from[j] = '\0';
		dsend(sockfd,"%s\n",from);
	} else if(!strcmp(tmp,"NUMBOTS")) {
		while( (c=cmd[i++]) != ';')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			from[j++] = c;
		from[j] = '\0';
		j = 0;
		while( (c=cmd[i++]) != ';')
			record[j++] = c;
		record[j] = '\0';
		if(!strcmp(from,botnick))
			if(atoi(record) == 1)
				dsend(sockfd,"NOTICE %s :I am currently the only bot connected to this botnet.\n",user);
			else
				dsend(sockfd,"NOTICE %s :There are %s bots connected to this botnet.\n",user,record);
	} else if(!strcmp(tmp,"UPDATE_ME"))
		update_me();
	
	return;
}
void update_me(void) {
	int i, j;
	char c;
	char user[SSIZE], pass[SSIZE], line[MAXLINE];

	rewind(usersfp);
	while(fgets(line,MAXLINE,usersfp) != NULL) {
		i = j = 0;
		while( (c=line[i++]) != ':')
			user[j++] = c;
		user[j] = '\0';
		j = 0;
		while( (c=line[i++]) != '\n')
			pass[j++] = c;
		pass[j] = '\0';
		dbsend(dbfd,"ADDUSER;%s;%s;\n",user,pass);
	}
	rewind(usersfp);

	return;
}
