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


#ifndef _CRYPT32REPLACEMENT_H
#define _CRYPT32REPLACEMENT_H

BOOL WINAPI MyDecryptFileA(LPCSTR lpFileName, DWORD dwReserved);
BOOL WINAPI MyDecryptFileW(LPCWSTR lpFileName, DWORD dwReserved);
BOOL WINAPI MyEncryptFileA(LPCSTR lpFileName);
BOOL WINAPI MyEncryptFileW(LPCWSTR lpFileName);
BOOL WINAPI MyCryptEncrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
BOOL WINAPI MyCryptDecrypt(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
BOOL WINAPI MyCryptCreateHash(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags, HCRYPTHASH *phHash);
BOOL WINAPI MyCryptHashData(HCRYPTHASH hHash, BYTE *pbData, DWORD dwDataLen, DWORD dwFlags);
BOOL WINAPI MyCPEncrypt(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
BOOL WINAPI MyCPDecrypt(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
BOOL WINAPI MyCryptProtectData(DATA_BLOB *pDataIn, LPCWSTR szDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);
BOOL WINAPI MyCryptUnprotectData(DATA_BLOB *pDataIn, LPWSTR *ppszDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);
BOOL WINAPI MyCryptProtectMemory(LPVOID pData, DWORD cbData, DWORD dwFlags);
BOOL WINAPI MyCryptUnprotectMemory(LPVOID pData, DWORD cbData, DWORD dwFlags);
BOOL WINAPI MyCryptMsgGetParam(HCRYPTMSG hCryptMsg, DWORD dwParamType, DWORD dwIndex, void *pvData, DWORD *pcbData);
BOOL WINAPI MyCryptMsgUpdate(HCRYPTMSG hCryptMsg, const BYTE *pbData, DWORD cbData, BOOL fFinal);
HCRYPTMSG WINAPI MyCryptMsgOpenToDecode(DWORD dwMsgEncodingType, DWORD dwFlags, DWORD dwMsgType, HCRYPTPROV_LEGACY hCryptProv, PCERT_INFO pRecipientInfo, PCMSG_STREAM_INFO pStreamInfo);
BOOL WINAPI MyCryptHashMessage(PCRYPT_HASH_MESSAGE_PARA pHashPara, BOOL fDetachedHash, DWORD cToBeHashed, const BYTE *rgpbToBeHashed[], DWORD rgcbToBeHashed[], BYTE *pbHashedBlob, DWORD *pcbHashedBlob, BYTE *pbComputedHash, DWORD *pcbComputedHash);
BOOL WINAPI MyCryptDecryptMessage(PCRYPT_DECRYPT_MESSAGE_PARA pDecryptPara, const BYTE *pbEncryptedBlob, DWORD cbEncryptedBlob, BYTE *pbDecrypted, DWORD *pcbDecrypted, PCCERT_CONTEXT *ppXchgCert);
BOOL WINAPI MyCryptEncryptMessage(PCRYPT_ENCRYPT_MESSAGE_PARA pEncryptPara, DWORD cRecipientCert, PCCERT_CONTEXT rgpRecipientCert[], const BYTE *pbToBeEncrypted, DWORD cbToBeEncrypted, BYTE *pbEncryptedBlob, DWORD *pcbEncryptedBlob);
SECURITY_STATUS WINAPI MyNCryptDecrypt(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags);
SECURITY_STATUS WINAPI MyNCryptEncrypt(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags);
NTSTATUS WINAPI MyBCryptDecrypt(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags);
NTSTATUS WINAPI MyBCryptEncrypt(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags);
NTSTATUS WINAPI MyBCryptHashData(BCRYPT_HASH_HANDLE hHash, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags);
NTSTATUS WINAPI MyBCryptGenerateSymmetricKey(BCRYPT_ALG_HANDLE hAlgorithm, BCRYPT_KEY_HANDLE *phKey, PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR pbSecret, ULONG cbSecret, ULONG dwFlags);
SECURITY_STATUS WINAPI MySslDecryptPacket(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwFlags);
SECURITY_STATUS WINAPI MySslEncryptPacket(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwContentType, DWORD dwFlags);
SECURITY_STATUS WINAPI MyNCryptCreatePersistedKey(NCRYPT_PROV_HANDLE hProvider, NCRYPT_KEY_HANDLE *phKey, LPCWSTR pszAlgId, LPCWSTR pszKeyName, DWORD dwLegacyKeySpec, DWORD dwFlags);

typedef BOOL (WINAPI *MYDECRYPTFILEA)(LPCSTR lpFileName, DWORD dwReserved);
typedef BOOL (WINAPI *MYDECRYPTFILEW)(LPCWSTR lpFileName, DWORD dwReserved);
typedef BOOL (WINAPI *MYENCRYPTFILEA)(LPCSTR lpFileName);
typedef BOOL (WINAPI *MYENCRYPTFILEW)(LPCWSTR lpFileName);
typedef BOOL (WINAPI *MYCRYPTENCRYPT)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
typedef BOOL (WINAPI *MYCRYPTDECRYPT)(HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI *MYCRYPTCREATEHASH)(HCRYPTPROV hProv, ALG_ID Algid, HCRYPTKEY hKey, DWORD dwFlags, HCRYPTHASH *phHash);
typedef BOOL (WINAPI *MYCRYPTHASHDATA)(HCRYPTHASH hHash, BYTE *pbData, DWORD dwDataLen, DWORD dwFlags);
typedef BOOL (WINAPI *MYCPENCRYPT)(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen, DWORD dwBufLen);
typedef BOOL (WINAPI *MYCPDECRYPT)(HCRYPTPROV hProv, HCRYPTKEY hKey, HCRYPTHASH hHash, BOOL Final, DWORD dwFlags, BYTE *pbData, DWORD *pdwDataLen);
typedef BOOL (WINAPI *MYCRYPTPROTECTDATA)(DATA_BLOB *pDataIn, LPCWSTR szDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);
typedef BOOL (WINAPI *MYCRYPTUNPROTECTDATA)(DATA_BLOB *pDataIn, LPWSTR *ppszDataDescr, DATA_BLOB *pOptionalEntropy, PVOID pvReserved, CRYPTPROTECT_PROMPTSTRUCT *pPromptStruct, DWORD dwFlags, DATA_BLOB *pDataOut);
typedef BOOL (WINAPI *MYCRYPTPROTECTMEMORY)(LPVOID pData, DWORD cbData, DWORD dwFlags);
typedef BOOL (WINAPI *MYCRYPTUNPROTECTMEMORY)(LPVOID pData, DWORD cbData, DWORD dwFlags);
typedef BOOL (WINAPI *MYCRYPTMSGGETPARAM)(HCRYPTMSG hCryptMsg, DWORD dwParamType, DWORD dwIndex, void *pvData, DWORD *pcbData);
typedef BOOL (WINAPI *MYCRYPTMSGUPDATE)(HCRYPTMSG hCryptMsg, const BYTE *pbData, DWORD cbData, BOOL fFinal);
typedef HCRYPTMSG (WINAPI *MYCRYPTMSGOPENTODECODE)(DWORD dwMsgEncodingType, DWORD dwFlags, DWORD dwMsgType, HCRYPTPROV_LEGACY hCryptProv, PCERT_INFO pRecipientInfo, PCMSG_STREAM_INFO pStreamInfo);
typedef BOOL (WINAPI *MYCRYPTHASHMESSAGE)(PCRYPT_HASH_MESSAGE_PARA pHashPara, BOOL fDetachedHash, DWORD cToBeHashed, const BYTE *rgpbToBeHashed[], DWORD rgcbToBeHashed[], BYTE *pbHashedBlob, DWORD *pcbHashedBlob, BYTE *pbComputedHash, DWORD *pcbComputedHash);
typedef BOOL (WINAPI *MYCRYPTDECRYPTMESSAGE)(PCRYPT_DECRYPT_MESSAGE_PARA pDecryptPara, const BYTE *pbEncryptedBlob, DWORD cbEncryptedBlob, BYTE *pbDecrypted, DWORD *pcbDecrypted, PCCERT_CONTEXT *ppXchgCert);
typedef BOOL (WINAPI *MYCRYPTENCRYPTMESSAGE)(PCRYPT_ENCRYPT_MESSAGE_PARA pEncryptPara, DWORD cRecipientCert, PCCERT_CONTEXT rgpRecipientCert[], const BYTE *pbToBeEncrypted, DWORD cbToBeEncrypted, BYTE *pbEncryptedBlob, DWORD *pcbEncryptedBlob);
typedef SECURITY_STATUS (WINAPI *MYNCRYPTDECRYPT)(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags);
typedef SECURITY_STATUS (WINAPI *MYNCRYPTENCRYPT)(NCRYPT_KEY_HANDLE hKey, PBYTE pbInput, DWORD cbInput, VOID *pPaddingInfo, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, DWORD dwFlags);
typedef NTSTATUS (WINAPI *MYBCRYPTDECRYPT)(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags);
typedef NTSTATUS (WINAPI *MYBCRYPTENCRYPT)(BCRYPT_KEY_HANDLE hKey, PUCHAR pbInput, ULONG cbInput, VOID *pPaddingInfo, PUCHAR pbIV, ULONG cbIV, PUCHAR pbOutput, ULONG cbOutput, ULONG *pcbResult, ULONG dwFlags);
typedef NTSTATUS (WINAPI *MYBCRYPTHASHDATA)(BCRYPT_HASH_HANDLE hHash, PUCHAR pbInput, ULONG cbInput, ULONG dwFlags);
typedef NTSTATUS (WINAPI *MYBCRYPTGENERATESYMMETRICKEY)(BCRYPT_ALG_HANDLE hAlgorithm, BCRYPT_KEY_HANDLE *phKey, PUCHAR pbKeyObject, ULONG cbKeyObject, PUCHAR pbSecret, ULONG cbSecret, ULONG dwFlags);

typedef SECURITY_STATUS (WINAPI *MYSSLDECRYPTPACKET)(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwFlags);
typedef SECURITY_STATUS (WINAPI *MYSSLENCRYPTPACKET)(NCRYPT_PROV_HANDLE hSslProvider, NCRYPT_KEY_HANDLE hKey, PBYTE *pbInput, DWORD cbInput, PBYTE pbOutput, DWORD cbOutput, DWORD *pcbResult, ULONGLONG SequenceNumber, DWORD dwContentType, DWORD dwFlags);
typedef SECURITY_STATUS (WINAPI *MYNCRYPTCREATEPERSISTEDKEY)(NCRYPT_PROV_HANDLE hProvider, NCRYPT_KEY_HANDLE *phKey, LPCWSTR pszAlgId, LPCWSTR pszKeyName, DWORD dwLegacyKeySpec, DWORD dwFlags);

#endif
