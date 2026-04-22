/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#include "..\include\globals.h"
#include "..\include\tcp.h"
#include "..\include\base.h"
#include "..\include\ras.h"
#include "..\include\nicknames.h"

extern int SocketOpen(int Port,char *Address,int iMyMessage)
{	
	int res;
	SOCKET The_Socket = NULL;
//	WSADATA WSock_dat;
	IN_ADDR	iaHost;
	LPHOSTENT lpHostEntry; 	
	SOCKADDR_IN Destination;
	//WORD WSock_ver=MAKEWORD(2,0);
	
	Destination.sin_family=AF_INET;
	Destination.sin_port = htons(Port);
//	res=WSAStartup(WSock_ver,&WSock_dat);
		
/*	if(res!=0) { 
		PostMessage(g_hwnd,WM_QUIT,NULL,NULL); //Quit if we dont have the correct version of WSock (2.0)
		return -1;
	}
*/
	iaHost.s_addr = inet_addr(Address); 
	
	if (iaHost.s_addr == INADDR_NONE)
		lpHostEntry = gethostbyname(Address);  
	else
		lpHostEntry = gethostbyaddr((const char *)&iaHost, sizeof(struct in_addr), AF_INET);

	if (lpHostEntry == NULL) { 
			return -1; 
	}

	The_Socket=socket(AF_INET,SOCK_STREAM,0);

	if(The_Socket == INVALID_SOCKET) { 
			return -1; 
	}
	
	Destination.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);

	res = connect(The_Socket,(LPSOCKADDR)&Destination,sizeof(SOCKADDR_IN));

	if(res!=0) { 
			return -1; 
	}
	if (iMyMessage != NULL)
	res = WSAAsyncSelect(The_Socket,g_hwnd,iMyMessage,FD_READ|FD_CLOSE);

	if(res!=0) { 
			return -1; 
	}

	return The_Socket;

}

int EasySSend(SOCKET The_Socket,const char*buf)
{
	int count = lstrlen(buf); //Length to send
	return SocketSend(The_Socket,buf,count);
}

extern int SocketSend(UINT The_Socket,const char* buf,int count)
{
	// Send packet
	int ret;
	TIMEVAL tm;
	fd_set wfds;
	FD_ZERO(&wfds);
	FD_SET(The_Socket,&wfds);
	tm.tv_sec=5;
	tm.tv_usec=0;
	if((ret=select(0,NULL,&wfds,NULL,&tm))>0) {
		do {
			ret=send(The_Socket,buf,count,0);
			if(ret==SOCKET_ERROR) break;

			count-=ret;
			buf+=ret;
			if(count>0) Sleep(20);
		} while(count>0);
		if(ret==SOCKET_ERROR) return -1;
		return 1;
	}
	return ret;
}

int SocketRecv (SOCKET The_Socket, char **pInData)
{
	// Check socket for readability (Thanks BO2k)
	TIMEVAL tv;
	int nRet;
	tv.tv_sec=0;
	tv.tv_usec=0;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(The_Socket,&rfds);
	nRet=select(1,&rfds,NULL,NULL,&tv);
	if(nRet==SOCKET_ERROR) return -1;
	if(nRet==0) return 0;

	DWORD len;
	if(ioctlsocket(The_Socket,FIONREAD,&len)==SOCKET_ERROR) 
		return -1;

	char *buf=(char *)malloc(len+1);
	if(buf==NULL) 
	{
		*pInData=NULL;
		return -1;
	}

	// Receive data
	char *pbuf=(char *)buf;
	int count=len,lenret=0;
	do {
		lenret=recv(The_Socket,pbuf,count,0);
		if(lenret==SOCKET_ERROR) 
		{
			free(buf);
			*pInData=NULL;
			return -1;
		}
		count-=lenret;
		pbuf+=lenret;
		if(count>0) Sleep(20);
	} while(count>0);

	// Pass data back to application
	*pInData=buf;

	return len;
}

SOCKET SocketListen(int nPort,unsigned int iMyMessage)
{
	int res;
	SOCKET The_Socket = NULL;
	WSADATA WSock_dat;
	WORD WSock_ver=MAKEWORD(1,1);
	
	res=WSAStartup(WSock_ver,&WSock_dat);
		
	if(res!=0) { 
				return -1;       
		}

	// Create listener socket
	The_Socket=socket(AF_INET,SOCK_STREAM,0);
	if(The_Socket==SOCKET_ERROR) 
		return NULL;
	
	// Bind socket and listen
	SOCKADDR_IN saddr;
	memset(&saddr,0,sizeof(SOCKADDR_IN));
	saddr.sin_addr.S_un .S_addr =INADDR_ANY;// =bindAddr;
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons((WORD)nPort);
	
	if(bind(The_Socket,(SOCKADDR *) &saddr,sizeof(SOCKADDR_IN))==SOCKET_ERROR) {
		closesocket(The_Socket);
		return NULL;
	}

	if(res=listen(The_Socket,SOMAXCONN)!=0) {
		closesocket(The_Socket);
		return NULL;
	}
	
	res = WSAAsyncSelect(The_Socket,g_hwnd,iMyMessage,FD_READ|FD_CLOSE|FD_ACCEPT);

	if(res!=0) {
		return -1; 
	}
	
	return The_Socket;
}

SOCKET SocketAccept(SOCKET The_Socket,unsigned int iMsgId)
{
	int res=0;
	// Check for connection
	fd_set rdfds;
	TIMEVAL tm;

	FD_ZERO(&rdfds);
	FD_SET(The_Socket,&rdfds);
	
	tm.tv_sec=0;
	tm.tv_usec=0;

	if(select(0,&rdfds,NULL,NULL,&tm)<=0) {
		return 0;
	}

	// Accept socket
	SOCKADDR_IN saddr;
	int len=sizeof(SOCKADDR_IN);
	SOCKET accs;

	accs=accept(The_Socket,(SOCKADDR *)&saddr,&len);
	if(accs==INVALID_SOCKET) return NULL;

	if (iMsgId != NULL)
		res = WSAAsyncSelect(accs,g_hwnd,iMsgId,FD_ACCEPT|FD_READ|FD_CLOSE);
	else
		WSAAsyncSelect(accs,g_hwnd, 0, 0); //We dont want anymore socket msgs for it

	if(res!=0) {
		return -1; 
	}
	
	BOOL sopt;
	sopt=TRUE;
	setsockopt(accs,IPPROTO_TCP,TCP_NODELAY,(char *)&sopt,sizeof(BOOL));
	sopt=TRUE;
	setsockopt(accs,SOL_SOCKET,SO_DONTLINGER,(char *)&sopt,sizeof(BOOL));

	return accs;
}
/*
char *EasySRecv(unsigned int The_Socket) //Just make it easier to get a string
{
	char *pdata = new char[MAX_BUFFER_LEN]; 
	int ret= SocketRecv(The_Socket,pdata);
	if (ret > 0) {
		pdata[ret] = '\0';
OutputDebugString(pdata);
	return pdata;
	} else {
		
		return "Error";
	}
}

typedef struct _RASCONNSTATUS { 
    DWORD         dwSize; 
    RASCONNSTATE  rasconnstate; 
    DWORD         dwError; 
    TCHAR         szDeviceType[RAS_MaxDeviceType + 1]; 
    TCHAR         szDeviceName[RAS_MaxDeviceName + 1]; 
} RASCONNSTATUS; 
This function gets the connection info for us, and tells us wether we are connected to the net
*/
BOOL IsConnected()
{	
	LPRASCONN lpRasConn = NULL;
	DWORD   cbBuf = 0;
	DWORD   cConn = 0;
    DWORD   dwRet = 0;
    HRASCONN   hrasconn;
    RASCONNSTATUS rasStatus;
    UINT    ndx;

    // enumerate connections
    cbBuf = sizeof(RASCONN);
    if ( (lpRasConn = ( LPRASCONN ) malloc((UINT)cbBuf)) != NULL)
    {
		lpRasConn->dwSize = sizeof( RASCONN );
        dwRet = RasEnumConnections( lpRasConn, &cbBuf, &cConn );
		for (ndx = 0; ndx < cConn; ndx++)
		{
			// get to HRASCONN
			hrasconn = lpRasConn[ndx].hrasconn;

            rasStatus.dwSize = sizeof(RASCONNSTATUS);
			dwRet = RasGetConnectStatus( hrasconn, &rasStatus );
			if ( dwRet ) 
				return FALSE;
			else
			{
				if (rasStatus.rasconnstate == RASCS_Connected)
				{
					return TRUE;
				}
				else 
					return FALSE;
			}
		}
		if (cConn == 0) { //No RAS, must be a lan? /me shrugs - try and connect anyway
			return TRUE;
		}
	}
   return FALSE; 
}