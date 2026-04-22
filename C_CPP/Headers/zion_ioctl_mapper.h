/*
 *      name: ZION server - MACROS for IOCTR DISPATCH mapping [header]
 *      type: Kernel Module
 *
 *      Org Author: Shikai Chen
 *      Version : 0.1
 */


#pragma once

#ifndef ZION_SERVER_MODE
#error This header file cannot be included without ZION_SERVER_MODE
#endif




#define BEGIN_IOCTL_MAPPING(func_name)      \
     NTSTATUS hook_##func_name        \
           (IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp) {                        \
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
		if (deviceObject == DeviceObject)			\
{													\
        switch (ioControlCode){                                             

#define DEFINE_IOCTL_MAPPING(func_name)                                          \
        NTSTATUS hook_##func_name        \
           (IN PDEVICE_OBJECT DeviceObject,IN PIRP Irp)
       
                  

#define END_IOCTL_MAPPING() \
            default:                                    \
                status = STATUS_INVALID_PARAMETER;      \
        }                                               \
        Irp->IoStatus.Status = status;                  \
	    Irp->IoStatus.Information = outputBufferLength; \
        IoCompleteRequest( Irp, IO_NO_INCREMENT );      \
        return status;                                  \
    }

#define IOCTL_MAPPING_START(_CMD)                   \
            case _CMD:                              \
               
