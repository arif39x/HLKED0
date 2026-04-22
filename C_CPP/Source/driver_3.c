#include <ntddk.h>
#define IOCTL_UNHOOK_SST    0x22C004
 
typedef struct SERVICE_DESCRIPTOR_ENTRY
{
    unsigned int    *ServiceTableBase;
    unsigned int    *ServiceCounterTableBase;
    unsigned int    NumberOfServices;
    unsigned char   *ParamTableBase;
} SERVICE_DESCRIPTOR_ENTRY,
*PSERVICE_DESCRIPTOR_ENTRY;
 
__declspec(dllimport) SERVICE_DESCRIPTOR_ENTRY KeServiceDescriptorTable;
 
#define SYSTEM_SERVICE(p) KeServiceDescriptorTable.ServiceTableBase[*(PULONG)((ULONG)p+1)]
 
#define NTCALL(p) KeServiceDescriptorTable.ServiceTableBase[p]
 
typedef struct _REQUEST {
    
    ULONG   s_cnt;
    ULONG   sst[ 400 ];
 
} REQUEST, *PREQUEST;
 
/*  ================================
    == MemClose
    ================================    */
PVOID
MemOpen( PMDL *Mdl, PVOID Addr, ULONG Length )
{
    PVOID pRet;
 
    if ( ( *Mdl = ( PMDL )IoAllocateMdl( Addr, Length, FALSE, FALSE, NULL ) ) == NULL )
        return NULL;
 
    __try
    {
        MmProbeAndLockPages( *Mdl, KernelMode, IoWriteAccess );
    }
    __except( EXCEPTION_EXECUTE_HANDLER )
    {
        IoFreeMdl( *Mdl );
        return NULL;
    }
 
    if ( ( pRet = MmMapLockedPagesSpecifyCache( *Mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority ) ) == NULL )
    {
        MmUnlockPages( *Mdl );
        IoFreeMdl( *Mdl );
        return NULL;
    }
 
    return pRet;
}
/*  ================================
    == MemClose
    ================================    */
void MemClose( PMDL Mdl, PVOID Addr )
{
    MmUnmapLockedPages( Addr, Mdl );
    MmUnlockPages( Mdl );
    IoFreeMdl( Mdl );
}
/*  ================================
    == UnHooSST
    ================================    */
void UnHooSST( ULONG *sst, ULONG num )
{
    ULONG i;
    PULONG ServiceTable;
    PMDL Mdl;
 
    if ( ( ServiceTable = ( PULONG )MemOpen(
        &Mdl, 
        KeServiceDescriptorTable.ServiceTableBase, 
        KeServiceDescriptorTable.NumberOfServices * sizeof( ULONG ) ) ) == NULL )
        return;
 
    __asm cli
 
    for ( i = 0; i < num; i++ )
        if ( NTCALL( i ) != sst[ i ] && sst[ i ] != 0 )
            InterlockedExchange( ( PVOID )&NTCALL( i ), sst[ i ] );
 
    __asm sti
 
    MemClose( Mdl, ServiceTable );
}
/*  ================================
    == DeviceControlRoutine
    ================================    */
NTSTATUS
DeviceControlRoutine( PDEVICE_OBJECT DeviceObject, PIRP Irp )
{
    PIO_STACK_LOCATION stack;
    NTSTATUS ns = STATUS_SUCCESS;
    ULONG code;
    PREQUEST buf_req;
 
    Irp -> IoStatus.Status = ns;
    Irp -> IoStatus.Information = 0;
 
    stack = IoGetCurrentIrpStackLocation( Irp );
 
    if (stack->MajorFunction == IRP_MJ_DEVICE_CONTROL) {
 
        code = stack->Parameters.DeviceIoControl.IoControlCode;
        buf_req = ( PREQUEST )Irp -> AssociatedIrp.SystemBuffer;
        Irp -> IoStatus.Information = sizeof( REQUEST );
 
        switch ( code )
        {
 
            case IOCTL_UNHOOK_SST:
                UnHooSST( buf_req -> sst, buf_req -> s_cnt );
                break;
 
            default:
                ns = STATUS_INVALID_DEVICE_REQUEST;
                Irp -> IoStatus.Information = 0;
                break;
        }
    }
 
    if ( ns != STATUS_PENDING ) {
        IoCompleteRequest( Irp, IO_NO_INCREMENT );
        Irp->IoStatus.Status = ns;
    }
 
    return ns;
}
 
/*  ================================
    == DriverEntry
    ================================    */
NTSTATUS 
DriverEntry( IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING RegistryPath ) {
 
    NTSTATUS ns = STATUS_SUCCESS;
    UNICODE_STRING dos_dev_name, dev_name;
    PDEVICE_OBJECT dev_obj;
 
    RtlInitUnicodeString( &dev_name, L"\\Device\\tw32servie" );
    RtlInitUnicodeString( &dos_dev_name, L"\\DosDevices\\tw32servie" );
 
    ns = IoCreateDevice(DriverObject, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);
 
    if ( NT_SUCCESS( ns ) ) {
 
        DriverObject->MajorFunction[ IRP_MJ_CREATE ]            = DeviceControlRoutine;
        DriverObject->MajorFunction[ IRP_MJ_CLOSE ]             = DeviceControlRoutine;
        DriverObject->MajorFunction[ IRP_MJ_DEVICE_CONTROL ]    = DeviceControlRoutine;
 
        dev_obj->Flags |= DO_BUFFERED_IO;
 
        ns = IoCreateSymbolicLink(&dos_dev_name, &dev_name);
 
        if ( !NT_SUCCESS( ns ) ) {
            IoDeleteDevice( DriverObject -> DeviceObject );
            return ns;
        }
    }
    else
        return ns;
 
 
    return STATUS_SUCCESS;
}