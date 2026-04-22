// ----------------------------------------------------------------- // 
DWORD WINAPI ChangeRate(char *rate)
{
	HKEY hKey; char szRegRate[256]; 
	wsprintf(szRegRate, "%s%s", TASKS_PREFIX_Z, TASKS_RATE);

	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return FALSE;
	
	RegSetValueExA(hKey, szRegRate, NULL, REG_SZ, (const unsigned char *)rate, lstrlen(rate));
	RegCloseKey(hKey);
	
	return TRUE;
}
// ----------------------------------------------------------------- // 
int WINAPI GetRate()
{
	HKEY hKey; char szRegRate[256]; 
	char szRate[256]	=	{ 0	};
	DWORD dwRateLen = sizeof(szRate);
	
	int refresh_rate = atoi(DEFAULT_RATE);
	wsprintf(szRegRate, "%s%s", TASKS_PREFIX_Z, TASKS_RATE);
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return refresh_rate;
	}
	
	if(RegQueryValueEx(hKey, szRegRate, 0,0, (unsigned char *)szRate, &dwRateLen) == ERROR_SUCCESS)
	{
		refresh_rate = atoi(szRate);
	} 
	else
	{
		ChangeRate(DEFAULT_RATE);
	}

	RegCloseKey(hKey);
	return refresh_rate;
}
// ----------------------------------------------------------------- // 