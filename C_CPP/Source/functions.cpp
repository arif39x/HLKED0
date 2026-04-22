// ------------------
// StartThread
// запустить поток
// ------------------
HANDLE WINAPI StartThread(LPTHREAD_START_ROUTINE lpStartAddress, LPVOID param)
{
	return CreateThread(NULL, NULL, lpStartAddress, param, NULL, 0);
}

// ------------------
//  GetIpByHostname
//  возвращает  IP по имени хоста
// ------------------
DWORD WINAPI GetIpByHostname(char *host)
{
    DWORD ret = 0;
    struct hostent * hp = gethostbyname(host);
	
    if (!hp)
		ret = inet_addr(host);
	
	if ((!hp)&&(ret == INADDR_NONE)) 
		return 0;
	
    if (hp != NULL)
		memcpy((void*)&ret, hp->h_addr,hp->h_length);
	
    return ret;
}

// ------------------
//  GetProcessIdByName
//  возвращает  PID процесса по его имени
// ------------------
DWORD WINAPI GetProcessIdByName(char *ProcessName)
{
	PROCESSENTRY32 ProcEntry = { sizeof(ProcEntry) };
	
	HANDLE ProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if(ProcSnap == INVALID_HANDLE_VALUE) return FALSE;
		
	if(!Process32First(ProcSnap, &ProcEntry))
		return FALSE;
		
	do 
	{
		if(lstrcmp(ProcEntry.szExeFile, ProcessName) == 0)
		{		
// MessageBox(0, ProcEntry.szExeFile, "", 0);
			return ProcEntry.th32ProcessID;
		}
		
	} while(Process32Next(ProcSnap, &ProcEntry));

	CloseHandle(ProcSnap);
	return FALSE;
}

// ------------------
// SetPrivelege
// получить привелегии
// ------------------
BOOL WINAPI SetPrivilege(DWORD dwNamePriv, char *SeNamePriv, BOOL EnableTF)
{
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,&hToken))
		return FALSE;
   
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue( NULL, SeNamePriv, &luid ))  
		return FALSE;

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	
	if (EnableTF)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;
		
	UCHAR   privPrev; 
	if(!RtlAdjustPrivilege(dwNamePriv, TRUE, FALSE, &privPrev) == STATUS_SUCCESS)
	{
		AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);
	}

//	BOOL    privPrev;
//	RtlAdjustPrivilege(0x14, EnableTF, FALSE, &privPrev);
//	RtlAdjustPrivilege(hToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL);
	CloseHandle(hToken);
	
 return TRUE;
}

// ------------------
// CheckInternetConnection
// самоудаление
// ------------------
DWORD WINAPI CheckInternetConnection(LPVOID lParam)
{
	DWORD lpdwFlags;
	BOOL Connect = InternetGetConnectedState(&lpdwFlags, 0) != 0;
	return Connect;
}

