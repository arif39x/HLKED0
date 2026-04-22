/*
 *      name: SSDT HOOK 
 *      type: tool
 *		Org Author: Liang Chen
 *      Version : 0.2
 */
///////////////////////////////////////////////////////////
// introduction:
//	send a zion std packet to the kernel mode using IOCTRL
//  function.
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//	In order to HOOK a function in SSDT , we need to call 
//	IOCTRL. The sub hook command  


#pragma once
#define CMD_SUB_HOOK_SSDT	0x0001u
#define CMD_SSDT_HOOK ((CMD_MAJOR_HOOK << 16) | CMD_SUB_HOOK_SSDT)

#define SSDT_FUNC_NAME_MAXLEN 100

typedef struct _ZION_SSDT_HOOK_CMD_DATA
{
	DWORD AddressOfStub;
	WCHAR FuncName[SSDT_FUNC_NAME_MAXLEN];
	//DWORD dwEPROCESS;
} ZION_SSDT_HOOK_CMD_DATA, *PZION_SSDT_HOOK_CMD_DATA;


