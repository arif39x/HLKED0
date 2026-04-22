typedef struct TCPFLOODTHREAD
{
	SESSION Session;
	char *RemoteHost;
	unsigned short RemotePort;
	int Delay;
	DWORD Times;
	int MaxSockets;
	char Data[1024];
//	bool SendData;
//	bool Random;
	bool Gotit;
	int Threadnum;

} TCPFLOODTHREAD;

DWORD WINAPI XTcpFloodThread(LPVOID lpParam);
