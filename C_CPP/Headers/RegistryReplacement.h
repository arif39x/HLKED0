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


#ifndef _REGISTRYREPLACEMENT_H
#define _REGISTRYREPLACEMENT_H

void getDataType(DWORD *pType, char *pOutBuffer);
void getKeyRootName(HKEY pKeyHandle, char *pOutBuffer);


LONG WINAPI MyRegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
LONG WINAPI MyRegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
LONG WINAPI MyRegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
LONG WINAPI MyRegOpenKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult);
LONG WINAPI MyRegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
LONG WINAPI MyRegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
LONG WINAPI MyRegQueryValueA(HKEY hKey, LPCSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue);
LONG WINAPI MyRegQueryValueW(HKEY hKey, LPCWSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue);
LONG WINAPI MyRegGetValueA(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);
LONG WINAPI MyRegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);


typedef LONG (WINAPI *MYREGOPENKEYEXA)(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
typedef LONG (WINAPI *MYREGOPENKEYEXW)(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult);
typedef LONG (WINAPI *MYREGOPENKEYA)(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult);
typedef LONG (WINAPI *MYREGOPENKEYW)(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult);
typedef LONG (WINAPI *MYREGQUERYVALUEEXA)(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef LONG (WINAPI *MYREGQUERYVALUEEXW)(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
typedef LONG (WINAPI *MYREGQUERYVALUEA)(HKEY hKey, LPCSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue);
typedef LONG (WINAPI *MYREGQUERYVALUEW)(HKEY hKey, LPCWSTR lpSubKey, LPTSTR lpValue, PLONG lpcbValue);
typedef LONG (WINAPI *MYREGGETVALUEA)(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);
typedef LONG (WINAPI *MYREGGETVALUEW)(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData);

#endif
