/*
 *      name: SSDT HOOK MODULE
 *      type: tool
 *
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
/*
 *      name: ZION system SSDT HOOK kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#include "zion_server_helper.h"
#define NAKE_FUNC_STUB_SIGNATURE "\x90\x90\x90\x90"
#define NAKE_JMP_ORIG_SIGNATURE "\x8b\xff\x8b\xff\x8b\xff"
typedef struct _ZION_Service_Descriptor_Entry {
    unsigned int *ServiceTableBase;
    unsigned int *ServiceCounterTableBase; 
    unsigned int NumberOfServices;
    unsigned char *ParamTableBase;
} ZION_Service_Descriptor_Entry, *PZION_Service_Descriptor_Entry;

ULONG SSDT_GetStubLength(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData);
ULONG SSDT_GetServiceNum(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData);

ULONG SSDT_HOOK_PROC(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData);
//
PVOID addrofOrigAddr;
PVOID ptrFuncOrigAddr;
//

extern PZION_Service_Descriptor_Entry KeServiceDescriptorTable;


