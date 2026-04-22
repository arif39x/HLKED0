/*
	Lithium Remote Administration Tool
	Plugin API Source by Olympus
	Copyright (c) 2001-2002 Olympus.

	-= Functions =-
*/

#include <windows.h>
#include "SrvPlugin.h"
#include "ServerAPI.h"
#include "Shared.h"

// Send a console message to client

void CliConsoleMsg(char *Message, unsigned short ClientIndex)
{
	unsigned long msgl = strlen(Message);
	lsSendMsg(LCC_CONSOLEMSG, 1, &Message, &msgl, ClientIndex);
}

// Send a message to client plugin
/*
void CliPluginMsg(char *PluginName, unsigned short cmd, unsigned short argc, char **args, unsigned long *argl, unsigned short ClientIndex)
{
	char **pargs;
	unsigned long *pargl;
        unsigned short i;

	pargs = (char **)malloc((argc + 2) * sizeof(char *));
	pargl = (unsigned long *)malloc((argc + 2) * sizeof(unsigned long));

	pargs[0] = PluginName;
	pargs[1] = (char *)&cmd;
	pargl[0] = strlen(PluginName);
	pargl[1] = sizeof(cmd);

	for (i = 0; i < argc; i++)
	{
		pargs[i + 2] = args[i];
		pargl[i + 2] = argl[i];
	}

	lsSendMsg(LCC_PLUGINMSG, argc + 2, pargs, pargl, ClientIndex);

	free(pargs);
	free(pargl);
}
*/
