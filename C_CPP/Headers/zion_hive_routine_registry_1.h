/*
 *      name: ZION system Register User
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
#include "zion_packet_helper.h"
ZIONRESULT Hive_SetHiveData(PZION_HIVE_REG_HOOK_CMD_DATA data,
							   WCHAR *RegKeyPath);