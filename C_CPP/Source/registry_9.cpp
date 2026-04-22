#include "includes.h"
#include "externs.h"

bool XAddAutoStart(char *Path)
{
	HKEY hKey = NULL;

//	printf("Add Registry: %s, DefaultRegPath: %s\r\n", filename, DefaultRegPath);

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, XDefaultRegPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
	{
		//printf("HKLM Failed: %d\r\n", GetLastError());
		//RegCloseKey(key); 
		if (!RegCreateKeyEx(HKEY_CURRENT_USER, XDefaultRegPath, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
		{
			//printf("HKCU Failed: %d\r\n", GetLastError());
			//RegCloseKey(key);
			return false;
		}
	}
		//printf("Error Code: %d\r\n", GetLastError());

	if (Path)
		RegSetValueEx(hKey, XDefaultServiceName, 0, REG_SZ, (LPBYTE)Path, strlen(Path));
	else
		RegDeleteValue(hKey, XDefaultServiceName);

	RegCloseKey(hKey); 

	return true;
}
