#include <Windows.h>
#include <iostream>

using namespace std;

wchar_t lpStubPath[MAX_PATH];
wchar_t lpFilePath[MAX_PATH];
wchar_t lpDonePath[MAX_PATH];

LPBYTE lpFile;
unsigned long dwFileLen;
BYTE lpKey[] = "2aikoniohf2";
unsigned long dwKeyLen = strlen((char *)lpKey);

void rc4(LPBYTE bytes, unsigned long bytesLength, LPBYTE k, unsigned long kLength)
{
    unsigned int i = 0, x = 0;
	
	for (i = 0; i < bytesLength; ++i)
	{
		bytes[i] =~ bytes[i] ^ k[x] & i;
		
		if (++x == kLength) x = 0;
	}
}

bool build()
{
	unsigned long byte;

	HANDLE hFile = CreateFile(lpFilePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wcout << L"Invalid file path." << endl;

		return false;
	}

	wcout << L"File successfully opened." << endl;

	dwFileLen = GetFileSize(hFile, NULL);

	wcout << L"File size calculated." << endl;

	if (dwFileLen == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);

		wcout << L"Invalid file size." << endl;

		return false;
	}

	lpFile = (unsigned char *) malloc (dwFileLen);

	if (lpFile == NULL)
	{
		CloseHandle(hFile);

		wcout << L"File buffer is null." << endl;

		return false;
	}

	ReadFile(hFile, lpFile, dwFileLen, &byte, NULL);

	CloseHandle(hFile);

	rc4(lpFile, dwFileLen, lpKey, dwKeyLen);

	wcout << L"File buffer encrypted." << endl;

	if (CopyFile(lpStubPath, lpDonePath, false) == 0)
	{
		wcout << L"Invalid stub path." << endl;
		
		return false;
	}

	HANDLE hUpdate = BeginUpdateResource(lpDonePath, false);
	
	if (hUpdate == NULL)
	{
		wcout << L"Could not create output file." << endl;

		return false;
	}

	else
	{
		UpdateResource(hUpdate, MAKEINTRESOURCE(3), MAKEINTRESOURCE(1), NULL, lpFile, dwFileLen);

		EndUpdateResource(hUpdate, false);

		return true;
	}
}

int wmain()
{
	HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );

	WORD wOldColorAttrs;

	CONSOLE_SCREEN_BUFFER_INFO csbiInfo; 
  
	GetConsoleScreenBufferInfo(h, &csbiInfo);

	wOldColorAttrs = csbiInfo.wAttributes; 
	
	SetConsoleTitleA(getenv("USERNAME"));
	
	SetConsoleTextAttribute ( h, FOREGROUND_RED | FOREGROUND_INTENSITY );
  
	wcout << L"Type in the path of the file to crypt." << endl;

	wcin >> lpFilePath;

	wcout << L"Type in the path of the stub to use." << endl;

	wcin >> lpStubPath;

	wcout << L"Type in the path of the output file." << endl;

	wcin >> lpDonePath;

	if (build())
	{
		wcout << L"File successfully protected." << endl;

		return 0;
	}
	
	else
	{
		wcout << L"Failed to protect file, error code " << GetLastError() << '.' << endl;

		return 1;
	}
}