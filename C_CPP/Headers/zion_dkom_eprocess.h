/*
 *      name: ZION system DKOM Eprocess User
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#include "stdafx.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h"

//#include "proxy_internal.h"
#include "zion/zion_ioctl_def.h"
#include "zion/zion_define.h"
#include "zion/zion_cmd_packet.h"
#include "zion/zion_dkom_def.h"
#include "zion/proxy/zion_proxy_dll.h"

#include "zion_ssdthook_cmdsender.h"

ZIONRESULT ZION_SetDKOMEPROCESSCMDData(PZION_DKOM_EPROCESS_CMD_DATA data,
							   DWORD pid);
