#include "../include/bot.h"

int sockfd;
FILE *usersfp;

struct nick users[MAXUSERS];

char botnick[LINE],server[LINE],port[SSIZE];

void sendmemo(char nickname[SSIZE],char msg[1024],char from[SSIZE],int supress) {
	int nrecord;
	long rec;
	time_t ticks;
	char mffile[MAXLINE];
	char date[LINE*2],record[6];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	if( (fp = fopen(mffile,"a+"))==NULL) {
		creat(mffile,0600);
		fp = fopen(mffile,"a+");
	}
	fseek(fp,0L,SEEK_END);
	if(putword(from,fp)==EOF)
		qerr("Error writing to memo database: write access is required");
	putc('&',fp);
	ticks = time(0);
	snprintf(date,sizeof(date),"%s",ctime(&ticks));
	trim(date,'\n');
	putword(date,fp);
	putc('&',fp);
	putc('0',fp);
	putc('&',fp);
	cleanmsg(msg);
	fputs(msg,fp);
	putc('\n',fp);
	fclose(fp);

	if( (nrecord = getrecordofdoublememo(nickname)) != 0) {
		snprintf(record,sizeof(record),"%d",nrecord);
		deletememo(sockfd,nickname,record,1);
	}

	if(!supress) {
		dsend(sockfd,"NOTICE %s :Memo sent to user %s.\n",from,nickname);
		if(useraccess(nickname)) {
			rec = countmemos(nickname);
			dsend(sockfd,"NOTICE %s :You have a new memo at record %ld!\n",nickname,rec);
		}
	}

	return;
}
void readmemo(int sockfd,char number[SSIZE],char nickname[SSIZE],int supress) {
	int i = 1;
	int j,got;
	long num;
	char *ptr;
	char c,read;
	char memoline[MAXLINE],msg[1024],date[LINE*2],from[SSIZE];
	char mffile[MAXLINE],ffile[LINE*2];
	FILE *fp;

	got = 1;
	if( (num=atol(number))<=0) {
		dsend(sockfd,"NOTICE %s :Invalid record number\n",nickname);
		return;
	}
	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	snprintf(ffile,sizeof(ffile),"/db/memos/%s.mmo",nickname);
	if( (fp = fopen(mffile,"r+"))==NULL) {
		dsend(sockfd,"NOTICE %s :No memo at record %d\n",nickname,num);
		return;
	}
	if(countmemos(nickname) < (int)num) got = 0;
	if(got) {
		while(fgets(memoline,MAXLINE,fp)!=NULL)
			if(i++ == num) break;
	}
	if(strlen(memoline) <= 5) got = 0;
	if(got) {
		i = j = 0;
		while( (c=memoline[i++]) != '&')
			from[j++] = c;
		from[j] = '\0';
		j = 0;
		while( (c=memoline[i++]) != '&')
			date[j++] = c;
		date[j] = '\0';
		while( (c=memoline[i++]) != '&')
			read = c;
		j = 0;
		while( (c=memoline[i++]) != '\n')
			msg[j++] = c;
		msg[j] = '\0';

		ptr = (char*)malloc(8);
		if( (ptr = strstr(memoline,"&0&")) != NULL) {
			ptr++;
			ptr = strtok(ptr,"&");
			*ptr = '1';
			*(++ptr) = '&';
		}
		//addlinetofile(fp,memoline,(int)num);
		//update_file(num,fp,ffile,1);
		addlinetofile(fp,memoline,(int)num);
	}
	if(!supress) {
		if(!got) dsend(sockfd,"NOTICE %s :No memo at record: %d\n",nickname,num);
		else {
			dsend(sockfd,"NOTICE %s :Memo (record %ld) from %s on %s states:\n",nickname,atol(number),from,date);
			dsend(sockfd,"NOTICE %s :%s\n",nickname,msg);
		}
	}
	fclose(fp);

	return;
}
int countmemos(char nickname[SSIZE]) {
	int count, test;
	char c;
	char mffile[MAXLINE];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	fp = fopen(mffile,"r");

	count = test = 0;
	while( (c=getc(fp))!=EOF) {
		test++;
		if(c == '\n') {
			count++;
			if(test == 1) {
				count = 0;
				break;
			}
		}
	}
	fclose(fp);

	return count;
}
void listmemos(int sockfd,char nickname[SSIZE]) {
	int i,j,count;
	long record;
	char c,read;
	char from[SSIZE],date[LINE*2],memoline[MAXLINE];
	char mffile[MAXLINE];
	FILE *fp;

	if(!useraccess(nickname)) {
		dsend(sockfd,"NOTICE %s :You are not registered and thus have no memo database.\n",nickname);
		return;
	}
	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	if( (fp = fopen(mffile,"r"))!=NULL) {
		count = countmemos(nickname);
		
		dsend(sockfd,"NOTICE %s :Memo database for %s contains %ld record%s\n",nickname,nickname,count,(count>1||!count)?"s.":".");
		record = 0L;
		while(fgets(memoline,MAXLINE,fp)!=NULL) {
			i = j = 0;
        		while( (c=memoline[i++]) != '&')
	     			from[j++] = c;
			from[j] = '\0';
			j = 0;
		        while( (c=memoline[i++]) != '&')
		                date[j++] = c;
		        date[j] = '\0';
			while( (c=memoline[i++]) != '&')
				read = c;
			
			dsend(sockfd,"NOTICE %s :(record %ld) from %s on %s%s\n",nickname,++record,from,date,(read=='1')?".":". [NEW]");
		}
		fclose(fp);
	} else if(!record || fp == NULL) dsend(sockfd,"NOTICE %s :You have no memo's.\n",nickname);
	
	return;
}
void deletememo(int sockfd,char nickname[SSIZE],char record[6],int supress) {
	long rec;
	char mffile[MAXLINE],filename[LINE*2];
	FILE *fp;

	rec=atol(record);
	snprintf(filename,sizeof(filename),"db/memos/%s.mmo",nickname);
	snprintf(mffile,sizeof(mffile),"%s/%s",INSTALLDIR,filename);
	if( (fp = fopen(mffile,"r+"))!=NULL) {
		if( (countmemos(nickname) < rec) || (rec <= 0) ) {
			if(!supress) dsend(sockfd,"NOTICE %s :No memo at record %ld.\n",nickname,rec);
			return;
		}
		update_file(rec,fp,filename,0);
		if(!supress) dsend(sockfd,"NOTICE %s :Deleted memo at record %ld.\n",nickname,rec);
	} else
		if(!supress) dsend(sockfd,"NOTICE %s :No memo at record %ld.\n",nickname,rec);

	return;
}
void checknew(int sockfd,char nickname[SSIZE]) {
	int i,j,count,records[3000];
	char c,read;
	char memoline[MAXLINE],cmd[MAXLINE],str[LINE];
	char mffile[MAXLINE];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	
	if( (fp = fopen(mffile,"r")) != NULL) {
		j = count = 0;

		while(fgets(memoline,MAXLINE,fp) != NULL) {
			i = 0;
			while( (c=memoline[i++]) != '&');
			while( (c=memoline[i++]) != '&');
			while( (c=memoline[i++]) != '&') {
				read = c;
				switch(read) {
					case '0': records[count++] = ++j;
						  break;
				}
				break;
			}
		}

	} else return;

	if(count > 0L) {
		switch(count) {
			case 1:
				snprintf(cmd,sizeof(cmd),"NOTICE %s :You have a new memo! (",nickname);
				break;
			default:
				snprintf(cmd,sizeof(cmd),"NOTICE %s :You have %ld new memo's! (",nickname,count);
				break;
		}
		for(i=0L;i<=count-1;i++) {
			if(i < count-1)
				snprintf(str,sizeof(str),"%d,",records[i]);
			else
				snprintf(str,sizeof(str),"%d",records[i]);
			strncat(cmd,str,sizeof(cmd));
		}
		strncat(cmd,")",sizeof(cmd));
		dsend(sockfd,"%s\n",cmd);
	} else
	fclose(fp);

	return;
}
int getrecordofdoublememo(char nickname[SSIZE]) {
	int i = 1;
	int j,k,cnt;
	char c,read;
	char dates[LINE*2][2000];
	char memoline[MAXLINE],msg[1024],date[LINE*2],from[SSIZE];
	char mffile[MAXLINE];
	FILE *fp;

	snprintf(mffile,sizeof(mffile),"%s/db/memos/%s.mmo",INSTALLDIR,nickname);
	
	if( (fp = fopen(mffile,"r")) != NULL) {
		k = 0;
		while(fgets(memoline,sizeof(memoline),fp) != NULL) {
			i = j = 0;
			while( (c=memoline[i++]) != '&')
				from[j++] = c;
			from[j] = '\0';
			j = 0;
			while( (c=memoline[i++]) != '&')
				date[j++] = c;
			date[j] = '\0';
			while( (c=memoline[i++]) != '&')
				read = c;
			j = 0;
			while( (c=memoline[i++]) != '\n')
				msg[j++] = c;
			msg[j] = '\0';
			strncpy(dates[k],date,sizeof(dates[k]));
			k++;

			cnt = 0;
			for(j = 1;j <= k;j++) {
				if(!strcmp(dates[j-1],date))
					cnt++;
				if(cnt > 1) return j;
			}
		}
	}

	return 0;
}
