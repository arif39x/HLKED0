#ifndef _ONION_
#define _ONION
#include <intrin.h>
#include "oniondef.h"

#define API_FXN_COUNT 4
#define PROLOGUE_COUNT 3

#define API_FUNCTIONS \
	_ExitProcess, _OpenThread, _CloseHandle, _VirtualAlloc

#define PROLOGUE_CODE \
	0x55, \
	0x8B, 0xEC \

#ifndef _M_IX86
	#ifndef _M_X64
		#error "Unsupported platform."
	#endif
#endif

#ifdef __cplusplus
	extern "C"
	{
#endif

#ifdef _M_IX86
	void __adjustESP( int adjustment );
	void __moveEAX( int adjustment );
#elif _M_X64
	void __adjustRSP( int adjustment );
	void __moveRAX( int adjustment );
#endif

NTSTATUS KiFastSystemCall( ... );

#ifdef __cplusplus
	}
#endif

extern NTSTATUS _Return;
extern DWORD _Version;
extern BOOL _RunOnce;

extern DWORD syscallTable[201][6];

void SetServices( );
NTSTATUS FastSystemCall( DWORD _Request[5], ... );

HANDLE WINAPI _OpenThread( DWORD DesiredAccess, BOOL InheritHandle, DWORD ThreadId );

LPVOID WINAPI _VirtualAllocEx( HANDLE ProcessHandle, LPVOID Address, SIZE_T Size, DWORD AllocationType, DWORD Protect );
LPVOID WINAPI _VirtualAlloc( LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect );

SIZE_T WINAPI _VirtualQuery( void *Address, PMEMORY_BASIC_INFORMATION Buffer, SIZE_T Length );

HANDLE WINAPI _GetCurrentProcess( );
DWORD WINAPI _GetCurrentProcessId( );
HANDLE WINAPI _GetCurrentThread( );
BOOL WINAPI _GetThreadContext( HANDLE ThreadHandle, CONTEXT *Context );

BOOL WINAPI _CloseHandle( HANDLE Object );
VOID WINAPI _ExitProcess( UINT ExitCode );

#define _NtAcceptConnectPort( PortHandle, PortIdentifier, Message, Accept, ServerView, ClientView ) \
		FastSystemCall( syscallTable[0], 0, PortHandle, PortIdentifier, Message, Accept, ServerView, ClientView )

#define _NtAccessCheck( SecurityDescriptor, TokenHandle, DesiredAccess, GenericMapping, PrivilegeSet, PrivilegeSetLength, GrantedAccess, AccessStatus ) \
		FastSystemCall( syscallTable[1], 0, SecurityDescriptor, TokenHandle, DesiredAccess, GenericMapping, PrivilegeSet, PrivilegeSetLength, GrantedAccess, AccessStatus )

#define _NtAccessCheckAndAuditAlarm( SubsystemName, HandleId, ObjectTypeName, ObjectName, SecurityDescriptor, DesiredAccess, GenericMapping, ObjectCreation, GrantedAccess, AccessStatus, GenerateOnClose ) \
		FastSystemCall( syscallTable[2], 0, SubsystemName, HandleId, ObjectTypeName, ObjectName, SecurityDescriptor, DesiredAccess, GenericMapping, ObjectCreation, GrantedAccess, AccessStatus, GenerateOnClose )

#define _NtAddAtom( String, StringLength, Atom ) \
		FastSystemCall( syscallTable[3], 0, String, StringLength, Atom )

#define _NtAdjustGroupsToken( TokenHandle, ResetToDefault, NewState, BufferLength, PreviousState, ReturnLength ) \
		FastSystemCall( syscallTable[4], 0, TokenHandle, ResetToDefault, NewState, BufferLength, PreviousState, ReturnLength )

#define _NtAdjustPrivilegesToken( TokenHandle, DisableAllPrivileges, NewState, BufferLength, PreviousState, ReturnLength ) \
		FastSystemCall( syscallTable[5], 0, TokenHandle, DisableAllPrivileges, NewState, BufferLength, PreviousState, ReturnLength )

#define _NtAlertResumeThread( ThreadHandle, PreviousSuspendCount ) \
		FastSystemCall( syscallTable[6], 0, ThreadHandle, PreviousSuspendCount )

#define _NtAlertThread( ThreadHandle ) \
		FastSystemCall( syscallTable[7], 0, ThreadHandle )

#define _NtAllocateLocallyUniqueId( Luid ) \
		FastSystemCall( syscallTable[8], 0, Luid )

#define _NtAllocateUuids( UuidLastTimeAllocated, UuidDeltaTime, UuidSequenceNumber, UuidSeed ) \
		FastSystemCall( syscallTable[9], 0, UuidLastTimeAllocated, UuidDeltaTime, UuidSequenceNumber, UuidSeed )

#define _NtAllocateVirtualMemory( ProcessHandle, BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect ) \
		FastSystemCall( syscallTable[10], 0, ProcessHandle, BaseAddress, ZeroBits, AllocationSize, AllocationType, Protect )

#define _NtCallbackReturn( Result, ResultLength, Status ) \
		FastSystemCall( syscallTable[11], 0, Result, ResultLength, Status )

#define _NtCancelIoFile( FileHandle, IoStatusBlock ) \
		FastSystemCall( syscallTable[12], 0, FileHandle, IoStatusBlock )

#define _NtCancelTimer( TimerHandle, PreviousState ) \
		FastSystemCall( syscallTable[13], 0, TimerHandle, PreviousState )

#define _NtClearEvent( EventHandle ) \
		FastSystemCall( syscallTable[14], 0, EventHandle )

#define _NtClose( Handle ) \
		FastSystemCall( syscallTable[15], 0, Handle )

#define _NtCloseObjectAuditAlarm( SubsystemName, Id, GenerateOnClose ) \
		FastSystemCall( syscallTable[16], 0, SubsystemName, Id, GenerateOnClose )

#define _NtCompleteConnectPort( PortHandle ) \
		FastSystemCall( syscallTable[17], 0, PortHandle )

#define _NtConnectPort( PortHandle, PortName, SecurityQos, ClientView, ServerView, MaxMessageLength, ConnectInformation, ConnectInformationLength ) \
		FastSystemCall( syscallTable[18], 0, PortHandle, PortName, SecurityQos, ClientView, ServerView, MaxMessageLength, ConnectInformation, ConnectInformationLength )

#define _NtContinue( Context, TestAlert ) \
		FastSystemCall( syscallTable[19], 0, Context, TestAlert )

#define _NtCreateDirectoryObject( DirectoryHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[20], 0, DirectoryHandle, DesiredAccess, ObjectAttributes )

#define _NtCreateEvent( EventHandle, DesiredAccess, ObjectAttributes, EventType, InitialState ) \
		FastSystemCall( syscallTable[21], 0, EventHandle, DesiredAccess, ObjectAttributes, EventType, InitialState )

#define _NtCreateEventPair( EventPairHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[22], 0, EventPairHandle, DesiredAccess, ObjectAttributes )

#define _NtCreateFile( FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength ) \
		FastSystemCall( syscallTable[23], 0, FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength )

#define _NtCreateIoCompletion( IoCompletionHandle, DesiredAccess, ObjectAttributes, NumberOfConcurrentThreads ) \
		FastSystemCall( syscallTable[24], 0, IoCompletionHandle, DesiredAccess, ObjectAttributes, NumberOfConcurrentThreads )

#define _NtCreateKey( KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions, Disposition ) \
		FastSystemCall( syscallTable[25], 0, KeyHandle, DesiredAccess, ObjectAttributes, TitleIndex, Class, CreateOptions, Disposition )

#define _NtCreateMailslotFile( FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, CreateOptions, InBufferSize, MaxMessageSize, ReadTime ) \
		FastSystemCall( syscallTable[26], 0, FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, CreateOptions, InBufferSize, MaxMessageSize, ReadTime )

#define _NtCreateMutant( MutantHandle, DesiredAccess, ObjectAttributes, InitialOwner ) \
		FastSystemCall( syscallTable[27], 0, MutantHandle, DesiredAccess, ObjectAttributes, InitialOwner )

#define _NtCreateNamedPipeFile( FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, CreateDisposition, CreateOptions, TypeMessage, ReadmodeMessage, Nonblocking, MaxInstances, InBufferSize, OutBufferSize, DefaultTime ) \
		FastSystemCall( syscallTable[28], 0, FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, CreateDisposition, CreateOptions, TypeMessage, ReadmodeMessage, Nonblocking, MaxInstances, InBufferSize, OutBufferSize, DefaultTime )

#define _NtCreatePagingFile( FileName, InitialSize, MaximumSize, Priority ) \
		FastSystemCall( syscallTable[29, 0, FileName, InitialSize, MaximumSize, Priority )

#define _NtCreatePort( PortHandle, ObjectAttributes, MaxConnectionInfoLength, MaxMessageLength, MaxPoolUsage ) \
		FastSystemCall( syscallTable[30], 0, PortHandle, ObjectAttributes, MaxConnectionInfoLength, MaxMessageLength, MaxPoolUsage )

#define _NtCreateProcess( ProcessHandle, DesiredAccess, ObjectAttributes, InheritFromProcessHandle, InheritHandles, SectionHandle, DebugPort, ExceptionPort ) \
		FastSystemCall( syscallTable[31], 0, ProcessHandle, DesiredAccess, ObjectAttributes, InheritFromProcessHandle, InheritHandles, SectionHandle, DebugPort, ExceptionPort )

#define _NtCreateProfile( ProfileHandle, ProcessHandle, Base, Size, BucketShift, Buffer, BufferLength, Source, ProcessorMask ) \
		FastSystemCall( syscallTable[32], 0, ProfileHandle, ProcessHandle, Base, Size, BucketShift, Buffer, BufferLength, Source, ProcessorMask )

#define _NtCreateSection( SectionHandle, DesiredAccess, ObjectAttributes, SectionSize, Protect, Attributes, FileHandle ) \
		FastSystemCall( syscallTable[33], 0, SectionHandle, DesiredAccess, ObjectAttributes, SectionSize, Protect, Attributes, FileHandle )

#define _NtCreateSemaphore( SemaphoreHandle, DesiredAccess, ObjectAttributes, InitialCount, MaximumCount ) \
		FastSystemCall( syscallTable[34], 0, SemaphoreHandle, DesiredAccess, ObjectAttributes, InitialCount, MaximumCount )

#define _NtCreateSymbolicLinkObject( SymbolicLinkHandle, DesiredAccess, ObjectAttributes, TargetName ) \
		FastSystemCall( syscallTable[35], 0, SymbolicLinkHandle, DesiredAccess, ObjectAttributes, TargetName )

#define _NtCreateThread( ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientId, ThreadContext, UserStack, CreateSuspended ) \
		FastSystemCall( syscallTable[36], 0, ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientId, ThreadContext, UserStack, CreateSuspended )

#define _NtCreateTimer( TimerHandle, DesiredAccess, ObjectAttributes, TimerType ) \
		FastSystemCall( syscallTable[37], 0, TimerHandle, DesiredAccess, ObjectAttributes, TimerType )

#define _NtCreateToken( TokenHandle, DesiredAccess, ObjectAttributes, Type, AuthenticationId, ExpirationTime, User, Groups, Privileges, Owner, PrimaryGroup, DefaultDacl, Source ) \
		FastSystemCall( syscallTable[38], 0, TokenHandle, DesiredAccess, ObjectAttributes, Type, AuthenticationId, ExpirationTime, User, Groups, Privileges, Owner, PrimaryGroup, DefaultDacl, Source )

#define _NtDelayExecution( Alertable, Interval ) \
		FastSystemCall( syscallTable[39], 0, Alertable, Interval )

#define _NtDeleteAtom( Atom ) \
		FastSystemCall( syscallTable[40], 0, Atom )

#define _NtDeleteFile( ObjectAttributes ) \
		FastSystemCall( syscallTable[41], 0, ObjectAttributes )

#define _NtDeleteKey( KeyHandle ) \
		FastSystemCall( syscallTable[42], 0, KeyHandle )

#define _NtDeleteObjectAuditAlarm( SubsystemName, Id, GenerateOnClose ) \
		FastSystemCall( syscallTable[43], 0, SubsystemName, Id, GenerateOnClose )

#define _NtDeleteValueKey( KeyHandle, ValueName ) \
		FastSystemCall( syscallTable[44], 0, KeyHandle, ValueName )

#define _NtDeviceIoControlFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength ) \
		FastSystemCall( syscallTable[45], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength )

#define _NtDisplayString( String ) \
		FastSystemCall( syscallTable[46], 0, String )

#define _NtDuplicateObject( SourceProcessHandle, SourceHandle, TargetProcessHandle, TargetHandle, DesiredAccess, Attributes, Options ) \
		FastSystemCall( syscallTable[47], 0, SourceProcessHandle, SourceHandle, TargetProcessHandle, TargetHandle, DesiredAccess, Attributes, Options )

#define _NtDuplicateToken( ExistingTokenHandle, DesiredAccess, ObjectAttributes, EffectiveOnly, TokenType, NewTokenHandle ) \
		FastSystemCall( syscallTable[48], 0, ExistingTokenHandle, DesiredAccess, ObjectAttributes, EffectiveOnly, TokenType, NewTokenHandle )

#define _NtEnumerateKey( KeyHandle, Index, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[49], 0, KeyHandle, Index, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength )

#define _NtEnumerateValueKey( KeyHandle, Index, KeyValueInformationClass, KeyValueInformation, KeyValueInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[50], 0, KeyHandle, Index, KeyValueInformationClass, KeyValueInformation, KeyValueInformationLength, ResultLength )

#define _NtExtendSection( SectionHandle, SectionSize ) \
		FastSystemCall( syscallTable[51], 0, SectionHandle, SectionSize )

#define _NtFindAtom( String, StringLength, Atom ) \
		FastSystemCall( syscallTable[52], 0, String, StringLength, Atom )

#define _NtFlushBuffersFile( FileHandle, IoStatusBlock ) \
		FastSystemCall( syscallTable[53], 0, FileHandle, IoStatusBlock )

#define _NtFlushInstructionCache( ProcessHandle, BaseAddress, FlushSize ) \
		FastSystemCall( syscallTable[54], 0, ProcessHandle, BaseAddress, FlushSize )

#define _NtFlushKey( KeyHandle ) \
		FastSystemCall( syscallTable[55], 0, KeyHandle )

#define _NtFlushVirtualMemory( ProcessHandle, BaseAddress, FlushSize, IoStatusBlock ) \
		FastSystemCall( syscallTable[56], 0, ProcessHandle, BaseAddress, FlushSize, IoStatusBlock )

#define _NtFlushWriteBuffer( ) \
		FastSystemCall( syscallTable[57], 0 )

#define _NtFreeVirtualMemory( ProcessHandle, BaseAddress, FreeSize, FreeType ) \
		FastSystemCall( syscallTable[58], 0, ProcessHandle, BaseAddress, FreeSize, FreeType )

#define _NtFsControlFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FsControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength ) \
		FastSystemCall( syscallTable[59], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FsControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength )

#define _NtGetContextThread( ThreadHandle, Context ) \
		FastSystemCall( syscallTable[60], 0, ThreadHandle, Context )

#define _NtGetPlugPlayEvent( Reserved1, Reserved2, Buffer, BufferLength ) \
		FastSystemCall( syscallTable[61], 0, Reserved1, Reserved2, Buffer, BufferLength )

#define _NtImpersonateClientOfPort( PortHandle, Message ) \
		FastSystemCall( syscallTable[62], 0, PortHandle, Message )

#define _NtImpersonateThread( ThreadHandle, TargetThreadHandle, SecurityQos ) \
		FastSystemCall( syscallTable[63], 0, ThreadHandle, TargetThreadHandle, SecurityQos  )

#define _NtInitializeRegistry( Setup ) \
		FastSystemCall( syscallTable[64], 0, Setup )

#define _NtListenPort( PortHandle, Message ) \
		FastSystemCall( syscallTable[65], 0, PortHandle, Message )

#define _NtLoadDriver( DriverServiceName ) \
		FastSystemCall( syscallTable[66], 0, DriverServiceName )

#define _NtLoadKey( KeyObjectAttributes, FileObjectAttributes ) \
		FastSystemCall( syscallTable[67], 0, KeyObjectAttributes, FileObjectAttributes )

#define _NtLoadKey2( KeyObjectAttributes, FileObjectAttributes, Flags ) \
		FastSystemCall( syscallTable[68], 0, KeyObjectAttributes, FileObjectAttributes, Flags )

#define _NtLockFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, LockOffset, LockLength, Key, FailImmediately, ExclusiveLock ) \
		FastSystemCall( syscallTable[69], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, LockOffset, LockLength, Key, FailImmediately, ExclusiveLock )

#define _NtLockVirtualMemory( ProcessHandle, BaseAddress, LockSize, LockType ) \
		FastSystemCall( syscallTable[70], 0, ProcessHandle, BaseAddress, LockSize, LockType )

#define _NtMakeTemporaryObject( ) \
		FastSystemCall( syscallTable[71], 0 )

#define _NtMapViewOfSection( SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Protect ) \
		FastSystemCall( syscallTable[72], 0, SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Protect )

#define _NtNotifyChangeDirectoryFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, BufferLength, NotifyFilter, WatchSubtree ) \
		FastSystemCall( syscallTable[73], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, BufferLength, NotifyFilter, WatchSubtree )

#define _NtNotifyChangeKey( KeyHandle, EventHandle, ApcRoutine, ApcContext, IoStatusBlock, NotifyFilter, WatchSubtree, Buffer, BufferLength, Asynchronous ) \
		FastSystemCall( syscallTable[74], 0, KeyHandle, EventHandle, ApcRoutine, ApcContext, IoStatusBlock, NotifyFilter, WatchSubtree, Buffer, BufferLength, Asynchronous )

#define _NtOpenDirectoryObject( DirectoryHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[75], 0, DirectoryHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenEvent( EventHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[76], 0, EventHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenEventPair( EventPairHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[77], 0, EventPairHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenFile( FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions ) \
		FastSystemCall( syscallTable[78], 0, FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions )

#define _NtOpenIoCompletion( IoCompletionHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[79], 0, IoCompletionHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenKey( KeyHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[80], 0, KeyHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenMutant( MutantHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[81], 0, MutantHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenObjectAuditAlarm( SubsystemName, HandleId, ObjectTypeName, ObjectName, SecurityDescriptor, TokenHandle, DesiredAccess, GrantedAccess, Privileges, ObjectCreation, AccessGranted, GenerateOnClose ) \
		FastSystemCall( syscallTable[82], 0, SubsystemName, HandleId, ObjectTypeName, ObjectName, SecurityDescriptor, TokenHandle, DesiredAccess, GrantedAccess, Privileges, ObjectCreation, AccessGranted, GenerateOnClose )

#define _NtOpenProcess( ProcessHandle, DesiredAccess, ObjectAttributes, ClientId ) \
		FastSystemCall( syscallTable[83], 0, ProcessHandle, DesiredAccess, ObjectAttributes, ClientId )

#define _NtOpenProcessToken( ProcessHandle, DesiredAccess, TokenHandle ) \
		FastSystemCall( syscallTable[84], 0, ProcessHandle, DesiredAccess, TokenHandle )

#define _NtOpenSection( SectionHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[85], 0, SectionHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenSemaphore( SemaphoreHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[86], 0, SemaphoreHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenSymbolicLinkObject( SymbolicLinkHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[87], 0, SymbolicLinkHandle, DesiredAccess, ObjectAttributes )

#define _NtOpenThread( ThreadHandle, DesiredAccess, ObjectAttributes, ClientId ) \
		FastSystemCall( syscallTable[88], 0, ThreadHandle, DesiredAccess, ObjectAttributes, ClientId )

#define _NtOpenThreadToken( ThreadHandle, DesiredAccess, OpenAsSelf, TokenHandle ) \
		FastSystemCall( syscallTable[89], 0, ThreadHandle, DesiredAccess, OpenAsSelf, TokenHandle )

#define _NtOpenTimer( TimerHandle, DesiredAccess, ObjectAttributes ) \
		FastSystemCall( syscallTable[90], 0, TimerHandle, DesiredAccess, ObjectAttributes )

#define _NtPlugPlayControl( ControlCode, Buffer, BufferLength ) \
		FastSystemCall( syscallTable[91], 0, ControlCode, Buffer, BufferLength )

#define _NtPrivilegeCheck( TokenHandle, RequiredPrivileges, Result ) \
		FastSystemCall( syscallTable[92], 0, TokenHandle, RequiredPrivileges, Result )

#define _NtPrivilegeObjectAuditAlarm( SubsystemName, Id, TokenHandle, DesiredAccess, Privileges, AccessGranted ) \
		FastSystemCall( syscallTable[93], 0, SubsystemName, Id, TokenHandle, DesiredAccess, Privileges, AccessGranted )

#define _NtPrivilegedServiceAuditAlarm( SubsystemName, ServiceName, TokenHandle, Privileges, AccessGranted ) \
		FastSystemCall( syscallTable[94], 0, SubsystemName, ServiceName, TokenHandle, Privileges, AccessGranted )

#define _NtProtectVirtualMemory( ProcessHandle, BaseAddress, ProtectSize, NewProtect, OldProtect ) \
		FastSystemCall( syscallTable[95], 0, ProcessHandle, BaseAddress, ProtectSize, NewProtect, OldProtect )

#define _NtPulseEvent( EventHandle, PreviousState ) \
		FastSystemCall( syscallTable[96], 0, EventHandle, PreviousState )

#define _NtQueryAttributesFile( ObjectAttributes, FileInformation ) \
		FastSystemCall( syscallTable[97], 0, ObjectAttributes, FileInformation )

#define _NtQueryDefaultLocale( ThreadOrSystem, Locale ) \
		FastSystemCall( syscallTable[98], 0, ThreadOrSystem, Locale )

#define _NtQueryDirectoryFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass, ReturnSingleEntry, FileName, RestartScan ) \
		FastSystemCall( syscallTable[99], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass, ReturnSingleEntry, FileName, RestartScan )

#define _NtQueryDirectoryObject( DirectoryHandle, Buffer, BufferLength, ReturnSingleEntry, RestartScan, Context, ReturnLength ) \
		FastSystemCall( syscallTable[100], 0, DirectoryHandle, Buffer, BufferLength, ReturnSingleEntry, RestartScan, Context, ReturnLength )

#define _NtQueryEaFile( FileHandle, IoStatusBlock, Buffer, BufferLength, ReturnSingleEntry, EaList, EaListLength, EaIndex, RestartScan ) \
		FastSystemCall( syscallTable[101], 0, FileHandle, IoStatusBlock, Buffer, BufferLength, ReturnSingleEntry, EaList, EaListLength, EaIndex, RestartScan )

#define _NtQueryEvent( EventHandle, EventInformationClass, EventInformation, EventInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[102], 0, EventHandle, EventInformationClass, EventInformation, EventInformationLength, ResultLength )

#define _NtQueryFullAttributesFile( ObjectAttributes, FileInformation ) \
		FastSystemCall( syscallTable[103], 0, ObjectAttributes, FileInformation )

#define _NtQueryInformationAtom( Atom, AtomInformationClass, AtomInformation, AtomInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[104], 0, Atom, AtomInformationClass, AtomInformation, AtomInformationLength, ReturnLength )

#define _NtQueryInformationFile( FileHandle, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass ) \
		FastSystemCall( syscallTable[105], 0, FileHandle, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass )

#define _NtQueryInformationPort( PortHandle, PortInformationClass, PortInformation, PortInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[106], 0, PortHandle, PortInformationClass, PortInformation, PortInformationLength, ReturnLength )

#define _NtQueryInformationProcess( ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[107], 0, ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength, ReturnLength )

#define _NtQueryInformationThread( ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[108], 0, ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength )

#define _NtQueryInformationToken( TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[109], 0, TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength, ReturnLength )

#define _NtQueryIntervalProfile( Source, Interval ) \
		FastSystemCall( syscallTable[110], 0, Source, Interval )

#define _NtQueryIoCompletion( IoCompletionHandle, IoCompletionInformationClass, IoCompletionInformation, IoCompletionInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[111], 0, IoCompletionHandle, IoCompletionInformationClass, IoCompletionInformation, IoCompletionInformationLength, ResultLength )

#define _NtQueryKey( KeyHandle, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[112], 0, KeyHandle, KeyInformationClass, KeyInformation, KeyInformationLength, ResultLength )

#define _NtQueryMultipleValueKey( KeyHandle, ValueList, NumberOfValues, Buffer, Length, ReturnLength ) \
		FastSystemCall( syscallTable[113], 0, KeyHandle, ValueList, NumberOfValues, Buffer, Length, ReturnLength )

#define _NtQueryMutant( MutantHandle, MutantInformationClass, MutantInformation, MutantInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[114], 0, MutantHandle, MutantInformationClass, MutantInformation, MutantInformationLength, ResultLength )

#define _NtQueryObject( ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[115], 0, ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength, ReturnLength )

#define _NtQueryPerformanceCounter( PerformanceCount, PerformanceFrequency ) \
		FastSystemCall( syscallTable[116], 0, PerformanceCount, PerformanceFrequency )

#define _NtQuerySection( SectionHandle, SectionInformationClass, SectionInformation, SectionInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[117], 0, SectionHandle, SectionInformationClass, SectionInformation, SectionInformationLength, ResultLength )

#define _NtQuerySecurityObject( Handle, SecurityInformation, SecurityDescriptor, SecurityDescriptorLength, ReturnLength ) \
		FastSystemCall( syscallTable[118], 0, Handle, SecurityInformation, SecurityDescriptor, SecurityDescriptorLength, ReturnLength )

#define _NtQuerySemaphore( SemaphoreHandle, SemaphoreInformationClass, SemaphoreInformation, SemaphoreInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[119], 0, SemaphoreHandle, SemaphoreInformationClass, SemaphoreInformation, SemaphoreInformationLength, ResultLength )

#define _NtQuerySymbolicLinkObject( SymbolicLinkHandle, TargetName, ReturnLength ) \
		FastSystemCall( syscallTable[120], 0, SymbolicLinkHandle, TargetName, ReturnLength )

#define _NtQuerySystemEnvironmentValue( Name, Value, ValueLength, ReturnLength ) \
		FastSystemCall( syscallTable[121], 0, Name, Value, ValueLength, ReturnLength )

#define _NtQuerySystemInformation( SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[122], 0, SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength )

#define _NtQuerySystemTime( CurrentTime ) \
		FastSystemCall( syscallTable[123], 0, CurrentTime )

#define _NtQueryTimer( TimerHandle, TimerInformationClass, TimerInformation, TimerInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[124], 0, TimerHandle, TimerInformationClass, TimerInformation, TimerInformationLength, ResultLength )

#define _NtQueryTimerResolution( CoarsestResolution, FinestResolution, ActualResolution ) \
		FastSystemCall( syscallTable[125], 0, CoarsestResolution, FinestResolution, ActualResolution );

#define _NtQueryValueKey( KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, KeyValueInformationLength, ResultLength ) \
		FastSystemCall( syscallTable[126], 0, KeyHandle, ValueName, KeyValueInformationClass, KeyValueInformation, KeyValueInformationLength, ResultLength )

#define _NtQueryVirtualMemory( ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength ) \
		FastSystemCall( syscallTable[127], 0, ProcessHandle, BaseAddress, MemoryInformationClass, MemoryInformation, MemoryInformationLength, ReturnLength )

#define _NtQueryVolumeInformationFile( FileHandle, IoStatusBlock, VolumeInformation, VolumeInformationLength, VolumeInformationClass ) \
		FastSystemCall( syscallTable[128], 0, FileHandle, IoStatusBlock, VolumeInformation, VolumeInformationLength, VolumeInformationClass )

#define _NtQueueApcThread( ThreadHandle, ApcRoutine, ApcContext, Argument1, Argument2 ) \
		FastSystemCall( syscallTable[129], 0, ThreadHandle, ApcRoutine, ApcContext, Argument1, Argument2 )

#define _NtRaiseException( ExceptionRecord, Context, SearchFrames ) \
		FastSystemCall( syscallTable[130], 0, ExceptionRecord, Context, SearchFrames )

#define _NtRaiseHardError( Status, NumberOfArguments, StringArgumentsMask, Arguments, ResponseOption, Response ) \
		FastSystemCall( syscallTable[131], 0, Status, NumberOfArguments, StringArgumentsMask, Arguments, ResponseOption, Response )

#define _NtReadFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key ) \
		FastSystemCall( syscallTable[132], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key )

#define _NtReadFileScatter( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key ) \
		FastSystemCall( syscallTable[133], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key )

#define _NtReadRequestData( PortHandle, Message, Index, Buffer, BufferLength, ReturnLength ) \
		FastSystemCall( syscallTable[134], 0, PortHandle, Message, Index, Buffer, BufferLength, ReturnLength )

#define _NtReadVirtualMemory( ProcessHandle, BaseAddress, Buffer, BufferLength, ReturnLength ) \
		FastSystemCall( syscallTable[135], 0, ProcessHandle, BaseAddress, Buffer, BufferLength, ReturnLength )

#define _NtRegisterThreadTerminatePort( PortHandle ) \
		FastSystemCall( syscallTable[136], 0, PortHandle )

#define _NtReleaseMutant( MutantHandle, PreviousState ) \
		FastSystemCall( syscallTable[137], 0, MutantHandle, PreviousState )

#define _NtReleaseSemaphore( SemaphoreHandle, ReleaseCount, PPreviousCount ) \
		FastSystemCall( syscallTable[138], 0, SemaphoreHandle, ReleaseCount, PPreviousCount )

#define _NtRemoveIoCompletion( IoCompletionHandle, CompletionKey, CompletionValue, IoStatusBlock, Time ) \
		FastSystemCall( syscallTable[139], 0, IoCompletionHandle, CompletionKey, CompletionValue, IoStatusBlock, Time )

#define _NtReplaceKey( NewFileObjectAttributes, KeyHandle, OldFileObjectAttributes ) \
		FastSystemCall( syscallTable[140], 0, NewFileObjectAttributes, KeyHandle, OldFileObjectAttributes )

#define _NtReplyPort( PortHandle, ReplyMessage ) \
		FastSystemCall( syscallTable[141], 0, PortHandle, ReplyMessage );

#define _NtReplyWaitReceivePort( PortHandle, PortIdentifier, ReplyMessage, Message ) \
		FastSystemCall( syscallTable[142], 0, PortHandle, PortIdentifier, ReplyMessage, Message )

#define _NtReplyWaitReplyPort( PortHandle, ReplyMessage ) \
		FastSystemCall( syscallTable[143], 0, PortHandle, ReplyMessage )

#define _NtRequestPort( PortHandle, RequestMessage ) \
		FastSystemCall( syscallTable[144], 0, PortHandle, RequestMessage )

#define _NtRequestWaitReplyPort( PortHandle, RequestMessage, ReplyMessage ) \
		FastSystemCall( syscallTable[145], 0, PortHandle, RequestMessage, ReplyMessage )

#define _NtResetEvent( EventHandle, PreviousState ) \
		FastSystemCall( syscallTable[146], 0, EventHandle, PreviousState )

#define _NtRestoreKey( KeyHandle, FileHandle, Flags ) \
		FastSystemCall( syscallTable[147], 0, KeyHandle, FileHandle, Flags )

#define _NtResumeThread( ThreadHandle, PreviousSuspendCount ) \
		FastSystemCall( syscallTable[148], 0, ThreadHandle, PreviousSuspendCount )

#define _NtSaveKey( KeyHandle, FileHandle ) \
		FastSystemCall( syscallTable[149], 0, KeyHandle, FileHandle )

#define _NtSetContextThread( ThreadHandle, Context ) \
		FastSystemCall( syscallTable[150], 0, ThreadHandle, Context )

#define _NtSetDefaultHardErrorPort( PortHandle ) \
		FastSystemCall( syscallTable[151], 0, PortHandle )

#define _NtSetDefaultLocale( ThreadOrSystem, Locale ) \
		FastSystemCall( syscallTable[152], 0, ThreadOrSystem, Locale )

#define _NtSetEaFile( FileHandle, IoStatusBlock, Buffer, BufferLength ) \
		FastSystemCall( syscallTable[153], 0, FileHandle, IoStatusBlock, Buffer, BufferLength )

#define _NtSetEvent( EventHandle, PreviousState ) \
		FastSystemCall( syscallTable[154], 0, EventHandle, PreviousState )

#define _NtSetHighEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[155], 0, EventPairHandle )

#define _NtSetHighWaitLowEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[156], 0, EventPairHandle )

#define _NtSetInformationFile( FileHandle, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass ) \
		FastSystemCall( syscallTable[157], 0, FileHandle, IoStatusBlock, FileInformation, FileInformationLength, FileInformationClass )

#define _NtSetInformationKey( KeyHandle, KeyInformationClass, KeyInformation, KeyInformationLength ) \
		FastSystemCall( syscallTable[158], 0, KeyHandle, KeyInformationClass, KeyInformation, KeyInformationLength )

#define _NtSetInformationObject( ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength ) \
		FastSystemCall( syscallTable[159], 0, ObjectHandle, ObjectInformationClass, ObjectInformation, ObjectInformationLength )

#define _NtSetInformationProcess( ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength ) \
		FastSystemCall( syscallTable[160], 0, ProcessHandle, ProcessInformationClass, ProcessInformation, ProcessInformationLength )

#define _NtSetInformationThread( ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength ) \
		FastSystemCall( syscallTable[161], 0, ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength )

#define _NtSetInformationToken( TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength ) \
		FastSystemCall( syscallTable[162], 0, TokenHandle, TokenInformationClass, TokenInformation, TokenInformationLength )

#define _NtSetIntervalProfile( Interval, Source ) \
		FastSystemCall( syscallTable[163], 0, Interval, Source )

#define _NtSetIoCompletion( IoCompletionHandle, CompletionKey, CompletionValue, Status, Information ) \
		FastSystemCall( syscallTable[164], 0, IoCompletionHandle, CompletionKey, CompletionValue, Status, Information )

#define _NtSetLdtEntries( Selector1, LdtEntry1, Selector2, LdtEntry2 ) \
		FastSystemCall( syscallTable[165], 0, Selector1, LdtEntry1, Selector2, LdtEntry2 )

#define _NtSetLowEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[166], 0, EventPairHandle )

#define _NtSetLowWaitHighEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[167], 0, EventPairHandle )

#define _NtSetSecurityObject( Handle, SecurityInformation, SecurityDescriptor ) \
		FastSystemCall( syscallTable[168], 0, Handle, SecurityInformation, SecurityDescriptor )

#define _NtSetSystemEnvironmentValue( Name, Value ) \
		FastSystemCall( syscallTable[169], 0, Name, Value )

#define _NtSetSystemInformation( SystemInformationClass, SystemInformation, SystemInformationLength ) \
		FastSystemCall( syscallTable[170], 0, SystemInformationClass, SystemInformation, SystemInformationLength )

#define _NtSetSystemPowerState( SystemAction, MinSystemState, Flags ) \
		FastSystemCall( syscallTable[171], 0, SystemAction, MinSystemState, Flags )

#define _NtSetSystemTime( NewTime, OldTime ) \
		FastSystemCall( syscallTable[172], 0,  NewTime, OldTime );

#define _NtSetTimer( TimerHandle, DueTime, TimerApcRoutine, TimerContext, Resume, Period, PreviousState ) \
		FastSystemCall( syscallTable[173], 0, TimerHandle, DueTime, TimerApcRoutine, TimerContext, Resume, Period, PreviousState )

#define _NtSetTimerResolution( RequestedResolution, Set, ActualResolution ) \
		FastSystemCall( syscallTable[174], 0, RequestedResolution, Set, ActualResolution )

#define _NtSetValueKey( KeyHandle, ValueName, TitleIndex, Type, Data, DataSize ) \
		FastSystemCall( syscallTable[175], 0, KeyHandle, ValueName, TitleIndex, Type, Data, DataSize )

#define _NtSetVolumeInformationFile( FileHandle, IoStatusBlock, Buffer, BufferLength, VolumeInformationClass ) \
		FastSystemCall( syscallTable[176], 0, FileHandle, IoStatusBlock, Buffer, BufferLength, VolumeInformationClass)

#define _NtShutdownSystem( Action ) \
		FastSystemCall( syscallTable[177], 0, Action )

#define _NtSignalAndWaitForSingleObject( HandleToSignal, HandleToWait, Alertable, Time ) \
		FastSystemCall( syscallTable[178], 0, HandleToSignal, HandleToWait, Alertable, Time )

#define _NtStartProfile( ProfileHandle ) \
		FastSystemCall( syscallTable[179], 0, ProfileHandle )

#define _NtStopProfile( ProfileHandle ) \
		FastSystemCall( syscallTable[180], 0, ProfileHandle )

#define _NtSuspendThread( ThreadHandle, PreviousSuspendCount ) \
		FastSystemCall( syscallTable[181], 0, ThreadHandle, PreviousSuspendCount )

#define _NtSystemDebugControl( ControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnLength ) \
		FastSystemCall( syscallTable[182], 0, ControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength, ReturnLength )

#define _NtTerminateProcess( ProcessHandle, ExitStatus ) \
		FastSystemCall( syscallTable[183], 0, ProcessHandle, ExitStatus )

#define _NtTerminateThread( ThreadHandle, ExitStatus ) \
		FastSystemCall( syscallTable[184], 0, ThreadHandle, ExitStatus )

#define _NtTestAlert( ) \
		FastSystemCall( syscallTable[185], 0 )

#define _NtUnloadDriver( DriverServiceName ) \
		FastSystemCall( syscallTable[186], 0, DriverServiceName);

#define _NtUnloadKey( KeyObjectAttributes ) \
		FastSystemCall( syscallTable[187], 0, KeyObjectAttributes )

#define _NtUnlockFile( FileHandle, IoStatusBlock, LockOffset, LockLength, Key ) \
		FastSystemCall( syscallTable[188], 0, FileHandle, IoStatusBlock, LockOffset, LockLength, Key )

#define _NtUnlockVirtualMemory( ProcessHandle, BaseAddress, LockSize, LockType ) \
		FastSystemCall( syscallTable[189], 0, ProcessHandle, BaseAddress, LockSize, LockType )

#define _NtUnmapViewOfSection( ProcessHandle, BaseAddress ) \
		FastSystemCall( syscallTable[190], 0, ProcessHandle, BaseAddress )

#define _NtVdmControl( Service, ServiceData ) \
		FastSystemCall( syscallTable[191], 0, Service, ServiceData )

#define _NtWaitForMultipleObjects( HandleCount, Handles, WaitType, Alertable, Time ) \
		FastSystemCall( syscallTable[192], 0, HandleCount, Handles, WaitType, Alertable, Time )

#define _NtWaitForSingleObject( Handle, Alertable, Time ) \
		FastSystemCall( syscallTable[193], 0, Handle, Alertable, Time )

#define _NtWaitHighEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[194], 0, EventPairHandle )

#define _NtWaitLowEventPair( EventPairHandle ) \
		FastSystemCall( syscallTable[195], 0, EventPairHandle )

#define _NtWriteFile( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key ) \
		FastSystemCall( syscallTable[196], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key )

#define _NtWriteFileGather( FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key ) \
		FastSystemCall( syscallTable[197], 0, FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, Buffer, Length, ByteOffset, Key )

#define _NtWriteRequestData( PortHandle, Message, Index, Buffer, BufferLength, ReturnLength ) \
		FastSystemCall( syscallTable[198], 0, PortHandle, Message, Index, Buffer, BufferLength, ReturnLength )

#define _NtWriteVirtualMemory( ProcessHandle, BaseAddress, Buffer, BufferLength, ReturnLength ) \
		FastSystemCall( syscallTable[199], 0, ProcessHandle, BaseAddress, Buffer, BufferLength, ReturnLength )

#define _NtYieldExecution( ) \
		FastSystemCall( syscallTable[200], 0 )

#endif