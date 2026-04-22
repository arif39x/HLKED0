#include "includes.h"
#include "externs.h"


bool XCreateService(char *Host, char *ServiceName, char *ServiceDisplayName, char *Path, bool Modify)
{
	#ifdef DEBUG_SYSTEM
	dbg("XInstallService()");
	#endif

	SC_HANDLE        schSCManager;
	SC_HANDLE        schService;
	DWORD            dwErrorCode;

	schSCManager = OpenSCManager((Host)?(Host):(NULL), NULL, SC_MANAGER_ALL_ACCESS);
    if (!schSCManager)
	{
		#ifdef DEBUG_SYSTEM
		dbg("OpenSCManager() Failed, Error Code: %d", GetLastError());
		#endif

		return false;
	}
	else
	{
		#ifdef DEBUG_SYSTEM
		dbg("Creating Service...");
		#endif

		schService = CreateService(schSCManager, ServiceName, ServiceDisplayName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, Path, NULL, NULL, NULL, NULL, NULL);
//		dbg("local create sc error code: %d", GetLastError());
		if (!schService)
		{
			dwErrorCode = GetLastError();

			#ifdef DEBUG_SYSTEM
			dbg("CreateService() Failed, Error Code: %d",dwErrorCode);
			#endif

			if (dwErrorCode != ERROR_SERVICE_EXISTS)
			{
				CloseServiceHandle(schSCManager);

       			return false;
			}
			else if (dwErrorCode == ERROR_SERVICE_EXISTS && Modify)
			{
				#ifdef DEBUG_SYSTEM
				dbg("Modifing Service: %s, Already Exists, Error Code: %d", ServiceName, dwErrorCode);
				#endif

				schService = OpenService(schSCManager, ServiceName, SC_MANAGER_ALL_ACCESS);
				if(!schService)
				{
					#ifdef DEBUG_SYSTEM
					dbg("OpenService(), Failed, Error Code: %d", GetLastError());
					#endif

					CloseServiceHandle(schSCManager);

					return false;
				}
				else if (!ChangeServiceConfig(schService, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, Path, NULL, NULL, NULL, NULL, NULL, ServiceDisplayName))
				{
					#ifdef DEBUG_SYSTEM
					dbg("ChangeServiceConfig(), Failed, Error Code: %d", GetLastError());
					#endif

					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);

					return false;
				}
				else
				{
					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);

					#ifdef DEBUG_SYSTEM
					dbg("ChangeServiceConfig() Successed!");
					#endif
					
					return true;
				}
			}
			else if (dwErrorCode == ERROR_SERVICE_EXISTS && !Modify)
			{
				#ifdef DEBUG_SYSTEM
				dbg("Starting Service: %s, Already Exists, Error Code: %d", ServiceName, GetLastError());
				#endif
				
				schService = OpenService(schSCManager, ServiceName, SERVICE_START);

				if(!schService)
				{
					#ifdef DEBUG_SYSTEM
					dbg("OpenService(), Failed, Error Code: %d", GetLastError());
					#endif

					CloseServiceHandle(schSCManager);

					return false;
				}
				else if (!StartService(schService, 0, NULL))
				{
					#ifdef DEBUG_SYSTEM
					dbg("StartService(), Failed, Error Code: %d", GetLastError());
					#endif

					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);
						
					return false;
				}
				else
				{
					CloseServiceHandle(schService);
					CloseServiceHandle(schSCManager);

					#ifdef DEBUG_SYSTEM
					dbg("StartService() Successed!");
					#endif
						
					return true;
				}
			}
		}
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);

			#ifdef DEBUG_SYSTEM
			dbg("CreateService() Successed!");
			#endif

			return true;
		
	//	#else
	//	CloseServiceHandle(schService);
	//	CloseServiceHandle(schSCManager);
//
//		return true;
//		#endif
	}
	return false;
}

bool XDeleteService(char *Host, char *ServiceName) 
{
	#ifdef DEBUG_SYSTEM
	dbg("XStopService()");
	#endif

	SC_HANDLE SCManager, Service;
//	SERVICE_STATUS ServiceStatus = {0};
//	char *buffer = 0;

	#ifdef DEBUG_SYSTEM
	dbg("Opening Service Manager, Host: %s", Host);
	#endif

	SCManager = OpenSCManager((Host)?(Host):(NULL), NULL, SC_MANAGER_ALL_ACCESS);
    if (!SCManager)
	{
		#ifdef DEBUG_SYSTEM
		dbg("OpenSCManager() Failed, Error Code: %d", GetLastError());
		#endif

		return false;
	}

	#ifdef DEBUG_SYSTEM
    dbg("Opening Service, Name: %s", ServiceName);
	#endif

	Service = OpenService(SCManager, ServiceName, SERVICE_ALL_ACCESS);
	if (!Service) 
	{
		CloseServiceHandle(SCManager);

		#ifdef DEBUG_SYSTEM
		dbg("OpenService() Failed, Error Code: %d", GetLastError());
		#endif

		return false;
	}
	else
	{
		#ifdef DEBUG_SYSTEM
     	dbg("Removing Service, Name: %s", ServiceName);
		#endif

		if (!DeleteService(Service))
		{
			#ifdef DEBUG_SYSTEM
			dbg("RemoveService() Failed, Error Code: %d", GetLastError());
			#endif

			CloseServiceHandle(Service);
			CloseServiceHandle(SCManager);

			return false;

		}
		else
		{
		//	buffer = (char *)XHeapAlloc(DATA_BUFFER);
	
		//	_snprintf(buffer, DATA_BUFFER, "Current State: %d\r\nExit Code: %d\r\nService Specific Exit Code: %d\r\nCheck Point: %d\r\nWait Hint: %d", ServiceStatus.dwCurrentState, ServiceStatus.dwWin32ExitCode, ServiceStatus.dwServiceSpecificExitCode, ServiceStatus.dwCheckPoint, ServiceStatus.dwWaitHint);
	
		//	xsend(client->CommandSocket, client->SecureKey, 3, "%s", buffer);

			CloseServiceHandle(Service);
			CloseServiceHandle(SCManager);

		//	XHeapFree(buffer);

			return true;
		}
	}

	return false;
}
