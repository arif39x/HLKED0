/* /* 
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
#ifndef _INC_NICKNAMES_H
#define _INC_NICKNAMES_H

extern unsigned long s_NKey[4];
extern long NLen;
extern unsigned char Nicks[3855];
extern unsigned long s_NKeyFF[4];



extern int iMAXNICKz;

#define MAX_NICKS 732
#define MAX_NICK_LEN iMAXNICKz

char *GetNick(BOOL bRaster);
void Rasterize(char *cNick);
int RetvNick(char *cBuf,int iNum);

#endif //_INC_NICKNAMES_H