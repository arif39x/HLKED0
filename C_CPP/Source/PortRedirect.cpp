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
#include "..\include\portredirect.h"
#include "..\include\base.h"
#include "..\include\tcp.h"
#include "..\include\globals.h"

PortRedirect::PortRedirect(int inPort,int outPort,char *RedirectTo)
{
	this->Type = PR;
	this->iPort = outPort;
	this->Status.Enabled = TRUE;
	this->iOpen = 0;
	this->cServer = strcpy(new char[lstrlen(RedirectTo)],RedirectTo);
	if (g_IOSock->Insert(this))
	{
		this->iSocket = SocketListen(inPort,this->GetMessageID());	
	}
	else
		delete this;

	return;
}

void PortRedirect::Accept(SOCKET iCheckSocket)
{
	if (this->iOpen >= MAX_PORT_REDIRECTS) 
	{
		closesocket(SocketAccept(iCheckSocket,NULL));
		return;
	}

	SOCKET iIn=SocketAccept(iCheckSocket,NULL),iOut=this->Connect();

	if (iIn == INVALID_SOCKET  || iOut == INVALID_SOCKET)
	{
		closesocket(iIn);
		closesocket(iOut);
		return;
	}
	
	this->iOpen++;

	DWORD dwBlock=1;
	ioctlsocket(iIn, FIONBIO, &dwBlock);
	DWORD dwLen;
	fd_set rfds;

	//Now Sit around a foward packets....
	while (this->En())
	{
		Sleep(PORTOPERATIONSLEEPTIME);
		FD_ZERO(&rfds);
		FD_SET(iIn,&rfds);
		FD_SET(iOut,&rfds);
		TIMEVAL tm;
		tm.tv_sec=0;
		tm.tv_usec=0;
	
		if(select(0,&rfds,NULL,&rfds,&tm)>0) //Check sockets for read and errors
		{		
			if(FD_ISSET(iIn,&rfds)) 
			{
				ioctlsocket(iIn,FIONREAD,&dwLen);
				if(dwLen<=0) break;
				char *buffer=(char *)malloc(dwLen);
				if(buffer!=NULL) 
				{
					dwLen=recv(iIn,buffer,dwLen,0);
					if(dwLen>0) send(iOut,buffer,dwLen,0);
					free(buffer);								
				}
			}	

			if(FD_ISSET(iOut,&rfds)) 
			{
				ioctlsocket(iOut,FIONREAD,&dwLen);
				if(dwLen<=0) break;
				char *buffer=(char *)malloc(dwLen);
				if(buffer!=NULL) 
				{
					dwLen=recv(iOut,buffer,dwLen,0);
					if(dwLen>0) send(iIn,buffer,dwLen,0);
					free(buffer);
				}
			}	
		}
	}
	closesocket(iIn);
	closesocket(iOut);
	this->iOpen--;
}

PortRedirect::~PortRedirect()
{
	closesocket(this->iSocket);
	this->Status.Enabled = FALSE;

	int iWait =0;
	while (this->iOpen && iWait < 10)
	{
		Sleep(1000);
	}
}
