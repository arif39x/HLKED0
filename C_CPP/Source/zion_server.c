/*
 *      name: ZION server
 *      type: Kernel Module
 *
 *      Org Author: Shikai Chen
 *		Modified: Liang Chen
 *      Version : 0.2
 */
#pragma once
#include <ntddk.h>
#include <windef.h>
#include "zion_server.h"
#include "zion/server/zion_common.h"
#include "zion/zion_define.h"
#include "zion/server/zion_ioctl_mapper.h"
#include "zion_file_filter_driver_impl.h"
#include "server_ioctl_dispatcher.h"
#include "zion_hive_register_routine_impl.h"
#include "zion_tdi_connection_impl.h"
#include "zion_file_filter_driver_fastIO_dispatcher.h"

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath);

#ifdef ALLOC_PRAGMA
  #pragma alloc_text(INIT, DriverEntry)
#endif
 

static
VOID
DriverUnload(IN PDRIVER_OBJECT DriverObject);

static
NTSTATUS
hook_DefDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


static
NTSTATUS
hook_IOCTRDispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


#ifdef ALLOC_DATA_PRAGMA
  #pragma data_seg(".eda")
#endif
/* Paged Data Here! */


#ifdef ALLOC_DATA_PRAGMA
  #pragma data_seg()
#endif
 
PDEVICE_OBJECT  deviceObject;
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
    UNICODE_STRING  str_uni_name, str_uni_linkString;
    NTSTATUS        status;
	PDEVICE_EXTENSION DeviceExtension;
	PDEVICE_OBJECT DiskDeviceObject;
	PDEVICE_OBJECT NewDeviceObject;
	UNICODE_STRING str_volume;
	IO_STATUS_BLOCK ioStatus;
	OBJECT_ATTRIBUTES objectAttributes;
	PFILE_OBJECT fileObject = NULL;
	HANDLE ntFileHandle;
	CHAR namebuf[512];
	int retLength;
	int i;

	RtlInitUnicodeString( &str_uni_name,STR_STD_OBJ_NAME );
	RtlInitUnicodeString( &str_volume, L"\\DosDevices\\C:\\");
	status = IoCreateDevice(
							DriverObject,
							0,
							&str_uni_name,
							FILE_DEVICE_DISK_FILE_SYSTEM,
							FILE_DEVICE_SECURE_OPEN,
							FALSE,
							&deviceObject	
							);
    
    if (!NT_SUCCESS( status )){
        safe_trace("ERROR: failed to create device.\n");
		return status;
    }
    DriverObject->DriverUnload = DriverUnload;
    
	RtlInitUnicodeString( &str_uni_linkString, STR_DOS_OBJ_NAME );
	status = IoCreateSymbolicLink (&str_uni_linkString, &str_uni_name);
    
    if (!NT_SUCCESS( status ))
	{
        safe_trace("ERROR: failed to create symbolic link.\n");
		IoDeleteDevice (DriverObject->DeviceObject);
		return status;
	}
    
	for ( i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)    {

		DriverObject->MajorFunction[i] = hook_DefDispatch;
	}
     
	DriverObject->MajorFunction[IRP_MJ_CREATE]= 
	DriverObject->MajorFunction[IRP_MJ_CLOSE] =  
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = hook_DefDispatch;
   

    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]  = hook_Server_IOCTL_handler;
	DriverObject->MajorFunction[IRP_MJ_DIRECTORY_CONTROL] = FsDirectoryControl;

	InitFastIo(DriverObject);
	InitializeObjectAttributes(&objectAttributes, &str_volume, OBJ_CASE_INSENSITIVE, NULL, NULL );
	status = ZwCreateFile(&ntFileHandle, SYNCHRONIZE|FILE_ANY_ACCESS, 
							&objectAttributes, &ioStatus, NULL, 0, 
							FILE_SHARE_READ|FILE_SHARE_WRITE, 
							FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT|FILE_DIRECTORY_FILE, 
							NULL, 0);

	if (!NT_SUCCESS(status))
	{
		DbgPrint(("Filemon: Could not open drive\n"));
		return FALSE;
	}
	KdPrint(("Open Device is OK\n"));
	//
	// Got the file handle, so now look-up the file-object it refers to
	//
	status = ObReferenceObjectByHandle(ntFileHandle, FILE_READ_DATA, NULL, KernelMode, &fileObject, NULL);
	if(!NT_SUCCESS(status)) 
	{
		DbgPrint(("Filemon: Could not get fileobject from handle\n"));
		ZwClose(ntFileHandle);
		return FALSE;			
	}
	DiskDeviceObject = IoGetRelatedDeviceObject(fileObject);
	if(!DiskDeviceObject) 
	{
		DbgPrint(("Filemon: Could not get related device object\n"));
		goto ErrHand;
	}

	status = IoCreateDevice(
							DriverObject,
							sizeof(DEVICE_EXTENSION),
							NULL,
							DiskDeviceObject->DeviceType,
							FILE_DEVICE_SECURE_OPEN,
							FALSE,
							&NewDeviceObject
							);
	if (!NT_SUCCESS(status)) 
	{
		goto ErrHand;
	}
	NewDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	DeviceExtension = NewDeviceObject->DeviceExtension;
	DeviceExtension->AttachedToDevice = DiskDeviceObject;
	DeviceExtension->DeviceName.Length = 0;
	DeviceExtension->DeviceName.MaximumLength = sizeof(DeviceExtension->DeviceNameBuffer);
	DeviceExtension->DeviceName.Buffer = DeviceExtension->DeviceNameBuffer;
	status = ObQueryNameString(deviceObject, (POBJECT_NAME_INFORMATION)namebuf, sizeof(namebuf), &retLength);
	DeviceExtension->DeviceName.Length = 0;
	if (NT_SUCCESS( status )) 
	{
		RtlCopyUnicodeString(&(DeviceExtension->DeviceName), &((POBJECT_NAME_INFORMATION)namebuf)->Name);
		//KdPrint(("Object %wZ,Len = %d.[%d]\n", Name, retLength, Name->Length));
	}
	DeviceExtension->AttachedToDevice = IoAttachDeviceToDeviceStack(NewDeviceObject, DiskDeviceObject);
	if (!DeviceExtension->AttachedToDevice)
	{
		goto ErrHand;
	}
	InitializeListHead(&g_HideObjHead);
	KdPrint(("Attath Device is OK\n"));
	ObDereferenceObject(fileObject);
	ZwClose(ntFileHandle);
	return TRUE;
ErrHand:
	KdPrint(("AttachDevice Failed status %08x\n", status));
	ObDereferenceObject(fileObject);
	ZwClose(ntFileHandle);
	return  FALSE;

	//
    safe_trace("Zion Server loaded\n");


    return STATUS_SUCCESS; 
}




static VOID DriverUnload(IN PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT       deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING       str_uni_name;
	PDEVICE_OBJECT DO;
	PDEVICE_OBJECT NextDO = NULL;
	PDEVICE_EXTENSION DevEx = NULL;

	PLIST_ENTRY pdLink = NULL;
	PHIDE_FILE  pHideObj = NULL;
	if (zion_ptrGetCellRoutine)
		   *zion_ptrGetCellRoutine = zion_pGetCellRoutine;
	if (g_OldIrpMjDeviceControl != NULL)
		InterlockedExchange ((PLONG)&(g_pDev_tcp->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]), (LONG)g_OldIrpMjDeviceControl);	
	if (g_pFile_tcp != NULL)
	{
		ObDereferenceObject(g_pFile_tcp);
		ObDereferenceObject(g_pDev_tcp);
	}
	g_pFile_tcp = NULL;
	g_pDev_tcp = NULL;

	//卸载资源
	
	//释放隐藏文件链表
	/*while(!IsListEmpty(&g_HideObjHead))
	{
		pdLink = RemoveHeadList(&g_HideObjHead);
		pHideObj = (PHIDE_FILE)CONTAINING_RECORD(pdLink, HIDE_FILE, linkfield);
		ExFreePool(pHideObj);
	}*/
	//卸载驱动

	DO = DriverObject->DeviceObject;
	while(DO) 
	{
		

			NextDO = DO->NextDevice;
			DevEx = DO->DeviceExtension;
			if (DevEx)
			{
				if (DevEx->AttachedToDevice)
				{
					IoDetachDevice(DevEx->AttachedToDevice);
				}
			}
			IoDeleteDevice(DO);
			DO = NextDO;
		}

	//释放FastIo
	DestoryFastIo(DriverObject);
	//删除符号链接

	RtlInitUnicodeString( &str_uni_name, STR_DOS_OBJ_NAME );
	IoDeleteSymbolicLink( &str_uni_name );
	ASSERT(!deviceObject->AttachedDevice);
	IoDeleteDevice( deviceObject );

    safe_trace("Zion Server Unloaded\n");
  
	return;
}





#ifdef ALLOC_PRAGMA
    #pragma code_seg("PAGE")
#endif
/* Paged Code Here ! */

static NTSTATUS hook_DefDispatch (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{ 
	if (deviceObject == DeviceObject) //如果请求发送至功能驱动
	{
		KdPrint(("CDO Closed!\n"));
		Irp->IoStatus.Information = 0;
		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);//完成该请求
		return STATUS_SUCCESS;
	}
	return ZionPassThrough(DeviceObject, Irp);//如果发送至过滤设备，将IRP发送至下层驱动
}
NTSTATUS
ZionPassThrough(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);

	if (deviceObject == DeviceObject)
	{
		KdPrint((("ERROR CDO:[%08x]\n"), irpSp->MajorFunction, irpSp->MinorFunction));
		Irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
		Irp->IoStatus.Information = 0;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_INVALID_DEVICE_REQUEST;
	}
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)DeviceObject->DeviceExtension)->AttachedToDevice, Irp);
}



#ifdef ALLOC_PRAGMA
    #pragma code_seg()
#endif