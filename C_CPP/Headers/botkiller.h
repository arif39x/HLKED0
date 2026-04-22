#ifndef NO_BOTKILLER

typedef struct BK {
	SOCKET Sock;
	int tnum;
	int ScanDelay;				
} BK;

DWORD WINAPI BotKillerThread(LPVOID param);

#endif