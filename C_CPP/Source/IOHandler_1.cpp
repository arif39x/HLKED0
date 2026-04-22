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
#include "..\include\IOHandler.h"
#include "..\include\globals.h"

IOHandler::IOHandler()
{
	int i;
	for(i=0;i<MAX_IO_HANDLERS;i++)
		this->p_Real[i] = INVALID_IO_HANDLER;
}

IOHandler::~IOHandler()
{
	for (int i=0;i < MAX_IO_HANDLERS;i++) 
		this->Remove(i);
}

TcpSock *IOHandler::GetPFromSocketNum (SOCKET iSocket)
{
	int i;
	for (i = 0; i < MAX_IO_HANDLERS; i++) {
		if (p_Real[i] != INVALID_IO_HANDLER) {
			if (this->p_Real[i]->GetSocketNum() == iSocket)
				break;
	}}

	if (i < MAX_IO_HANDLERS)
		return this->p_Real[i];
	else
		return INVALID_IO_HANDLER;
}
      
TcpSock *IOHandler::GetPFromMessageID (unsigned int iMessage)
{
	int i;
	for (i = 0; i<MAX_IO_HANDLERS;i++){
		if (p_Real[i] != INVALID_IO_HANDLER)
			if (this->p_Real[i]->GetMessageID() == iMessage) break;
	}


	if (i < MAX_IO_HANDLERS)
		return this->p_Real[i];
	else
		return INVALID_IO_HANDLER;
}

TcpSock *IOHandler::GetPFromID (int iId)
{
	if (iId < 0 || iId > MAX_IO_HANDLERS)
		return NULL;

	return this->p_Real[iId];
}

int IOHandler::GetIdFromPointer(TcpSock *pTemp)
{
	for (int i =0; i < MAX_IO_HANDLERS; i++)
	{
		if (p_Real[i] == pTemp)
			return i;
	}
	return -1;
}
int IOHandler::GetIdFromType (S_TYPE Type,int iSkip,char *cSearch)
{
	int i =0;
	//iSkip +=1;
	do 
	{		
		if (p_Real[i] != INVALID_IO_HANDLER) 
		{		
			if (this->p_Real[i]->GetType() == Type)
			{			
				if (cSearch != NULL)
				{
					if (strstr(this->p_Real[i]->cOther,cSearch) != NULL || 
						strstr(this->p_Real[i]->cServer,cSearch) != NULL)
					iSkip--;
				} else
					iSkip--;

				if (iSkip < 0)	break;
			}
		}
		i++;
		

	} while (i < MAX_IO_HANDLERS);
	
	if (i < MAX_IO_HANDLERS)
		return i;
	else
		return -1;
}
	 
BOOL IOHandler::Insert (TcpSock *p_MyNew)
{
	int i;
	for(i=0;i<MAX_IO_HANDLERS;i++)
	{
		if (this->p_Real[i] == INVALID_IO_HANDLER) 
			break;
	}

	if(i==MAX_IO_HANDLERS) 
		return FALSE;
	
	this->p_Real[i] = p_MyNew;
	
	return TRUE;
}
int IOHandler::Count(S_TYPE Type)
{
	int iRep =0,iRepNoCare =0;
	for (int i=0;i < MAX_IO_HANDLERS;i++) 
	{
		if (p_Real[i] != INVALID_IO_HANDLER) 
		{			
			if (this->p_Real[i]->GetType() == Type)
				iRep++;
			iRepNoCare++;		
		}
	}
	if (Type == -1)
		return iRepNoCare;
	else
		return iRep;	
}

BOOL IOHandler::Remove(int iId)
{
	if (iId > MAX_IO_HANDLERS || iId < 0 || this->p_Real[iId] == INVALID_IO_HANDLER)
		return FALSE;
	
	delete this->p_Real[iId];
	this->p_Real[iId]=INVALID_IO_HANDLER;
	
	return TRUE;
}
