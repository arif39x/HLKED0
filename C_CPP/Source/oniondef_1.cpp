#include "onion.h"

HANDLE WINAPI _OpenThread( DWORD DesiredAccess, BOOL InheritHandle, DWORD ThreadId )
{
	HANDLE ThreadHandle;
	CLIENT_ID CliId;
	OBJECT_ATTRIBUTES ObjAtt;
	NTSTATUS ErrorCode;

	CliId.UniqueProcess = 0;
	CliId.UniqueThread = ( PVOID ) ThreadId;

	if( !InheritHandle )
		ObjAtt.Attributes = 0;
	else
		ObjAtt.Attributes = 2;

	ObjAtt.ObjectName = 0;

	if( ( ErrorCode = _NtOpenThread( &ThreadHandle, DesiredAccess, &ObjAtt, &CliId ) ) == 0 )
		return( ThreadHandle );
	else
		return( ( HANDLE ) ErrorCode );
}

LPVOID WINAPI _VirtualAllocEx( HANDLE ProcessHandle, LPVOID Address, SIZE_T Size, DWORD AllocationType, DWORD Protect )
{
	SIZE_T LclSize = Size;
	return( ( LPVOID ) _NtAllocateVirtualMemory( ProcessHandle, Address, 0x00, &LclSize, AllocationType, Protect ) );
}

LPVOID WINAPI _VirtualAlloc( LPVOID Address, SIZE_T Size, DWORD AllocationType, DWORD Protect )
{
	PVOID lclAddress = ( PVOID ) 1;
	if( !Address )
	{
		_VirtualAllocEx( _GetCurrentProcess( ), &lclAddress, Size, AllocationType, Protect );
		return( lclAddress );
	}
	else
	{
		_VirtualAllocEx( _GetCurrentProcess( ), Address, Size, AllocationType, Protect );
		return( Address );
	}
}

HANDLE WINAPI _GetCurrentProcess( )
{
	return( ( HANDLE ) ~( DWORD_PTR ) 0 );
}

DWORD WINAPI _GetCurrentProcessId( )
{
	#ifdef _M_IX86
		return( *( ( DWORD_PTR * ) __readfsdword( 0x18 ) + 0x20 / sizeof( DWORD_PTR ) ) );
	#elif _M_IX64
		return( *( ( DWORD_PTR * ) __readgsqword( 0x30 ) + 0x40 / sizeof( DWORD_PTR ) ) );
	#endif
}

HANDLE WINAPI _GetCurrentThread( )
{
	return( ( HANDLE ) ~( DWORD_PTR ) 1 );
}

BOOL WINAPI _GetThreadContext( HANDLE ThreadHandle, CONTEXT *Context )
{
	return( _NtGetContextThread( ThreadHandle, Context ) );
}

BOOL WINAPI _CloseHandle( HANDLE Object )
{
	return( _NtClose( Object ) );
}

VOID WINAPI _ExitProcess( UINT ExitCode )
{
	_NtTerminateProcess( _GetCurrentProcess( ), ExitCode );
}

SIZE_T WINAPI _VirtualQuery( void *Address, PMEMORY_BASIC_INFORMATION Buffer, SIZE_T Length )
{
	return( _NtQueryVirtualMemory( _GetCurrentProcess( ), Address, 0, Buffer, Length, 0 ) );
}