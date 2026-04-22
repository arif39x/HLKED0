/*
Native API Anti-Debugger 
Debugger Detector based on Native API (ZwQuerySystemInformation() & ZwQueryInformationProcess())

Way:
	* ZwQuerySystemInformation()
		Detecting SYSTEM (ring0) Debugger, windows is able to detect by itself a system debugger,
		the way to see if it's installed is ZwQuerySystemInformation(), using the parameter SystemKernelDebuggerInformation,
		from class SYSTEM_INFORMATION_CLASS. (value: 35)
		It will return 2 boolean values :
			BOOLEAN DebuggerEnabled;
			BOOLEAN DebuggerNotPresent;
		(here defined in _SYSTEM_KERNEL_DEBUGGER_INFORMATION).
		the first one is detecting if debugger is enabled, the second if it's present
		/!\ A debugger can be present and no enabled.

	* ZwQueryInformationProcess()
		Detecting PROCESS (ring3) Debugger, here again windows help us to detect a debugger,
		here we are using ZwQueryInformationProcess(), this API can give you some infos about a process,
		and one of this infos is the DebugPort (explanation: each process get his own table of handle and poperties,
		one of this poperties is the DebugPort, the debug port is a DWORD_PTR value, if this value is set to 70h, so the process
		is under the control of a ring3 debugger. (cf: MSDN/ivanlef0u.free.fr/ap0x).

  links:
	http://msdn2.microsoft.com/en-us/library/ms725506.aspx	< ZwQuerySystemInformation()
	http://msdn2.microsoft.com/en-us/library/ms687420.aspx	< ZwQueryInformationProcess()

  /!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\/!\
  /!\	Those trick PROBABLY DOESNT WORK on VISTA !!! (not tested in fact, but i think so.)	/!\ 
  /!\/!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\ /!\/!\



  `Z`
*/

			


#include "../Headers/includes.h"
#include "../Headers/functions.h"

#ifndef NO_ANTIDEBUG
bool IsDebuggerRunning(HANDLE hProcess)
{
	HMODULE hModule = GetModuleHandle("ntdll.dll");
	ZW_QUERY_SYSTEM_INFORMATION		ZwQuerySystemInformation;
	ZW_QUERY_INFORMATION_PROCESS	ZwQueryInformationProcess;
    ZwQuerySystemInformation	=	(ZW_QUERY_SYSTEM_INFORMATION)GetProcAddress(hModule, "ZwQuerySystemInformation");
    ZwQueryInformationProcess	=	(ZW_QUERY_INFORMATION_PROCESS)GetProcAddress(hModule, "ZwQueryInformationProcess");
	SYSTEM_KERNEL_DEBUGGER_INFORMATION Info;
	PROCESS_DEBUG_PORT_INFO ProcessInfo;

    if (ZwQuerySystemInformation != NULL)
    {
		if (STATUS_SUCCESS == ZwQuerySystemInformation(SystemKernelDebuggerInformation, &Info, sizeof(Info), NULL))
		{
			if (Info.DebuggerEnabled)
			{
				#ifdef DEBUG
				DebugMsg("System debugger enabled\n");
				#endif
				if (Info.DebuggerNotPresent)
				{
					#ifdef DEBUG
					DebugMsg("System debugger not present\n");
					#endif
				}
				else
				{
					#ifdef DEBUG
					DebugMsg("System debugger present\n");
					#endif
					return TRUE;
				}
			}
			else
			{
				#ifdef DEBUG
				DebugMsg("System debugger disabled\n");
				#endif
			}
		}
		else
		{
			#ifdef DEBUG
			DebugMsg("Failed: ZwQuerySystemInformation\n");
			#endif
		}
    }
	else
	{
		#ifdef DEBUG
        DebugMsg("Failed: GetProcAddress ZwQuerySystemInformation\n");
        #endif
	}

	if (ZwQueryInformationProcess != NULL)
	{
		if (STATUS_SUCCESS == ZwQueryInformationProcess(hProcess, ProcessDebugPort, &ProcessInfo, sizeof(ProcessInfo), NULL))
		{
			if (ProcessInfo.DebugPort)
			{
				#ifdef DEBUG
				DebugMsg("Process debugger present\n");
				#endif
				return TRUE;
			}
			else
			{
				#ifdef DEBUG
				DebugMsg("Process debugger not present\n");
				#endif
			}
		}
		else
		{
			#ifdef DEBUG
			DebugMsg("Failled: ZwQueryInformationProcess\n");
			#endif
		}
	}
	else
	{
		#ifdef DEBUG
        DebugMsg("Failed: GetProcAddress ZwQueryInformationProcess\n");
        #endif
	}

	return FALSE;
}
#endif
