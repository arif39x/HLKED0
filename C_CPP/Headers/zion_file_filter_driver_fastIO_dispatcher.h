/*
 *      name: ZION system File filter Fast IO Dispatcher(return False)
 *      Org Author: Liang Chen
 *      Version : 0.2
 */
#include <ntddk.h>
BOOLEAN
ZION_FastIoCheckIfPossible (
	IN PFILE_OBJECT FileObject,
	IN PLARGE_INTEGER FileOffset,
	IN ULONG Length,
	IN BOOLEAN Wait,
	IN ULONG LockKey,
	IN BOOLEAN CheckForReadOperation,
	OUT PIO_STATUS_BLOCK IoStatus,
	IN PDEVICE_OBJECT DeviceObject)
{
	return FALSE;
}
BOOLEAN
ZION_FastIoRead(
	IN struct _FILE_OBJECT *FileObject,
	IN PLARGE_INTEGER FileOffset,
	IN ULONG Length,
	IN BOOLEAN Wait,
	IN ULONG LockKey,
	OUT PVOID Buffer,
	OUT PIO_STATUS_BLOCK IoStatus,
	IN struct _DEVICE_OBJECT *DeviceObject)
{
	return FALSE;
}

BOOLEAN
ZION_FastIoWrite (
				   IN struct _FILE_OBJECT *FileObject,
				   IN PLARGE_INTEGER FileOffset,
				   IN ULONG Length,
				   IN BOOLEAN Wait,
				   IN ULONG LockKey,
				   IN PVOID Buffer,
				   OUT PIO_STATUS_BLOCK IoStatus,
				   IN struct _DEVICE_OBJECT *DeviceObject
				   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoQueryBasicInfo (
							  IN struct _FILE_OBJECT *FileObject,
							  IN BOOLEAN Wait,
							  OUT PFILE_BASIC_INFORMATION Buffer,
							  OUT PIO_STATUS_BLOCK IoStatus,
							  IN struct _DEVICE_OBJECT *DeviceObject
							  )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoQueryStandardInfo (
								 IN struct _FILE_OBJECT *FileObject,
								 IN BOOLEAN Wait,
								 OUT PFILE_STANDARD_INFORMATION Buffer,
								 OUT PIO_STATUS_BLOCK IoStatus,
								 IN struct _DEVICE_OBJECT *DeviceObject
								 )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoLock (
				  IN struct _FILE_OBJECT *FileObject,
				  IN PLARGE_INTEGER FileOffset,
				  IN PLARGE_INTEGER Length,
				  PEPROCESS ProcessId,
				  ULONG Key,
				  BOOLEAN FailImmediately,
				  BOOLEAN ExclusiveLock,
				  OUT PIO_STATUS_BLOCK IoStatus,
				  IN struct _DEVICE_OBJECT *DeviceObject
				  )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoUnlockSingle (
						   IN struct _FILE_OBJECT *FileObject,
						   IN PLARGE_INTEGER FileOffset,
						   IN PLARGE_INTEGER Length,
						   PEPROCESS ProcessId,
						   ULONG Key,
						   OUT PIO_STATUS_BLOCK IoStatus,
						   IN struct _DEVICE_OBJECT *DeviceObject
						   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoUnlockAll (
						   IN struct _FILE_OBJECT *FileObject,
						   PEPROCESS ProcessId,
						   OUT PIO_STATUS_BLOCK IoStatus,
						   IN struct _DEVICE_OBJECT *DeviceObject
				   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoUnlockAllByKey (
							   IN struct _FILE_OBJECT *FileObject,
							   PVOID ProcessId,
							   ULONG Key,
							   OUT PIO_STATUS_BLOCK IoStatus,
							   IN struct _DEVICE_OBJECT *DeviceObject
							   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoDeviceControl (
							IN struct _FILE_OBJECT *FileObject,
							IN BOOLEAN Wait,
							IN PVOID InputBuffer OPTIONAL,
							IN ULONG InputBufferLength,
							OUT PVOID OutputBuffer OPTIONAL,
							IN ULONG OutputBufferLength,
							IN ULONG IoControlCode,
							OUT PIO_STATUS_BLOCK IoStatus,
							IN struct _DEVICE_OBJECT *DeviceObject
							)
{
	return FALSE;
}

VOID
ZION_FastIoAcquireFile (
						  IN struct _FILE_OBJECT *FileObject
						  )
{
}

VOID
ZION_FastIoReleaseFile (
						  IN struct _FILE_OBJECT *FileObject
						  )
{
}

VOID
ZION_FastIoDetachDevice (
						   IN struct _DEVICE_OBJECT *SourceDevice,
						   IN struct _DEVICE_OBJECT *TargetDevice
						   )
{
}

BOOLEAN
ZION_FastIoQueryNetworkOpenInfo (
									 IN struct _FILE_OBJECT *FileObject,
									 IN BOOLEAN Wait,
									 OUT struct _FILE_NETWORK_OPEN_INFORMATION *Buffer,
									 OUT struct _IO_STATUS_BLOCK *IoStatus,
									 IN struct _DEVICE_OBJECT *DeviceObject
									 )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoMdlRead (
					  IN struct _FILE_OBJECT *FileObject,
					  IN PLARGE_INTEGER FileOffset,
					  IN ULONG Length,
					  IN ULONG LockKey,
					  OUT PMDL *MdlChain,
					  OUT PIO_STATUS_BLOCK IoStatus,
					  IN struct _DEVICE_OBJECT *DeviceObject
					  )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoMdlReadComplete (
							   IN struct _FILE_OBJECT *FileObject,
							   IN PMDL MdlChain,
							   IN struct _DEVICE_OBJECT *DeviceObject
							   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoPrepareMdlWrite (
							   IN struct _FILE_OBJECT *FileObject,
							   IN PLARGE_INTEGER FileOffset,
							   IN ULONG Length,
							   IN ULONG LockKey,
							   OUT PMDL *MdlChain,
							   OUT PIO_STATUS_BLOCK IoStatus,
							   IN struct _DEVICE_OBJECT *DeviceObject
							   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoMdlWriteComplete (
								IN struct _FILE_OBJECT *FileObject,
								IN PLARGE_INTEGER FileOffset,
								IN PMDL MdlChain,
								IN struct _DEVICE_OBJECT *DeviceObject
								)
{
	return FALSE;
}

NTSTATUS
ZION_FastIoAcquireForModeWrite (
								   IN struct _FILE_OBJECT *FileObject,
								   IN PLARGE_INTEGER EndingOffset,
								   OUT struct _ERESOURCE **ResourceToRelease,
								   IN struct _DEVICE_OBJECT *DeviceObject
								   )
{
	return 0;
}

NTSTATUS
ZION_FastIoReleaseForModWrite (
								   IN struct _FILE_OBJECT *FileObject,
								   IN struct _ERESOURCE *ResourceToRelease,
								   IN struct _DEVICE_OBJECT *DeviceObject
								   )
{
	return 0;
}

NTSTATUS
ZION_FastIoAcquireForCcFlush (
								 IN struct _FILE_OBJECT *FileObject,
								 IN struct _DEVICE_OBJECT *DeviceObject
								 )
{
	return 0;
}

NTSTATUS
ZION_FastIoReleaseForCcFlush (
								 IN struct _FILE_OBJECT *FileObject,
								 IN struct _DEVICE_OBJECT *DeviceObject
								 )
{
	return 0;
}

BOOLEAN
ZION_FastIoReadCompressed (
							 IN struct _FILE_OBJECT *FileObject,
							 IN PLARGE_INTEGER FileOffset,
							 IN ULONG Length,
							 IN ULONG LockKey,
							 OUT PVOID Buffer,
							 OUT PMDL *MdlChain,
							 OUT PIO_STATUS_BLOCK IoStatus,
							 OUT struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
							 IN ULONG CompressedDataInfoLength,
							 IN struct _DEVICE_OBJECT *DeviceObject
							 )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoWriteCompressed (
							  IN struct _FILE_OBJECT *FileObject,
							  IN PLARGE_INTEGER FileOffset,
							  IN ULONG Length,
							  IN ULONG LockKey,
							  IN PVOID Buffer,
							  OUT PMDL *MdlChain,
							  OUT PIO_STATUS_BLOCK IoStatus,
							  IN struct _COMPRESSED_DATA_INFO *CompressedDataInfo,
							  IN ULONG CompressedDataInfoLength,
							  IN struct _DEVICE_OBJECT *DeviceObject
							  )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoMdlReadCompleteCompressed (
										  IN struct _FILE_OBJECT *FileObject,
										  IN PMDL MdlChain,
										  IN struct _DEVICE_OBJECT *DeviceObject
										  )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoMdlWriteCompleteCompressed (
										   IN struct _FILE_OBJECT *FileObject,
										   IN PLARGE_INTEGER FileOffset,
										   IN PMDL MdlChain,
										   IN struct _DEVICE_OBJECT *DeviceObject
										   )
{
	return FALSE;
}

BOOLEAN
ZION_FastIoQueryOpen(
						IN struct _IRP *Irp,
						OUT PFILE_NETWORK_OPEN_INFORMATION NetworkInformation,
						IN struct _DEVICE_OBJECT *DeviceObject
						)
{
	return FALSE;
}

BOOLEAN
InitFastIo(IN PDRIVER_OBJECT DriverObject)
{
	PFAST_IO_DISPATCH fastIoDispatch;
	fastIoDispatch = ExAllocatePool(NonPagedPool, sizeof(FAST_IO_DISPATCH));
	if (!fastIoDispatch) 
	{
		return FALSE;
	}
	RtlZeroMemory(fastIoDispatch, sizeof(FAST_IO_DISPATCH));
	fastIoDispatch->SizeOfFastIoDispatch = sizeof(FAST_IO_DISPATCH);
	fastIoDispatch->FastIoCheckIfPossible = ZION_FastIoCheckIfPossible;
	fastIoDispatch->FastIoRead = ZION_FastIoRead;
	fastIoDispatch->FastIoWrite = ZION_FastIoWrite;
	fastIoDispatch->FastIoQueryBasicInfo = ZION_FastIoQueryBasicInfo;
	fastIoDispatch->FastIoQueryStandardInfo = ZION_FastIoQueryStandardInfo;
	fastIoDispatch->FastIoLock = ZION_FastIoLock;
	fastIoDispatch->FastIoUnlockSingle = ZION_FastIoUnlockSingle;
	fastIoDispatch->FastIoUnlockAll = ZION_FastIoUnlockAll;
	fastIoDispatch->FastIoUnlockAllByKey = ZION_FastIoUnlockAllByKey;
	fastIoDispatch->FastIoDeviceControl = ZION_FastIoDeviceControl;
	fastIoDispatch->FastIoDetachDevice = ZION_FastIoDetachDevice;
	fastIoDispatch->FastIoQueryNetworkOpenInfo = ZION_FastIoQueryNetworkOpenInfo;
	fastIoDispatch->MdlRead = ZION_FastIoMdlRead;
	fastIoDispatch->MdlReadComplete = ZION_FastIoMdlReadComplete;
	fastIoDispatch->PrepareMdlWrite = ZION_FastIoPrepareMdlWrite;
	fastIoDispatch->MdlWriteComplete = ZION_FastIoMdlWriteComplete;
	fastIoDispatch->FastIoReadCompressed = ZION_FastIoReadCompressed;
	fastIoDispatch->FastIoWriteCompressed = ZION_FastIoWriteCompressed;
	fastIoDispatch->MdlReadCompleteCompressed = ZION_FastIoMdlReadCompleteCompressed;
	fastIoDispatch->MdlWriteCompleteCompressed = ZION_FastIoMdlWriteCompleteCompressed;
	fastIoDispatch->FastIoQueryOpen = ZION_FastIoQueryOpen;

	DriverObject->FastIoDispatch = fastIoDispatch;
	return TRUE;
}

VOID
DestoryFastIo(IN PDRIVER_OBJECT DriverObject)
{
	PFAST_IO_DISPATCH fastIoDispatch = DriverObject->FastIoDispatch;
	//PAGED_CODE();
	if (fastIoDispatch)
	{
		KdPrint(("FreeFastIo Pool\n"));
		DriverObject->FastIoDispatch = NULL;
		ExFreePool(fastIoDispatch);
	}
}