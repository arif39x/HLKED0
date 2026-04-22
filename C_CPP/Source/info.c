#include "../include/bot.h"

int sockfd;
FILE *usersfp;

struct nick users[MAXUSERS];

char botnick[LINE],server[LINE],port[SSIZE];

void addinfo(char *itemname,char info[4096]) {
	char mffile[MAXLINE];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/infos/%s.itm",INSTALLDIR,itemname);

	creat(mffile,0600);
	fp = fopen(mffile,"r+");
	fputs(info,fp);

	fclose(fp);

	return;
}
void delinfo(char *itemname) {
	char mffile[MAXLINE];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/infos/%s.itm",INSTALLDIR,itemname);

	remove(mffile);

	return;
}
char *readinfo(char *itemname) {
	int i,c;
	char mffile[MAXLINE];
	char str[4096];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/infos/%s.itm",INSTALLDIR,itemname);

	i = 0;
	if( (fp = fopen(mffile,"r")) != NULL) {
		while( (c = getc(fp)) != EOF) 
			str[i++] = c;
		str[i] = '\0';
		fclose(fp);
	} else
		return (NULL);
		//snprintf(str,sizeof(str),"Info item %s does not exist.",itemname);

	return strdup(str);
}
