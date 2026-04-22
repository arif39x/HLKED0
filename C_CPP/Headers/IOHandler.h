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
#ifndef __INC_IO_HANDLER_H_
#define __INC_IO_HANDLER_H_

#define MAX_IO_HANDLERS 32
#define INVALID_IO_HANDLER ((TcpSock *) NULL)
#include <windows.h>
#include "..\include\base.h"

enum FTYPE { CTCP = 1,JP = 2,RTEXT = 3,SKILL = 4}; //Join/Part CTCP RepetedText SevicesKill
enum S_TYPE { DC = -1 ,MB = 0,FB = 1, PR = 2, ID=4,FT=5}; //MainBot FloodBot PortRedirect WebDownload IDent FileTransfer

struct MBStatus //Status used in MainBots and Floodbots
{
public:
	MBStatus() { Online=FALSE;	Enabled=FALSE; }
	~MBStatus(){};
	BOOL Online;
	BOOL Enabled;
};

class TcpSock
{
public:
	
	TcpSock() {	this->iMessageId = GetNextMessageID(); cOther=" "; cServer=" "; iSList[0]=iSList[1]=0;iSList[2]=-1;iSocket=0;iTypeFB=0;iPort=0;} //Next message id in sequence
	int Connect();
	virtual ~TcpSock() {};

	virtual void DataIn(SOCKET iCheckSocket) {};
	virtual void Accept(SOCKET iCheckSocket) {};
	virtual void Close(SOCKET iCheckSocket) {};
	virtual void Go() {};

	unsigned int GetMessageID() { return this->iMessageId; }
	SOCKET GetSocketNum() { return this->iSocket; }

	BOOL En() { return this->Status.Enabled; }
	BOOL On() { return this->Status.Online; }

	enum S_TYPE GetType() { return this->Type; }

	char *cOther;
	char *cServer;
	int iSList[3];
	int iTypeFB;
	MBStatus Status;

//protected:
	S_TYPE Type;
	unsigned int iMessageId;
	SOCKET iSocket;
	unsigned int iPort;
};

class IrcSocket : public TcpSock
{
public:
	IrcSocket() { cMyNick = NULL;}
	void DataIn(SOCKET iCheckSocket);
	virtual void Process(char *command) {}
	void NewNick(BOOL bRandom = FALSE);
	~IrcSocket();
//protected:
	int iMyNickLen;
	char *cMyNick;
};

class IOHandler
{
public:
	IOHandler();
	~IOHandler();
	TcpSock *GetPFromMessageID (unsigned int iMessage);
	TcpSock *GetPFromSocketNum (SOCKET iSocket);
	TcpSock *GetPFromID (int iId);
	int Count(S_TYPE Type);
	int GetIdFromPointer(TcpSock *pTemp);
	int GetIdFromType (S_TYPE Type,int iSkip,char *cSearch = NULL);
	BOOL Remove(int iId);
	BOOL Insert (TcpSock *p_MyNew);

private:
	 TcpSock *p_Real[MAX_IO_HANDLERS];
};
extern IOHandler *g_IOSock;

#endif //__INC_IO_HANDLER_H_