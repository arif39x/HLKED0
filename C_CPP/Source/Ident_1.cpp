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
#include "..\include\iohandler.h"
#include "..\include\tcp.h"
#include "..\include\base.h"
#include "..\include\ident.h"
#include "..\include\nicknames.h"

IdentSock::IdentSock()
{
	this->Type = ID;
	if (g_IOSock->Insert(this))
		this->iSocket = SocketListen(113,this->GetMessageID());
	else
		delete this;
}

IdentSock::~IdentSock()
{
	closesocket(this->GetSocketNum());
}
	
void IdentSock::Accept(SOCKET iCheckSocket)
{
	if (this->GetSocketNum() == iCheckSocket)
		SocketAccept(iCheckSocket,this->GetMessageID());
}

void IdentSock::DataIn(SOCKET iCheckSocket) //No checking of socket :P
{
	char *cIn = NULL;
	char *cDel = GetNick(TRUE);
	int  iRec  = SocketRecv(iCheckSocket,&cIn);
	
	if (iRec > 1) 
	{
		cIn[iRec-2] =0;
		char *cReply = new char[24 + lstrlen(cIn) + lstrlen(cDel)];
		sprintf(cReply,"%s : USERID : UNIX : %s\x0D\x0A\0",cIn,cDel);
		EasySSend(iCheckSocket,cReply);
		delete cReply;
		closesocket(iCheckSocket);
	}
	delete cDel; 
	delete cIn;
}