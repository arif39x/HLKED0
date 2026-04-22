#include "../include/bot.h"

char join_channels[MAXCHANS][SSIZE];

int sockfd;
FILE *usersfp;

struct nick users[MAXUSERS];

char botnick[LINE],server[LINE],port[SSIZE];

void *showhelpall(void *arg) {
	THREADDATA *td;
	char *nick;
	char bnick[LINE];

	td = (THREADDATA*)arg;
	nick = td->n;
	strncpy(bnick,td->botnick,sizeof(bnick));

	displayhelp(1,nick);
	ddsend(sockfd,"NOTICE %s :%s is an all-round channel holding bot\n",nick,bnick);
	ddsend(sockfd,"NOTICE %s :that is, in effect, part of a \"police force\" of\n",nick);
	ddsend(sockfd,"NOTICE %s :bots that control and maintain a channel.\n",nick);
	ddsend(sockfd,"NOTICE %s : \n",nick);
	ddsend(sockfd,"NOTICE %s :For more information on a command /msg %s help <command>\n",nick,bnick);
	ddsend(sockfd,"NOTICE %s : \n",nick);
	ddsend(sockfd,"NOTICE %s :Core Commands:\n",nick);
	ddsend(sockfd,"NOTICE %s :  ACCESS/IDENTIFY  - Authorize yourself using a password\n",nick);
	ddsend(sockfd,"NOTICE %s :  SET       - Change settings, including the PASSWD option\n",nick);
	ddsend(sockfd,"NOTICE %s :  MODE      - Change a channel's modes\n",nick);
	ddsend(sockfd,"NOTICE %s :  KICK      - Kick a user off a channel\n",nick);
	ddsend(sockfd,"NOTICE %s :  INVITE    - Invite a user to a channel\n",nick);
	ddsend(sockfd,"NOTICE %s :  JOIN      - Force the bot to join a channel\n",nick);
	ddsend(sockfd,"NOTICE %s :  PART      - Force the bot to part a channel\n",nick);
	ddsend(sockfd,"NOTICE %s :  MEMO      - Send/Read/List/Delete Memo's to/from other users\n",nick);
	ddsend(sockfd,"NOTICE %s : \n",nick);
	ddsend(sockfd,"NOTICE %s :Access Commands:\n",nick);
	ddsend(sockfd,"NOTICE %s :  ADDUSER/DELUSER  - Add/remove operators to a channel\n",nick);
	ddsend(sockfd,"NOTICE %s : \n",nick);
	ddsend(sockfd,"NOTICE %s :File Share Commands:\n",nick);
	ddsend(sockfd,"NOTICE %s :  REGISTER - Register a new user to use the file share system\n",nick);
	ddsend(sockfd,"NOTICE %s :  LIST     - Search for files based on pattern search\n",nick);
	ddsend(sockfd,"NOTICE %s :  GETLIST  - Download list of entire file share database\n",nick);
	ddsend(sockfd,"NOTICE %s :  GET      - Download a file from the file share database\n",nick);
	ddsend(sockfd,"NOTICE %s : \n",nick);
	ddsend(sockfd,"NOTICE %s :Other Commands:\n",nick);
	ddsend(sockfd,"NOTICE %s :  RAW     - Execute a raw server command\n",nick);
	ddsend(sockfd,"NOTICE %s :  STATUS  - Retrieve current user status\n",nick);
	ddsend(sockfd,"NOTICE %s :  BOTINFO - Retrieve various botnet information\n",nick);
	ddsend(sockfd,"NOTICE %s :  SEEN    - Last seen time of any user (channel)\n",nick);
	ddsend(sockfd,"NOTICE %s :  INFO    - Information on any existing info item (channel)\n",nick);
	ddsend(sockfd,"NOTICE %s :  ADDINFO/DELINFO  - Add/remove info items\n",nick);
	displayhelp(2,nick);

	return;
}
void displayhelp(int what,char *nick) {
	switch(what) {
		case 1:
			slowdsend(sockfd,"NOTICE %s :***** BotNET Help *****\n",nick);
			break;
		case 2:
			slowdsend(sockfd,"NOTICE %s :***** End of HELP *****\n",nick);
	}

	return;
}
void showhelp(char *nick,int cwords,char cargs[50][MAXLINE]) {
	displayhelp(1,nick);
	if(!strcmp(cargs[4],"identify")) {
		slowdsend(sockfd,"NOTICE %s :Command - IDENTIFY\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - IDENTIFY <password>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command identifies you to the bot as a regular user.\n",nick);
		slowdsend(sockfd,"NOTICE %s :Regular users may only use the File Share Commands.\n",nick);
	} else if(!strcmp(cargs[4],"access")) {
		slowdsend(sockfd,"NOTICE %s :Command - ACCESS\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - ACCESS <password>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command identifies you to the bot as an administrator.\n",nick);
		slowdsend(sockfd,"NOTICE %s :There exists only one administrator password that all\n",nick);
		slowdsend(sockfd,"NOTICE %s :administrators share.\n",nick);
	} else if(!strcmp(cargs[4],"set")) {
		if(cwords == 5) {
			slowdsend(sockfd,"NOTICE %s :Command - SET <command> [<value>]\n",nick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :Lets you change your user settings.\n",nick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :For more information on a command /msg %s help SET <command>\n",nick,botnick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :Available SET Commands:\n",nick);
			slowdsend(sockfd,"NOTICE %s :  PASSWD  - Lets you change your users password\n",nick);
		} else if(cwords > 5)
			if(!strcmp(cargs[5],"passwd")) {
				slowdsend(sockfd,"NOTICE %s :Command - SET PASSWD\n",nick);
				slowdsend(sockfd,"NOTICE %s :Usage   - SET PASSWD <password>\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This changes the password associated with your user.\n",nick);
			}
	} else if(!strcmp(cargs[4],"mode")) {
		slowdsend(sockfd,"NOTICE %s :Command - MODE\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - MODE <channel> <modes>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command allows you to change channel modes.\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :The following examples show what can be done:\n",nick);
		slowdsend(sockfd,"NOTICE %s :  MODE #channel +b user@host - ban an address\n",nick);
		slowdsend(sockfd,"NOTICE %s :  MODE #channel +o nickname  - give chanops\n",nick);
		slowdsend(sockfd,"NOTICE %s :  MODE #channel +nt-spimlk   - change channel modes\n",nick);
	} else if(!strcmp(cargs[4],"kick")) {
		slowdsend(sockfd,"NOTICE %s :Command - KICK\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - KICK <channel> <nick> [<reason>]\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command lets you kick a user off a channel.\n",nick);
	} else if(!strcmp(cargs[4],"invite")) {
		slowdsend(sockfd,"NOTICE %s :Command - INVITE\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - INVITE <nick> <channel>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command lets you invite a user to a channel.\n",nick);
	} else if(!strcmp(cargs[4],"join")) {
		slowdsend(sockfd,"NOTICE %s :Command - JOIN\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - JOIN <channel> [<password>]\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command causes the bot to join a channel.\n",nick);
	} else if(!strcmp(cargs[4],"part")) {
		slowdsend(sockfd,"NOTICE %s :Command - PART\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - PART <channel>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command causes the bot to part a channel.\n",nick);
	} else if(!strcmp(cargs[4],"memo")) {
		if(cwords == 5) {
			slowdsend(sockfd,"NOTICE %s :Command - MEMO <command> <arguments>\n",nick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :Lets you manage your own memo database as well as\n",nick);
			slowdsend(sockfd,"NOTICE %s :send memo's to other users.\n",nick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :For more information on a command /msg %s help MEMO <command>\n",nick,botnick);
			slowdsend(sockfd,"NOTICE %s : \n",nick);
			slowdsend(sockfd,"NOTICE %s :Available MEMO Commands:\n",nick);
			slowdsend(sockfd,"NOTICE %s :  SEND     - Lets you send a memo to another user\n",nick);
			slowdsend(sockfd,"NOTICE %s :  READ     - Lets you read a memo from another user\n",nick);
			slowdsend(sockfd,"NOTICE %s :  LIST     - Displays all memo's in your memo database\n",nick);
			slowdsend(sockfd,"NOTICE %s :  DELETE   - Lets you delete a memo from your memo database\n",nick);
			slowdsend(sockfd,"NOTICE %s :  CLEARALL - Erases your entire memo database\n",nick);
		} else if(cwords > 5) {
			if(!strcmp(cargs[5],"send")) {
				slowdsend(sockfd,"NOTICE %s :Command - MEMO SEND\n",nick);
				slowdsend(sockfd,"NOTICE %s :Usage   - MEMO SEND <user> <message>\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This sends a memo to a desired user.\n",nick);
			} else if(!strcmp(cargs[5],"read")) {
				slowdsend(sockfd,"NOTICE %s :Command - MEMO READ\n",nick);
				slowdsend(sockfd,"NOTICE %s :Usage   - MEMO READ <record>\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This reads a memo at a certain record.\n",nick);
				slowdsend(sockfd,"NOTICE %s :A record is a number that corresponds to a memo\n",nick);
				slowdsend(sockfd,"NOTICE %s :item and is displayed to the left of your memo's\n",nick);
				slowdsend(sockfd,"NOTICE %s :in a MEMO LIST query.\n",nick);
				slowdsend(sockfd,"NOTICE %s :See also: /msg %s help MEMO LIST\n",nick,botnick);
			} else if(!strcmp(cargs[5],"list")) {
				slowdsend(sockfd,"NOTICE %s :Command - MEMO LIST\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This lists all memo's that are in your memo\n",nick);
				slowdsend(sockfd,"NOTICE %s :database, indicating new memo's with the [NEW] suffix.\n",nick);
				slowdsend(sockfd,"NOTICE %s :Each memo listed has a prepended record number\n",nick);
				slowdsend(sockfd,"NOTICE %s :that identifies the memo when it is read.\n",nick);
			} else if(!strcmp(cargs[5],"delete")) {
				slowdsend(sockfd,"NOTICE %s :Command - MEMO DELETE\n",nick);
				slowdsend(sockfd,"NOTICE %s :Usage   - MEMO DELETE <record>\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This deletes a memo at a certain record.\n",nick);
				slowdsend(sockfd,"NOTICE %s :A record is a number that corresponds to a memo\n",nick);
				slowdsend(sockfd,"NOTICE %s :item and is displayed to the left of your memo's\n",nick);
				slowdsend(sockfd,"NOTICE %s :in a MEMO LIST query.\n",nick);
				slowdsend(sockfd,"NOTICE %s :See also: /msg %s help MEMO LIST\n",nick,botnick);
			} else if(!strcmp(cargs[5],"clearall")) {
				slowdsend(sockfd,"NOTICE %s :Command - MEMO CLEARALL\n",nick);
				slowdsend(sockfd,"NOTICE %s : \n",nick);
				slowdsend(sockfd,"NOTICE %s :This erases your entire memo database.\n",nick);
			} else slowdsend(sockfd,"NOTICE %s :Unknown help topic MEMO %s\n",nick,strtoupper(cargs[5]));
		}
	} else if(!strcmp(cargs[4],"adduser")) {
		slowdsend(sockfd,"NOTICE %s :Command - ADDUSER\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - ADDUSER <nick> <new_password>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command adds a new operator user to the bot.\n",nick);
		slowdsend(sockfd,"NOTICE %s :In essence, only operator users can actually use the bot.\n",nick);
		slowdsend(sockfd,"NOTICE %s :The administrator password is required to add users in\n",nick);
		slowdsend(sockfd,"NOTICE %s :order to prevent abuse.\n",nick);
	} else if(!strcmp(cargs[4],"deluser")) {
		slowdsend(sockfd,"NOTICE %s :Command - DELUSER\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - DELUSER <nick>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command deletes an operator user from the bot.\n",nick);
		slowdsend(sockfd,"NOTICE %s :The administrator password is required to delete users in\n",nick);
		slowdsend(sockfd,"NOTICE %s :order to prevent abuse.\n",nick);
	} else if(!strcmp(cargs[4],"raw")) {
		slowdsend(sockfd,"NOTICE %s :Command - RAW\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - RAW <command>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command grants you complete control over the bot.\n",nick);
		slowdsend(sockfd,"NOTICE %s :By causing the bot to execute a raw command you can make the\n",nick);
		slowdsend(sockfd,"NOTICE %s :bot perform actions that are unavailable in its command library.\n",nick);
		slowdsend(sockfd,"NOTICE %s :A command such as: 'PRIVMSG #channel :hello all'\n",nick);
		slowdsend(sockfd,"NOTICE %s :would make the bot greet the channel.\n",nick);
	} else if(!strcmp(cargs[4],"status")) {
		slowdsend(sockfd,"NOTICE %s :Command - STATUS\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - STATUS <user>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command returns active, real-time status about\n",nick);
		slowdsend(sockfd,"NOTICE %s :a requested user.\n",nick);
	} else if(!strcmp(cargs[4],"botinfo")) {
		slowdsend(sockfd,"NOTICE %s :Command - ^BBOTINFO\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command returns active, real-time information about\n",nick);
		slowdsend(sockfd,"NOTICE %s :the botnet on which this bot is connected to.\n",nick);
	} else if(!strcmp(cargs[4],"seen")) {
		slowdsend(sockfd,"NOTICE %s :Command - SEEN\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - SEEN <nick>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This is a channel command and therefore can only\n",nick);
		slowdsend(sockfd,"NOTICE %s :be invoked from a channel.\n",nick);
		slowdsend(sockfd,"NOTICE %s :It returns the last time the bot saw a certain user.\n",nick);
	} else if(!strcmp(cargs[4],"info")) {
		slowdsend(sockfd,"NOTICE %s :Command - INFO\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - INFO <item>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This is a channel command and therefore can only\n",nick);
		slowdsend(sockfd,"NOTICE %s :be invoked from a channel.\n",nick);
		slowdsend(sockfd,"NOTICE %s :It returns comprehensive information on a specific item\n",nick);
		slowdsend(sockfd,"NOTICE %s :as long as that item exists in the info items database.\n",nick);
		slowdsend(sockfd,"NOTICE %s :See also: /msg %s help ADDINFO\n",nick,botnick);
	} else if(!strcmp(cargs[4],"addinfo")) {
		slowdsend(sockfd,"NOTICE %s :Command - ADDINFO\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - ADDINFO <item name> <info>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command adds an info item under the item name specified\n",nick);
		slowdsend(sockfd,"NOTICE %s :with the information specified by the <info> parameter.\n",nick);
		slowdsend(sockfd,"NOTICE %s :See also: /msg %s help DELINFO\n",nick,botnick);
	} else if(!strcmp(cargs[4],"delinfo")) {
		slowdsend(sockfd,"NOTICE %s :Command - DELINFO\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - DELINFO <item name> <info>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command deletes an info item under the item name specified.\n",nick);
		slowdsend(sockfd,"NOTICE %s :See also: /msg %s help ADDINFO\n",nick,botnick);
	} else if(!strcmp(cargs[4],"register")) {
		slowdsend(sockfd,"NOTICE %s :Command - REGISTER\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - REGISTER <password>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :Use this command to register yourself as a new user to the\n",nick);
		slowdsend(sockfd,"NOTICE %s :file share database in order to gain access to the other file.\n",nick);
		slowdsend(sockfd,"NOTICE %s :share commands.\n",nick);
	} else if(!strcmp(cargs[4],"list")) {
		slowdsend(sockfd,"NOTICE %s :Command - LIST\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - LIST <pattern>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command lets you list up to 10 files on the file share database\n",nick);
		slowdsend(sockfd,"NOTICE %s :that have the string in <pattern> anywhere in its filename.\n",nick);
		slowdsend(sockfd,"NOTICE %s :This function is case insensitive and does not require asterixes\n",nick);
		slowdsend(sockfd,"NOTICE %s :in the search patterns.\n",nick);
	} else if(!strcmp(cargs[4],"getlist")) {
		slowdsend(sockfd,"NOTICE %s :Command - GETLIST\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command causes the bot to send you a list of the entire file share.\n",nick);
	} else if(!strcmp(cargs[4],"GET")) {
		slowdsend(sockfd,"NOTICE %s :Command - GET\n",nick);
		slowdsend(sockfd,"NOTICE %s :Usage   - GET <filename>\n",nick);
		slowdsend(sockfd,"NOTICE %s : \n",nick);
		slowdsend(sockfd,"NOTICE %s :This command causes the bot to send you a file that you request.\n",nick);
		slowdsend(sockfd,"NOTICE %s :The file must exist in the file share database and must be identical\n",nick);
		slowdsend(sockfd,"NOTICE %s :to the name of the file in a LIST or GETLIST command.\n",nick);
		slowdsend(sockfd,"NOTICE %s :File names are case sensitive to support the UNIX filesystem structure.\n",nick);
	} else {
		strtoupper(cargs[4]);
		slowdsend(sockfd,"NOTICE %s :Unknown help topic %s, type /msg %s help\n",nick,cargs[4],botnick);
	}
				
	displayhelp(2,nick);

	return;
}
