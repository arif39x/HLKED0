/*
 *      name: SSDT HOOK MODULE
 *      type: tool
 *
 *      Org Author: Liang Chen
 *      Version : 0.2
 */

//
#include <ntddk.h>
#include <windef.h>
#include <memory.h>
#include "../zion_proxy_dll/zion_ssdt_hook.h"
#include "zion_ssdt_hook_impl.h"

#include "zion/server/zion_common.h"
#include "zion/zion_cmd_packet.h"

//#include "zion/server/zion_ioctl_mapper.h"

//#include "server_ioctl_dispatcher.h"

BOOL SSDT_HOOK_SERVER(PVOID ptrBufferAddr)
{
	//safe_trace("%x",PsGetCurrentProcess());
	PVOID ptrProxySSDTHookData = ExAllocatePool(
												PagedPool,
												sizeof(ZION_SSDT_HOOK_CMD_DATA)
												);
	ULONG dwStubSize;
	PVOID UserStubBuffer;
	if (NULL == ptrProxySSDTHookData)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser((PZION_SSDT_HOOK_CMD_DATA)((PZION_CS_PACKET)ptrBufferAddr)->pData,
						   sizeof(ZION_SSDT_HOOK_CMD_DATA),
						   ptrProxySSDTHookData))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub = 
			(*(PLONG)((PBYTE)((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub + 1)) + 
			(LONG)((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub + 5;
	dwStubSize = SSDT_GetStubLength((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData);
	UserStubBuffer = (PVOID)((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub;
	((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub =(DWORD) ExAllocatePool(NonPagedPool,
																				   dwStubSize
																					);
	if (NULL == ((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser(UserStubBuffer,
						   dwStubSize,
						   (PVOID)((PZION_SSDT_HOOK_CMD_DATA)ptrProxySSDTHookData)->AddressOfStub))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	if (1 != SSDT_HOOK_PROC(ptrProxySSDTHookData))
	{
		safe_trace("ZION_SERVER: hook error.\n");
		return 0;
	}
	return 1;
	
}

ULONG SSDT_GetStubLength(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData)
{
	PMDL ptrUserMDL;
	ULONG dwBytesRead;
	PVOID ptrStubAddr;
	PVOID ptrCurrent;
	if (NULL == ptrSSDTHookData)
	{
		safe_trace("ZION_SERVER: SSDT DATA ERROR.\n");
		return -1;
	}
	ptrStubAddr = (PVOID)ptrSSDTHookData->AddressOfStub;
	if (NULL == ptrStubAddr)
	{
		safe_trace("ZION_SERVER: SSDT FUNC DATA ERROR.\n");
		return -1;
	}
	ptrCurrent = ptrStubAddr;
	dwBytesRead = 4;
	while (1)
	{
		ptrUserMDL = IoAllocateMdl(ptrStubAddr, 
			(dwBytesRead/PAGE_SIZE + 1) * PAGE_SIZE, 
			FALSE, 
			FALSE, 
			NULL); 
		if(!ptrUserMDL) 
		{ 
			safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
			return -1; 
		} 

		MmProbeAndLockPages(ptrUserMDL,
			KernelMode,
			IoReadAccess
			);
		while (RtlCompareMemory(ptrCurrent, NAKE_FUNC_STUB_SIGNATURE,sizeof(NAKE_FUNC_STUB_SIGNATURE) - 1) != (sizeof(NAKE_FUNC_STUB_SIGNATURE) - 1))
		{
			ptrCurrent = (PVOID)((PBYTE)ptrCurrent + 1);
			if ((dwBytesRead % PAGE_SIZE) == 0) break;
			dwBytesRead ++;
		}
		if (RtlCompareMemory(ptrCurrent, NAKE_FUNC_STUB_SIGNATURE,sizeof(NAKE_FUNC_STUB_SIGNATURE) - 1) == (sizeof(NAKE_FUNC_STUB_SIGNATURE) - 1))
		{
			MmUnlockPages(ptrUserMDL);
			IoFreeMdl(ptrUserMDL);
			break;
		}
		MmUnlockPages(ptrUserMDL);
		IoFreeMdl(ptrUserMDL);
	}
	return ((ULONG)ptrCurrent - (ULONG)ptrStubAddr + sizeof(NAKE_FUNC_STUB_SIGNATURE) - 1);
}

ULONG SSDT_GetServiceNum(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData)
{
	if (wcscmp(ptrSSDTHookData->FuncName,L"NtQuerySystemInformation") == 0)
	return 0xad;
	if (wcscmp(ptrSSDTHookData->FuncName,L"NtQueryDirectoryFile") == 0)
	return 0x91;

	return 0x00;
}

ULONG SSDT_HOOK_PROC(PZION_SSDT_HOOK_CMD_DATA ptrSSDTHookData)
{
	UNICODE_STRING wcsRoutineName;
	
	PVOID ptrCurrent;
	PVOID storeAddress;
	RtlInitUnicodeString(&wcsRoutineName,ptrSSDTHookData->FuncName);
	/*ptrFuncOrigAddr = MmGetSystemRoutineAddress(&wcsRoutineName);
	if (!ptrFuncOrigAddr)
	{
		safe_trace("ZION_SERVER: no proc in ntoskrl.exe.\n");
		return -1;
	}*/
	//addrofOrigAddr = &ptrFuncOrigAddr;
	ptrCurrent = ptrSSDTHookData->AddressOfStub;
	storeAddress = ExAllocatePool(NonPagedPool,32);
	if (NULL == storeAddress)
	{
		safe_trace("ZION_SERVER: Memory alloc error.\n");
		return -1;
	}

	while (RtlCompareMemory(ptrCurrent, NAKE_JMP_ORIG_SIGNATURE,sizeof(NAKE_JMP_ORIG_SIGNATURE) - 1) != (sizeof(NAKE_JMP_ORIG_SIGNATURE) - 1))
	{
		ptrCurrent = (PVOID)((PBYTE)ptrCurrent + 1);
	}
	*(PWORD)ptrCurrent = 0x15FF;
	ptrCurrent = (PVOID)((PBYTE)ptrCurrent + 2);
	//*(PDWORD)ptrCurrent = (DWORD)addrofOrigAddr;
	 _asm
    {
        CLI                    //dissable interrupt
        MOV    EAX, CR0        //move CR0 register into EAX
        AND EAX, NOT 10000H //disable WP bit 
        MOV    CR0, EAX        //write register back
    }

    *(PDWORD)storeAddress = (DWORD)(*(((PZION_Service_Descriptor_Entry)KeServiceDescriptorTable)->ServiceTableBase + SSDT_GetServiceNum(ptrSSDTHookData))) ;
	//获得原始地址
	*(PDWORD)ptrCurrent = (DWORD)storeAddress; //填上原始地址
	(*(((PZION_Service_Descriptor_Entry)KeServiceDescriptorTable)->ServiceTableBase + SSDT_GetServiceNum(ptrSSDTHookData))) = ptrSSDTHookData->AddressOfStub;
    _asm 
    {
        MOV    EAX, CR0        //move CR0 register into EAX
        OR    EAX, 10000H        //enable WP bit     
        MOV    CR0, EAX        //write register back        
        STI                    //enable interrupt
    }


	return 1;
}