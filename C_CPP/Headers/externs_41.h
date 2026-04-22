extern MAINSTRUCT MainStruct;
extern IRC MainIrc;
extern THREAD threads[MAXTHREADS];
extern PLUGIN plugins[MAXPLUGINS];

extern bool AutoStart;			// enable autostart registry keys
extern char Prefix;				// command prefix (one character max.)

extern IRCSERVERS ServerList[];

extern EXPLOITS ExploitList[30];

extern short CurrentVersion;

extern char shellcode[];
//extern short CurrentServer;

extern bool ParseTopic;
extern bool ParseMotd;

extern bool RandomFilename;

extern short MaxRand;

extern char Version[];
extern char ID[];

extern char Filename[];

extern char Password[];

extern char *AccessList[];

extern char *VersionList[];
