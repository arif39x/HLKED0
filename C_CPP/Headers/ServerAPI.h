/*
	Lithium Remote Administration Tool
	Plugin API Source by Olympus
	Copyright (c) 2001-2002 Olympus.

	-= Header =-
*/

#ifndef _SERVERAPI_
#define _SERVERAPI_

// ServerAPI.c functions

void CliConsoleMsg(char *Message, unsigned short ClientIndex);
void CliPluginMsg(char *PluginName, unsigned short cmd, unsigned short argc, char **args, unsigned long *argl, unsigned short ClientIndex);

// Server EXE function definitions

typedef void (LFT_lsSendMsg)(unsigned short, unsigned short, char **, unsigned long *, unsigned short);
typedef void (LFT_lsGetCfg)(char *Name, char **Value);

// Plugin information structure

typedef struct {
	char *Name;
} PluginInfo;

// Plugin initialization structure

typedef struct {
	LFT_lsSendMsg *pSendMsg;
	LFT_lsGetCfg *pGetCfg;
} PluginData;

// Plugin function definitions (unnecessary, provided for clarity)

typedef void (LFT_lspGetInfo)(PluginInfo *);
typedef void (LFT_lspInitialize)(PluginData *);
typedef void (LFT_lspCleanup)(void);
typedef void (LFT_lspParseMsg)(unsigned short, unsigned short, char **, unsigned long *, unsigned short);

// Plugin structure (unnecessary, provided for clarity)

typedef struct {
	HMODULE Handle;
	char *FileName, *Name;
	LFT_lspGetInfo *lspGetInfo;
	LFT_lspInitialize *lspInitialize;
	LFT_lspCleanup *lspCleanup;
	LFT_lspParseMsg *lspParseMsg;
} LithiumPlugin;

#endif