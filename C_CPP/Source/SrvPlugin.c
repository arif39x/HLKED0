// Lithium Server Plugin Sample Code


#include <windows.h>
#include "SrvPlugin.h"
#include "ServerAPI.h"
#include "Shared.h" // client side command constants
#include "portscan.h"

// Define plugin name

static char PluginName[] = "PortScan2";


/* Server EXE functions -
  lsSendMsg() - Use this function to send raw data back to
    the client if necessary. Most of the time, usage of this
    raw function will be unnecessary, as the ServerAPI.c
    functions provide wrappers (CliConsoleMsg() and
	CliPluginMsg()) for console and client plugin messaging.
	Note: the client index variable "ci" must be specified so
	the server can send the message through the appropriate
	client handler. Ex:

      unsigned short cmd;
	  unsigned short argc;
	  char *args[1];
	  unsigned long argl[1];

	  cmd = LCC_CONSOLEMSG;
	  argc = 1;
	  args[0] = "Hello world!";
	  argl[0] = strlen(args[0]);

      lsSendMsg(cmd, argc, args, argl, ci);

  lsGetCfg() - Use this function to fetch a configuration
    variable pointer from the server. (Null terminated) Ex:

	  char *value;
	  lsGetCfg("ServerID", &value);

*/

LFT_lsSendMsg *lsSendMsg = NULL;
LFT_lsGetCfg *lsGetCfg = NULL;


/* lspGetInfo() - Called by Server API to fetch the plugin's
 unique name. Do not modify. This function must be exported
 in SrvPlugin.def */

__declspec(dllexport) void lspGetInfo(PluginInfo *pi)
{
	pi->Name = (char *)&PluginName;
}


/* lspInitialize() - Called by Server API on plugin load; add
 any initialization routines here. The server simply passes
 pointers to data sending and configuration functions here.
 If the server plugin needs to run constantly, a thread
 should be created here. This function must be exported in
 SrvPlugin.def */

__declspec(dllexport) void lspInitialize(PluginData *pd)
{
	lsSendMsg = pd->pSendMsg;
	lsGetCfg = pd->pGetCfg;

	InitScanList();
}


/* lspCleanup() - Called by ServerAPI on exit; add any
 cleanup routines here. This function must be exported
 in SrvPlugin.def */

__declspec(dllexport) void lspCleanup()
{
	StopAllPortscans();
}


/* lspParseMsg() - Called by ServerAPI on the arrival
 of data to this specific plugin. "cmd" should be used
 as your main function code on which a switch statement
 can easily be done. Any arguments will be placed in
 the argument arrays. The variable "ci" specifies the
 client handler index and is needed to send data to the
 client. This function must be exported in SrvPlugin.def */

__declspec(dllexport) void lspParseMsg(unsigned short cmd, unsigned short argc, char **args, unsigned long *argl, unsigned short ci)
{
	char cCmd[2];
	memcpy(cCmd,&cmd,2);

	switch (tolower(*cCmd))
	{
		case (PSP_STOP): {
			PORTSCAN_ID dId;
			if (argc != 1) {
				ErrorInArgs(ci);
				return;
			}

			dId = atoi(args[0]);

			if (!dId){
				ErrorInArgs(ci);
				return;
			}

			StopPortScan(dId,ci);
			break;
		}
		case (PSP_START): {

			if (argc != 2) {
				ErrorInArgs(ci);
				return;
			}

			StartPortScan(args[0],args[1],ci);
			break;
		}
		case (PSP_LIST): {
			ListPortScans(ci);
			break;
		}
		case (PSP_PAUSE): {

			PORTSCAN_ID dId;
			if (argc != 1) {
				ErrorInArgs(ci);
				return;
			}

			dId = atoi(args[0]);

			if (!dId){
				ErrorInArgs(ci);
				return;
			}
			PausePortScan(dId,ci);
			break;
		}
		case (PSP_SALL): {
			StopAllPortscans();
			break;
		}
		case (PSP_INFO): {

			PORTSCAN_ID dId;
			if (argc != 1) {
				ErrorInArgs(ci);
				return;
			}

			dId = atoi(args[0]);

			if (!dId){
				ErrorInArgs(ci);
				return;
			}
			PortScanInfo(dId,ci);
			break;
		}
		default: {
			ErrorInArgs(ci);
		}

	}

}


// DLL EntryPoint

__declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			break;
		}
	}
	return TRUE;
}

