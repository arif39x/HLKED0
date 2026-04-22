// ------------------
// CheckDTaskById
// проверить статус dTask по его ID
// ------------------
DWORD WINAPI CheckDTaskById(char *TaskID)
{
	HKEY hKey; char zTaskID[256];
	wsprintf(zTaskID, "%s%s", TASKS_PREFIX_Z, TaskID);
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return FALSE;
		
	if (RegQueryValueEx(hKey, zTaskID, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return FALSE;
	}
	
	RegCloseKey(hKey);
	return TRUE;
}

// ------------------
// AddDTaskToDb
// добавить dTask в реестр как выполненное
// ------------------
DWORD WINAPI AddDTaskToDb(char *TaskID)
{
	HKEY hKey; char zTaskID[256];
	wsprintf(zTaskID, "%s%s", TASKS_PREFIX_Z, TaskID);
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_WRITE, &hKey) != ERROR_SUCCESS)
		return FALSE;
		
	RegSetValueEx(hKey, zTaskID, 0, REG_SZ, NULL, NULL);
	RegCloseKey(hKey);
	
	return TRUE;
}

// ------------------
//  DeleteDTaskFromDb
// удалить dTask из базы
// ------------------
DWORD WINAPI DeleteDTaskFromDb(char *TaskID)
{
	HKEY hKey; char zTaskID[256];
	wsprintf(zTaskID, "%s%s", TASKS_PREFIX_Z, TaskID);
	
	if(!RegOpenKeyEx(HKEY_LOCAL_MACHINE, TASKS_REG_DATA, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
		return FALSE;
		
	RegDeleteValue(hKey, zTaskID);
	
	return TRUE;
}
