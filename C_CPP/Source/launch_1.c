#include "../include/bot.h" 
char join_channels[MAXCHANS][SSIZE];

char botnick[LINE],server[LINE],port[SSIZE];

int sockfd, logsw = 1;
int numbots = 0;
FILE *usersfp;

struct nick users[MAXUSERS];
struct channel schannels[MAXCHANS];

void launch(void) {
	int i,j,k,p,nready,cwords,tcwords,admincmd,maxfd,inelse,val,gr;
	pid_t pid;
	char *servcmd,*nick,*chan,*ptr,*cmdptr;
	char cmd[2048],scmd[MAXLINE],lffile[MAXLINE],tbuf[MAXLINE];
	char cargs[50][MAXLINE],cargs_bak[50][MAXLINE],nick_bak[LINE];
	char snick[LINE],mffile[MAXLINE];
	fd_set rset,allset;
	FILE *logfp,*tfp;

	prep();
	initseenstructs();
	snprintf(lffile,sizeof(lffile),"%s/logs/channel.log",INSTALLDIR);
        if( (logfp = fopen(lffile,"r+")) == NULL)
        	logsw = 0;
	else fseek(logfp,0L,SEEK_END);

	if(sockfd > dbfd) maxfd = sockfd;
	else maxfd = dbfd;
	val = fcntl(sockfd,F_GETFL,0);
	fcntl(sockfd,F_SETFL,val | O_NONBLOCK);
	FD_ZERO(&allset);
	FD_SET(sockfd,&allset);
	if(dbfd >= 0) FD_SET(dbfd,&allset);
	for(;;) {
		rset = allset;
		if( (nready=select(maxfd+1,&rset,NULL,NULL,NULL))<0) {
			perror("select error");
			exit(-1);
		}
		if(FD_ISSET(sockfd,&rset)) {
			memset(cmd,'\0',sizeof cmd);
			if(drecv(sockfd,cmd,sizeof(cmd)) < 0) if(--nready <= 0) continue;
			trim(cmd,'\n');
			inelse = 0;
			if(strstr(cmd,"376")!=NULL) { // End of /MOTD command.
				cwords = args(cmd,cargs);
				if(!strcmp(cargs[1],"376"))
				{
					server_init();
					if(--nready <=0) continue;
				}
				// else fall through
			}
			if(strstr(cmd,"PING")!=NULL) {
				dsend(sockfd,"PONG %s\n",cmd);
				if(--nready <= 0) continue;
			}
			else if(strstr(cmd,"VERSION")!=NULL) {
				nick = (char*)getnick(cmd);
				strncpy(nick_bak,nick,sizeof(nick_bak));
				strtolower(&nick);
				dsend(sockfd,"NOTICE %s :\001VERSION %s\001\n",nick,VERSION_REPLY);
				if(--nready <= 0) continue;
				inelse = 1;
			} else if(strstr(cmd,"ERROR")!=NULL&&strstr(cmd,"Closing Link")!=NULL) {
				cwords = args(cmd,cargs);
				if(!strcmp(cargs[0],"ERROR"))
					start();
			} else if(strstr(cmd,"352")!=NULL) { // who result
				cwords = args(cmd,cargs);
				if(!strcmp(cargs[1],"352"))
				{
					chan = (char*)cargs[3];
					nick = (char*)cargs[7];
					strtolower(&chan);
					strtolower(&nick);
					if(strcmp(nick,botnick))
					{
						killseen(nick,chan);
						addtostruct(nick,chan);
					}
					if(--nready <= 0) continue;
				}
				// else fall through
			} else if(strstr(cmd,"JOIN")!=NULL) {
				cwords = args(cmd,cargs);
				nick = (char*)getnick(cmd);
				strtolower(&nick);
				if(strcmp(nick,botnick) && !strcmp(cargs[1],"JOIN")) {
					chan = getchannel(cargs);
					trim(chan,':');
					strtolower(&chan);
					killseen(nick,chan);
					addtostruct(nick,chan);
					if(adminaccess(nick))
						dsend(sockfd,"MODE %s +o %s\n",chan,nick);
				} if(!strcmp(cargs[1],"JOIN")) {
					chan = getchannel(cargs);
					if(logsw) {
						if(strcmp(chan,botnick)) {
							fseek(logfp,0L,SEEK_END);
							fputs(parsemsg(cmd),logfp);
							fflush(logfp);
						}
					}
					if(!strcmp(nick,botnick))
						dsend(sockfd,"WHO %s\n",chan);
				}
			} if(strstr(cmd,"MODE")!=NULL) {
				cwords = args(cmd,cargs);
				nick = (char*)getnick(cmd);
				if(!strcmp(cargs[1],"MODE")) {
					if(logsw) {
						fseek(logfp,0L,SEEK_END);
						fputs(parsemsg(cmd),logfp);
						fflush(logfp);
					}
					if(--nready <= 0) continue;
				}
			} if (strstr(cmd,"KICK")!=NULL) {
				cwords = args(cmd,cargs);
				nick = (char*)getnick(cmd);
				if(!strcmp(cargs[1],"KICK")) {
					if(logsw) {
						fseek(logfp,0L,SEEK_END);
						fputs(parsemsg(cmd),logfp);
						fflush(logfp);
					}
					if(--nready <= 0) continue;
				}
			} if((strstr(cmd,"QUIT")!=NULL)||(strstr(cmd,"PART")!=NULL)) {
				cwords = args(cmd,cargs);
				nick = (char*)getnick(cmd);
				strtolower(&nick);
				if(strcmp(nick,botnick) && (!strcmp(cargs[1],"PART") || !strcmp(cargs[1],"QUIT"))) {
					for(j = 0;j <= MAXUSERS-1;j++) {
						if(users[j].flags == OPER || users[j].flags == ADMIN)
							if(!strcmp(users[j].name,nick)) {
								users[j].flags = NORMAL;
								break;
							}
					}
					if(!strcmp(cargs[1],"QUIT")) {
						cleanallseen(nick);
						if(logsw) {
							fseek(logfp,0L,SEEK_END);
							fputs(parsemsg(cmd),logfp);
							fflush(logfp);
						}
					} else if(!strcmp(cargs[1],"PART")) {
						chan = getchannel(cargs);
						trim(chan,':');
						strtolower(&chan);
						setseen(nick,chan);
						if(logsw) {
							fseek(logfp,0L,SEEK_END);
							fputs(parsemsg(cmd),logfp);
							fflush(logfp);
						}
					}
				}
			} if(strstr(cmd,"PRIVMSG")!=NULL && !inelse) {
				cwords = args(cmd,cargs);
				for(j = 0;j < cwords;j++)
					strncpy(cargs_bak[j], cargs[j], sizeof(cargs_bak[j]));
				if(cwords > 4) strncpy(snick,cargs[4],sizeof(snick)-1);
				makelower(cargs,cwords);
				nick = (char*)getnick(cmd);
				if(cwords > 2) {
					chan = getchannel(cargs);
					trim(chan,':');
					strtolower(&chan);
					if(logsw && strcmp(chan,botnick)) {
						fseek(logfp,0L,SEEK_END);
						fputs(parsemsg(cmd),logfp);
						fflush(logfp);
					}
				}
				if(cwords > 2 && chanin(chan)) {
					if(!strcmp(cargs[3],":seen") && cwords > 4)
						seen(sockfd,snick,chan);
					else if(!strcmp(cargs[3],":info") && cwords > 4)
						if(readinfo(cargs[4]) != NULL)
							dsend(sockfd,"PRIVMSG %s :%s\n",chan,readinfo(cargs[4]));
					else if(--nready <= 0) continue;
				} else if(!strcmp(cargs[2],botnick)) {
					i = 6;
					admincmd = 0;
					tcwords = cwords - 5;

					strncpy(nick_bak,nick,sizeof(nick_bak));
					strtolower(&nick);
					if(adminaccess(nick) || useraccess(nick)) {
						if(!strcmp(cargs[3],":get")) {
							if(cwords>4) {
								i = 4;
								memset(cmd,'\0',sizeof(cmd));
								while(i < cwords) {
									strncat(cmd,cargs_bak[i++],sizeof(cmd));
									if(i < cwords) strncat(cmd," ",sizeof(cmd));
								}
								snprintf(mffile,sizeof(mffile),"%s/%s",MP3DIR,cmd);
								if( (tfp = fopen(mffile,"r")) == NULL || strstr(mffile,"..") != NULL)
									dsend(sockfd,"NOTICE %s :File %s does not exist.\n",nick,cmd);
								else {
									fclose(tfp);
									dccsend(nick,mffile);
								}
							}
							if(--nready <= 0) continue;
						} else if(!strcmp(cargs[3],":list")) {
							if(cwords>4)
								sendlist(nick,cargs[4]);
							else
								sendlist(nick,"*");
							if(--nready <= 0) continue;
						} else if(!strcmp(cargs[3],":getlist")) {
							sendlist(nick,"%all%");
							if(--nready <= 0) continue;
						}
					} else {
						if(!userexist(nick)) {
							if(!strcmp(cargs[3],":identify")) {
								dsend(sockfd,"NOTICE %s :You are not a registered user.\n",nick);
								if(--nready <= 0) continue;
							}
						}
					}
					if(!strcmp(cargs[3],":access")) {
						if(cwords>4) {
							ptr = PASSWORD;
							strtolower(&ptr);
							if(!strcmp(cargs[4],ptr)) {
								if(addadmin(nick))
									dsend(sockfd,"NOTICE %s :You now have administrator privileges.\n",nick);
								else
									dsend(sockfd,"NOTICE %s :You have already identified as an administrator.\n",nick);
							} else {
								if(logsw) {
									dsend(sockfd,"NOTICE %s :Your attempt has been logged.\n",nick);
									fseek(logfp,0L,SEEK_END);
									memset(cmd,'\0',sizeof(cmd));
									snprintf(cmd,sizeof(cmd),"+ALERT+ %s has failed an admin access attempt.\n",nick_bak);
									fputs(cmd,logfp);
									fflush(logfp);
								} else
									dsend(sockfd,"NOTICE %s :Failed admin access attempt.\n",nick);
							}
						} else
							dsend(sockfd,"NOTICE %s :Invalid syntax for command: access.\n",nick);
						if(--nready <= 0) continue;
					}
					else if(!strcmp(cargs[3],":identify") && !useraccess(nick) && !adminaccess(nick)) {
						if(cwords>3) {
							if(rightpass(nick,cargs[4],usersfp)) {
								for(j=0;j<=MAXUSERS-1;j++)
									if(users[j].flags==NORMAL) {
										users[j].flags = OPER;
										strncpy(users[j].name,nick,sizeof(users[j].name));
										break;
									}
								dsend(sockfd,"NOTICE %s :Access granted: you now have user access.\n",nick);
								checknew(sockfd,nick);
							} else
								dsend(sockfd,"NOTICE %s :Access denied: password incorrect.\n",nick);
						}
						if(--nready <= 0) continue;
					} else if(!strcmp(cargs[3],":help")) {
						if(cwords == 4)
							runthread(nick,showhelpall);
						else if(cwords > 4)
							showhelp(nick,cwords,cargs);
						if(--nready <= 0) continue;
					} else if(!strcmp(cargs[3],":botinfo")) {
						if(dbfd < 0)
							dsend(sockfd,"NOTICE %s :I am not connected to a botnet.\n",nick);
						else dbsend(dbfd,"NUMBOTS?;%s;%s;\n",nick,botnick);
						if(--nready <= 0) continue;
					} else if(!strcmp(cargs[3],":register")) {
						if(cwords>4) {
							if(!userexist(nick)) {
								adduser(nick,cargs[4]);
								dbsend(dbfd,"ADDUSER;%s;%s;\n",cargs[4],cargs[5]);
								dsend(sockfd,"NOTICE %s :You have registered and now have access to my file database.\n",nick);
								dsend(sockfd,"NOTICE %s :For more information about bot commands /msg %s help\n",nick,botnick);
								for(j=0;j<=MAXUSERS-1;j++)
									if(users[j].flags==NORMAL) {
										users[j].flags = OPER;
										strncpy(users[j].name,nick,sizeof(users[j].name));
										break;
									}
								checknew(sockfd,nick);
							} else
								dsend(sockfd,"NOTICE %s :You are already a registered user.\n",nick);
						} else
							dsend(sockfd,"NOTICE %s :Invalid syntax for command: register.\n",nick);
						if(--nready <= 0) continue;
					} else
					if(adminaccess(nick)) {
						if(!strcmp(cargs[3],":mode")) {
							servcmd = "MODE";
							admincmd = 1;
						} else if(!strcmp(cargs[3],":kick")) {
							servcmd = "KICK";
							admincmd = 1;
						} else if(!strcmp(cargs[3],":invite")) {
							servcmd = "INVITE";
							admincmd = 1;
						} else if(!strcmp(cargs[3],":join")) {
							servcmd = "JOIN";
							admincmd = 1;
							if(cwords > 4) addchantoarr(cargs[4]);
						} else if(!strcmp(cargs[3],":part")) {
							servcmd = "PART";
							admincmd = 1;
							if(cwords > 4) delchanfromarr(cargs[4]);
						}
						if(admincmd) {
							strncpy(scmd,cmd,sizeof(scmd));
							snprintf(cmd,sizeof(cmd),"%s %s %s%s",servcmd,cargs[4],cargs[5],(servcmd!="KICK"&&servcmd!="PART")?" ":" :");
							while(--tcwords>0) {
								strncat(cmd,cargs[i++],sizeof(cmd));
								if(tcwords>1) strncat(cmd," ",sizeof(cmd));
							}
							ptr = killstring(cmd,"!global");
							dsend(sockfd,"%s\n",ptr);
							if(strstr(scmd,"!global") != NULL)
								dbsend(dbfd,"GLOBAL_COMMAND;%s;\n",ptr);
						} else {
							if(!strcmp(cargs[3],":status")) {
								if(cwords < 5) {
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: status.\n",nick);
									if(--nready <= 0) continue;
								} else
									if(adminaccess(cargs[4]))
										dsend(sockfd,"NOTICE %s :%s is an administrator.\n",nick,cargs_bak[4]);
									else if(useraccess(cargs[4]))
										dsend(sockfd,"NOTICE %s :%s is currently identified.\n",nick,cargs_bak[4]);
									else if(!userexist(cargs[4]))
										dsend(sockfd,"NOTICE %s :%s is not a registered user.\n",nick,cargs_bak[4]);
									else
										dsend(sockfd,"NOTICE %s :%s has not identified yet.\n",nick,cargs_bak[4]);
							} else if(!strcmp(cargs[3],":raw")) {
								if(cwords>4) {
									gr = 0;
									if(strstr(cmd,"!global") != NULL) gr = 1; // globalraw
									bzero(&cmd,sizeof(cmd));
									for(k=4;k<=cwords-1;k++) {
										if(strstr(cargs_bak[k],"!global") == NULL) {
											send(sockfd,cargs_bak[k],strlen(cargs_bak[k]),0);
											if(gr) {
												strncat(cmd,cargs_bak[k],sizeof(cmd));
											}
										}
										if(k!=cwords-1) {
											send(sockfd," ",1,0);
											if(gr) strncat(cmd," ",sizeof(cmd));
										}
									}
									send(sockfd,"\r\n",2,0);
									if(gr) dbsend(dbfd,"GLOBAL_COMMAND;%s;\n",cmd);
								} else dsend(sockfd,"NOTICE %s :Invalid syntax for command: raw.\n",nick);
							} else if(!strcmp(cargs[3],":memo")) {
								if(!strcmp(cargs[4],"send")) {
									if(cwords<7) {
										dsend(sockfd,"NOTICE %s :Invalid syntax for command: memo send.\n",nick);
										if(--nready <= 0) continue;
									} else {
										memset(cmd,'\0',sizeof(cmd));
										while(--tcwords>0) {
											strncat(cmd,cargs_bak[i++],sizeof(cmd));
											if(tcwords>1) strncat(cmd," ",sizeof(cmd));
										}
										if(userexist(cargs[5])) {
											sendmemo(cargs[5],cmd,nick,0);
											dbsend(dbfd,"MEMO_SEND;%s;%s;%s;\n",nick,cargs[5],cmd);
										}
										else
											dsend(sockfd,"NOTICE %s :User %s is not registered and thus cannot receive memo's.\n",nick,cargs[5]);
										memset(cmd,'\0',sizeof(cmd));
										if(--nready <= 0) continue;
									}
								} else if(!strcmp(cargs[4],"read")) {
									if(cwords<6) {
										dsend(sockfd,"NOTICE %s :Invalid syntax for command: memo read.\n",nick);
										if(--nready <= 0) continue;
									} else {
										readmemo(sockfd,cargs[5],nick,0);
										dbsend(dbfd,"MEMO_READ;%s;%s;\n",nick,cargs[5]);
									}
								} else if(!strcmp(cargs[4],"list")) listmemos(sockfd,nick);
								else if(!strcmp(cargs[4],"delete")) {
									if(cwords<6) {
										dsend(sockfd,"NOTICE %s :Invalid syntax for command: memo delete.\n",nick);
										if(--nready <= 0) continue;
									} else {
										deletememo(sockfd,nick,cargs[5],0);
										dbsend(dbfd,"MEMO_DELETE;%s;%s;\n",nick,cargs[5]);
									}
								} else if(!strcmp(cargs[ 4],"clearall")) {
									memset(cmd,'\0',sizeof(cmd));
									snprintf(cmd,sizeof(cmd),"%s/db/memos/%s.mmo",INSTALLDIR,nick);
									remove(cmd);
									dsend(sockfd,"NOTICE %s :Memo database for %s cleared.\n",nick,nick);
									dbsend(dbfd,"MEMO_CLEAR_ALL;%s;\n",nick);
								} else {
									strtoupper(cargs[4]);
									dsend(sockfd,"NOTICE %s :Invalid parameter (%s) for command MEMO.\n",nick,cargs[4]);
								}
									
								if(--nready <= 0) continue;
							}
							else if(!strcmp(cargs[3],":adduser"))
								if(cwords>5) {
									if(!userexist(cargs[4])) {
										adduser(cargs[4],cargs[5]);
										dbsend(dbfd,"ADDUSER;%s;%s;\n",cargs[4],cargs[5]);
										dsend(sockfd,"NOTICE %s :User %s added to access list.\n",nick,cargs[4]);
									} else
										dsend(sockfd,"NOTICE %s :User %s already exists.\n",nick,cargs[4]);
								} else
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: adduser.\n",nick);
							else if(!strcmp(cargs[3],":deluser"))
								if(cwords>4) {
									if(deluser(cargs[4])<0)
										dsend(sockfd,"NOTICE %s :User %s does not exist.\n",nick,cargs[4]);
									else {
										dbsend(dbfd,"DELUSER;%s;\n",cargs[4]);
										dsend(sockfd,"NOTICE %s :User %s removed from access list.\n",nick,cargs[4]);
									}
								} else
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: deluser.\n",nick);
							else if(!strcmp(cargs[3],":set"))
								if(cwords>5) {
									if(!strcmp(cargs[4],"passwd")) {
										setpasswd(nick,cargs[5]);
										dsend(sockfd,"NOTICE %s :Your new password is %s.\n",nick,cargs[5]);
										dbsend(dbfd,"SET_PASSWD;%s;%s;\n",nick,cargs[5]);
										dsend(sockfd,"NOTICE %s :Make sure you remember it for later use.\n",nick);
									} else {
										strtoupper(cargs[4]);
										dsend(sockfd,"NOTICE %s :Invalid parameter (%s) for command: SET.\n",nick,cargs[4]);
									}
								} else
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: set.\n",nick);
							else if(!strcmp(cargs[3],":identify"))
								dsend(sockfd,"NOTICE %s :You have already identified.\n",nick);
							else if(!strcmp(cargs[3],":addinfo")) {
								if(cwords>4) {
									if(cwords>5) {
										p = 5;
										memset(cmd,'\0',sizeof(cmd));
										snprintf(tbuf,sizeof(tbuf),"%s: ",cargs_bak[4]);
										strncat(cmd,tbuf,sizeof(cmd));
										while(--tcwords+1>0) {
											strncat(cmd,cargs_bak[p++],sizeof(cmd));
											if(tcwords>0) strncat(cmd," ",sizeof(cmd));
										}
										addinfo(cargs[4],cmd);
										dsend(sockfd,"NOTICE %s :Info item %s added to info database.\n",nick,cargs_bak[4]);
									} else {
										dsend(sockfd,"NOTICE %s :Invalid syntax for command: addinfo.\n",nick);
										if(--nready <= 0) continue;
									}
								} else
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: addinfo.\n",nick);
							} else if(!strcmp(cargs[3],":delinfo")) {
								if(cwords>4) {
									delinfo(cargs[4]);
									dsend(sockfd,"NOTICE %s :Info item %s deleted from database.\n",nick,cargs[4]);
								} else
									dsend(sockfd,"NOTICE %s :Invalid syntax for command: delinfo.\n",nick);
							} else {
								trim(cargs[3],':');
								strtoupper(cargs[3]);
								dsend(sockfd,"NOTICE %s :Unrecognized command %s\n",nick,cargs[3]);
								dsend(sockfd,"NOTICE %s :For help: /msg %s HELP\n",nick,botnick);
							}
							if(--nready <= 0) continue;
						}
					} else if(useraccess(nick)) {
						if(!strcmp(cargs[3],":identify")) {
							dsend(sockfd,"NOTICE %s :You have already identified.\n",nick);
							if(--nready <= 0) continue;
						}
						/*else if(cmdexist(cargs[3])) {
							dsend(sockfd,"NOTICE %s :You must identify to use that command.\n",nick);
						 } else {
							trim(cargs[3],':');
							strtoupper(cargs[3]);
							dsend(sockfd,"NOTICE %s :Unrecognized command ^B%s\n",nick,cargs[3]);
							dsend(sockfd,"NOTICE %s :For help: /msg %s HELP\n",nick,botnick);
						}*/
					} if(!strcmp(cargs[3],":status")) {
						if(cwords < 5) {
							dsend(sockfd,"NOTICE %s :Invalid syntax for command: status.\n",nick);
							if(--nready <= 0) continue;
						} else
							if(adminaccess(cargs[4]))
								dsend(sockfd,"NOTICE %s :%s is an administrator.\n",nick,cargs_bak[4]);
							else if(useraccess(cargs[4]))
								dsend(sockfd,"NOTICE %s :%s is currently identified.\n",nick,cargs_bak[4]);
							else if(!userexist(cargs[4]))
								dsend(sockfd,"NOTICE %s :%s is not a registered user.\n",nick,cargs_bak[4]);
							else
								dsend(sockfd,"NOTICE %s :%s has not identified yet.\n",nick,cargs_bak[4]);
					} else {
						if(!cmdexist(cargs[3])) {
							trim(cargs[3],':');
							strtoupper(cargs[3]);
							dsend(sockfd,"NOTICE %s :Unrecognized command %s\n",nick,cargs[3]);
							dsend(sockfd,"NOTICE %s :For help: /msg %s HELP\n",nick,botnick);
						} else {
							if(!adminaccess(nick))
								dsend(sockfd,"NOTICE %s :Access denied - you are not authorized to use that command.\n",nick);
						}
					}
				}	
			}
			
		}
		if(dbfd >= 0) {
			if(FD_ISSET(dbfd,&rset)) {
				memset(cmd,'\0',sizeof(cmd));
				if(dbrecv(dbfd,cmd) <= 0) continue;
				parse(dbfd,cmd);
			}
		}
	}
	
	return;
}
