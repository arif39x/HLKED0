/*
 *      name: ZION system tdi hook user
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#include "stdafx.h"
#include "malloc.h"
#include "stdio.h"
#include "string.h"

#include "zion/zion_ioctl_def.h"
#include "zion/zion_define.h"
#include "zion/zion_cmd_packet.h"
#include "zion/zion_tdi_connection_def.h"
#include "zion/proxy/zion_proxy_dll.h"
#include "zion_packet_helper.h"

ZIONRESULT SetTDIData(PZION_TDI_CONNECTION_CMD_DATA data,
							   DWORD Type,
							   DWORD SrcAddr,
							   USHORT SrcPort,
							   DWORD DstAddr,
							   USHORT DstPort);