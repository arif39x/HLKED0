#include "zion_file_filter_driver_impl.h"

NTSTATUS
FsDirectoryControl(IN PDEVICE_OBJECT DeviceObject,
				   IN PIRP Irp)
{
	NTSTATUS status;
	PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);	//当前Irp(IO_STACK_LOCATION)的参数
	PDEVICE_EXTENSION devExt = DeviceObject->DeviceExtension;
	PFILE_BOTH_DIR_INFORMATION dirInfo = NULL;
	KEVENT waitEvent;
	//UNICODE_STRING path;
	ASSERT(g_CDO != DeviceObject);
	ASSERT(IS_MY_DEVICE_OBJECT(DeviceObject));
	if (IRP_MN_QUERY_DIRECTORY != irpSp->MinorFunction)
	{
		goto SkipHandle;
	}
	if (Irp->RequestorMode == KernelMode)
	{
		goto SkipHandle;
	}
    if (FileBothDirectoryInformation != ((PQUERY_DIRECTORY)&irpSp->Parameters)->FileInformationClass) 
	{	
        goto SkipHandle;
    }
	//设置完成回调函数
    KeInitializeEvent(&waitEvent, NotificationEvent, FALSE);
    IoCopyCurrentIrpStackLocationToNext(Irp);
	//IoSetCompletionRoutine(Irp,CompletionRoutine,context,InvokeOnSuccess,InvokeOnError,InvokeOnCancel);
    IoSetCompletionRoutine(	
							Irp,
							DirControlCompletion,		//CompletionRoutine
							&waitEvent,					//context parameter
							TRUE,
							TRUE,
							TRUE
							);

	status = IoCallDriver(devExt->AttachedToDevice, Irp);
	if (STATUS_PENDING == status)
	{
		//等待完成
        status = KeWaitForSingleObject(&waitEvent,
										Executive,
										KernelMode,
										FALSE,
										NULL
										);
        ASSERT(STATUS_SUCCESS == status);
	}
    if (!NT_SUCCESS(status) ||(0 == irpSp->Parameters.QueryFile.Length)) 
	{	
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }
	//KdPrint(("Hook Directory.\n"));
	HandleDirectory(Irp->UserBuffer,  &((PQUERY_DIRECTORY)&irpSp->Parameters)->Length);
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;

SkipHandle:
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(devExt->AttachedToDevice, Irp);
}

BOOLEAN
HandleDirectory(IN OUT PFILE_BOTH_DIR_INFORMATION DirInfo, IN PULONG lpBufLenth)
{
	//处理目录操作
	PFILE_BOTH_DIR_INFORMATION currentDirInfo = DirInfo;
	PFILE_BOTH_DIR_INFORMATION lastDirInfo = NULL;
	ULONG offset = 0;
	ULONG position = 0;
	ULONG newLenth = *lpBufLenth;
	WCHAR fileName[] = L"Test.txt";
	do
	{
		offset = currentDirInfo->NextEntryOffset;
		//if (!(FILE_ATTRIBUTE_DIRECTORY & currentDirInfo->FileAttributes) &&
		//	 (0 == wcsncmp(currentDirInfo->FileName, fileName, currentDirInfo->FileNameLength>>1)))
		if (IS_MY_HIDE_OBJECT(currentDirInfo->FileName, 
							  currentDirInfo->FileNameLength,
							  currentDirInfo->FileAttributes))
		{
			//Now We Will Test The FileName
			//KdPrint(("%08x Hided File:%ws[%d]\n", currentDirInfo->FileAttributes, currentDirInfo->FileName, currentDirInfo->FileNameLength));
			if (0 == offset)
			{
				//KdPrint(("l[%d][%d][%d][%d]\n", newLenth, *lpBufLenth, position, newLenth-(*lpBufLenth - position)));
				//Reset Last DirInfo NextEntryOffset To Zero!!!
				if (lastDirInfo)
				{
					lastDirInfo->NextEntryOffset = 0;
					newLenth -= *lpBufLenth - position;
				}
				else
				{
					currentDirInfo->NextEntryOffset = 0;
					*lpBufLenth = 0;
					return TRUE;
				}
			}
			else
			{
				//KdPrint(("n[%d][%d][%d]\n", newLenth, *lpBufLenth, position));
				RtlMoveMemory(currentDirInfo, (PUCHAR)currentDirInfo + offset, *lpBufLenth - position - offset);
				newLenth -= offset;
				position += offset;
			}
		}
		else
		{
			//KdPrint(("%08x Directory:%ws\n", currentDirInfo->FileAttributes, currentDirInfo->FileName));
			//Move Next
			position += offset;
			lastDirInfo = currentDirInfo;
			currentDirInfo = (PFILE_BOTH_DIR_INFORMATION)((PUCHAR)currentDirInfo + offset);
		}
	} while (0 != offset);
	*lpBufLenth = newLenth;
	return TRUE;
}


BOOLEAN
IS_MY_HIDE_OBJECT(const WCHAR *Name, ULONG NameLenth, ULONG Flag)
{
	PLIST_ENTRY headListEntry = &g_HideObjHead;
	PLIST_ENTRY tmpListEntry = headListEntry;
	PHIDE_FILE tmpHideFile = NULL;
	ULONG ObjFlag = (FILE_ATTRIBUTE_DIRECTORY & Flag)?CMD_DIRECTORY_FILTER:CMD_FILE_FILTER;

	if (IsListEmpty(&g_HideObjHead))
	{
		return FALSE;
	}
	while (tmpListEntry->Flink != headListEntry)
	{
		tmpListEntry = tmpListEntry->Flink;
		tmpHideFile = (PHIDE_FILE)CONTAINING_RECORD(tmpListEntry, HIDE_FILE, linkfield);
		if ((ObjFlag == tmpHideFile->Flag) &&
			(0 == wcsncmp(Name, tmpHideFile->Name, NameLenth>>1)))
		{
			KdPrint(("Find Obj\n"));
			return TRUE;
		}
	}
	return FALSE;
}

NTSTATUS
DirControlCompletion(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp, IN PVOID Context)
{
	//if (Irp->PendingReturned) IoMarkIrpPending(Irp);
	KeSetEvent((PKEVENT)Context, IO_NO_INCREMENT, FALSE);
	return STATUS_MORE_PROCESSING_REQUIRED;	//注：必须返回这个值
}
BOOLEAN
IS_MY_DEVICE_OBJECT(PDEVICE_OBJECT DeviceObject)
{
	return DeviceObject->DriverObject == deviceObject->DriverObject;
}
VOID
AddHideObject(PWCHAR Name, ULONG Flag)
{
	//添加一个隐藏
	PHIDE_FILE newHideObj;
	newHideObj = ExAllocatePool(PagedPool, sizeof(HIDE_FILE));
	newHideObj->Flag = Flag;
	wcscpy(newHideObj->Name, Name);
	InsertTailList(&g_HideObjHead, &newHideObj->linkfield);
	KdPrint(("Add Hide Obj:%ws", Name));
}
