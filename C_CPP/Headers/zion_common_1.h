/*
 *      name: ZION system common header
 *      type: Kernel Module
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */
#include <ntddk.h>
#include <windef.h>

#pragma once



#define ZION_SERVER_MODE
//if Defined, meaning the code runs in Ring 0

#define ZION_HIDE_SERVER

#define ZION_SERVER_VERSION 0x00000001
                           //0x (____) | (____)
                           //     ^         +------- Sub Version
                           //     +----------------- Major Version


#define trace DbgPrint("[%d:%s()]", __LINE__,__FUNCTION__);DbgPrint

#ifdef ZION_DEBUG_VERSION
#define safe_trace trace
#else
#define safe_trace (void)
#endif



