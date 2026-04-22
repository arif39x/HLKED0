#ifndef _REGIST_INCLUDED_
#define _REGIST_INCLUDED_

#include "string.h"

int regist_get(char *dir,char *name,char *value)
{
DWORD sz;
HKEY hnd;
if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,dir,0,KEY_QUERY_VALUE,&hnd)!=ERROR_SUCCESS){ value[0]=0; return 0; };
sz=200;value[0]=0;
RegQueryValueEx(hnd,name,NULL,NULL,(unsigned char*)value,&sz);
RegCloseKey(hnd);
return 1;
}

int regist_set(char *dir,char *name,char *value)
{
HKEY hnd;
if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,dir,0,KEY_SET_VALUE,&hnd)!=ERROR_SUCCESS){ value[0]=0; return 0; };
RegSetValueEx(hnd,name,NULL,REG_SZ,(unsigned char*)value,strlen(value)+1);
RegCloseKey(hnd);
return 1;
}

int regist_getbin(char *dir,char *name,char *value,int size)
{
DWORD sz;
HKEY hnd;
if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,dir,0,KEY_QUERY_VALUE,&hnd)!=ERROR_SUCCESS){ value[0]=0; return 0; };
sz=size;
RegQueryValueEx(hnd,name,NULL,NULL,(unsigned char*)value,&sz);
RegCloseKey(hnd);
return 1;
}

int regist_setbin(char *dir,char *name,char *value,int size)
{
HKEY hnd;
if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,dir,0,KEY_SET_VALUE,&hnd)!=ERROR_SUCCESS){ value[0]=0; return 0; };
RegSetValueEx(hnd,name,NULL,REG_BINARY,(unsigned char*)value,size);
RegCloseKey(hnd);
return 1;
}

#endif
