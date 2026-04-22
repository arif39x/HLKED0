/*
 *      name: SSDT HOOK COMMAND SENDER
 *      type: USER MODE
 *		Org Author: Liang Chen
 *      Version : 0.2
 */
#pragma once
#include "stdafx.h"
#include "zion_packet_helper.h"





extern HANDLE      g_hZionServer_Dev;
#define NT_SUCCESS(status)          ((NTSTATUS)(status)>=0)
#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED        ((NTSTATUS)0xC0000022L)
//////

//////
ZIONRESULT SSDT_HOOK(PVOID ptrFuncStub, 
					 PWCHAR wcsFuncName
					 );


ZIONRESULT ZION_SetSSDTCMDData(PZION_SSDT_HOOK_CMD_DATA data,
							   DWORD ptrFuncAddr,
							   PWCHAR SSDTFuncName
							   );
typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG            ProcessId;
    UCHAR            ObjectTypeNumber;
    UCHAR            Flags;
    USHORT            Handle;
    PVOID            Object;
    ACCESS_MASK        GrantedAccess;
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;
ZIONRESULT GetEPROCESSAddr(DWORD pid);
typedef enum _SYSTEM_INFORMATION_CLASS
{
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS ( __stdcall *ZWQUERYSYSTEMINFORMATION ) ( IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
														  IN OUT PVOID SystemInformation, 
														  IN ULONG SystemInformationLength,
														  OUT PULONG ReturnLength OPTIONAL );