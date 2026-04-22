#ifndef _PATHS_INCLUDED_
#define _PATHS_INCLUDED_

#include "crypt.c"

int paths_usedir(char *ptr,int type,char *ad)
{
ptr[0]=0;
if(type==1){ GetSystemDirectory(ptr,80); };
if(type==2){ GetWindowsDirectory(ptr,80); };
if(type==3){ GetTempPath(80,ptr); };
ptr[80]=0;
strcat(ptr,ad);
return 0;
}

char * paths_target()
{
static char r[100];
paths_usedir(r,crypt.target_type,crypt.target_file);
return r;
}

char * paths_temp()
{
static char r[100];
paths_usedir(r,crypt.temp_type,crypt.temp_file);
return r;
}

char * paths_exe()
{
static char aa[200];
GetModuleFileName(NULL,aa,199);
aa[199]=0;
return aa;
}

#endif
