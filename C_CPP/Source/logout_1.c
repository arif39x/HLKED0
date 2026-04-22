#ifndef _LOGOUT_INCLUDED_
#define _LOGOUT_INCLUDED_

#define logout(x) /*   x    */

/*
void logout(char *in)
{
HANDLE ff;
DWORD sz;
ff=CreateFile("c:\\KeyHook.log",GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
if(ff==INVALID_HANDLE_VALUE||ff==0){ return; };
SetFilePointer(ff,0,NULL,FILE_END);
WriteFile(ff,in,strlen(in),&sz,NULL);
CloseHandle(ff);
}
*/


#endif
