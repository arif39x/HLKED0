/*
*    ZION DEFINIATION FOR BOTH SERVER AND CLIENT
*
*    Org. By Shikai Chen
*	 Modified. By Liang Chen
*    Version 0.2
*/



#pragma once

#define STR_OBJ_NAME L"Zion_srv"

#define STR_STD_OBJ_NAME L"\\Device\\"STR_OBJ_NAME
#define STR_DOS_OBJ_NAME L"\\DosDevices\\"STR_OBJ_NAME

//Specify the device type for ZION
#define FILE_DEVICE_ZION      0x00002a7c

#define ZION_SERVER_VERSION 0x00000001
                           //0x (____) | (____)
                           //     ^         +------- Sub Version
                           //     +----------------- Major Version

#include "zion_ioctl_def.h"

#include "zion_cmd_packet.h"