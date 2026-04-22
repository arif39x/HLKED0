// Lithium Server Plugin Sample Header

#include <windows.h>
#include "ServerAPI.h"

#ifndef _SRVPLUGIN_
#define _SRVPLUGIN_

// Define server EXE function pointers
// (required by ServerAPI.c)

extern LFT_lsSendMsg *lsSendMsg;
extern LFT_lsGetCfg *lsGetCfg;

// Define command constants

#define SP_COMMAND1		0
#define SP_COMMAND2		1
#define SP_COMMAND3		2
#define SP_COMMAND4		3

#endif