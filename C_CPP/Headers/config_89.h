/* Configuration File <Please Customize> */

/* Bot Settings */

// Bots nickname:
#define	BOTNICK	"Clambot"
// IRC Server to connect to:
#define	SERVER	"irc.prison.net"
// IRC Server Port:
#define	PORT	"6667"
// Auto-join this channel:
#define	CHANNEL	"#x-war"

/* These 2 Values apply to both bot and botserv. */

// Server running BotNET Communication Stream (botserv)
// ie: "localhost" to connect to your own database after
// executing bin/botserv.
#define	DATABASE	"localhost"
// Port of this server.
#define	SERVPORT	"43210"

/* Non-crucial Bot Settings */

// WHOIS Bot Information.
// Change ONLY if you know what you are doing or else
// the bot may have trouble connecting to the server.
#define USER    "aibot localhost aibot aibot"
// Bot Version Reply.
#define	VERSION_REPLY	"BotNET v1.0 stable release [/ebbtepid/]"
// Help Delay: Change to 0 (or reduce) if server doesnt have flood control.
// 1900000 is 1.9 seconds. 1000000 is 1 second. 500000 is 0.5 seconds.
#define	HELP_DELAY	1900000

/* Administrative Settings */

// Administrator Password for use with 'adduser' and 'deluser' Bot Commands.
// Usage: /msg botnick access <password>
// Change this default value!
#define	PASSWORD	"admin"
// Maximum amount of users that can obtain Bot Access.
#define	MAXUSERS	20
// Maximum amount of channels that the bot can join.
#define	MAXCHANS	3
// Storage location of your MP3's.
// This directory can actually be the location for any type of files.
// The bot will share all files in this directory and not just mp3's.
#define	MP3DIR		"/home/boaz/mp3"
