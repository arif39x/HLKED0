// functions.h - iStealer v4.0 - Kizar Labs 2009
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

void Base64Decode(int none, char *szInput, char *szOutput);
int ReadFileData(char *filePath, char **buffer, DWORD *bufferSize);
int FileSize(char *filePath);
int ReadKeyData(HKEY hKey, char *subKey, char *value, char *buffer);
unsigned long FindString(char *buffer, unsigned long bufferlen, char *string, unsigned long start);
int GetLine(unsigned long *start, char *buffer, unsigned long bufferlen, char *find, char *output);
char *UrlEncode(char *string);

// custom developed functions
int GetPathFromRegistryEntry(char * Path, HKEY Key, char * SubKey, char * Value = NULL);
int Cut(char * Output, char * SearchIn, char * Delimiter1, char * Delimiter2);
