/*
 *      name: ZION system Packet helper func
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
#include "test_ssdt_func.h"
#include "zion/zion_define.h"
#include "zion/zion_cmd_packet.h"
#include "zion/proxy/zion_proxy_dll.h"

#include "zion_ssdt_hook.h"
#include "zion_ssdthook_cmdsender.h"
ZIONRESULT ZION_SetPacketValue(PZION_CS_PACKET Packet,
							   DWORD cmd,
							   PVOID ptrData,
							   size_t s_datasize);