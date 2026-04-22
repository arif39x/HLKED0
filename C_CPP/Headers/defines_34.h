/****************************************************************** *
 * Modify your darkbot settings by changing the values of the data
 * in this file.
 ****************************************************************** */


#define VERSION_REPLY "Hi, I'm a Darkbot. Download me from \2http://darkbot.net\2"

/* signal.h path is located here for those of you who must change it's
 * path */
#include <signal.h>

/* Modify the following defines if needed- ------------------------ */
/* Please note that the below defines are the RECOMMENDED settings! */
/* So if you don't know if you should change something, leave it :) */

#define NEED_LIBC5 OFF
/**
 * If you have an OS with libc5 as primary libc, you need this
 * defined. You'll know if your shell requires this define
 * because your bot will connect, but not say anything :)
 * Most BSD systems will not need this, but Linux systems tend to
 * need it. <green@crimea.edu>
 */

#define	SGI OFF
/**
 * SGI boxes (ie; IRIX system) need this
 * define to run. Added by fouton
 * <c592030@everest.cclabs.missouri.edu>
 */

#define	LANG 1
/**
 * What language do you want your darkbot to
 * speak in? Pick a number from below.
 * -------------------------------------------
 * 1 = ENGLISH	- play <jason@superlink.net>
 * 2 = FRENCH	- Inajira <inajira@videotron.ca>
 *		EfX <michel.efx@globetrotter.net>
 *		eCHaLoTTe <echalotte@cablevision.qc.ca>
 * 3 = SPANISH  - speed1 <speed@eduredes.edu.do>
 * 4 = DUTCH    - Asmodai <asmodai@wxs.nl>
 * 5 = LATIN	- Otaku <otaku@unixgeek.ml.org>
 * 6 = GREEK    - Chris_CY <chriscy@cylink.net>
 * 7 = EBONICS  - rapsux <bitter@ici.net>
 * 8 = PIG LATIN- Cloud <burtner@usa.net>
 * 9 = RUSSIAN	- KOI8 encoding
 * 10= RUSSIAN	- CP1251 encoding
 *              - Oleg Drokin <green@ccssu.crimea.ua>
 * 11= PORTUGUSE- Pincel <Pincel@worldnet.att.net>
 * 12= GERMAN	- C.Hoegl@gmx.net & marc@reymann.net
 * -------------------------------------------------
 * ? = Email me if you want to help add other langs!
 * -------------------------------------------------
 */

#define DO_CHANBOT_CRAP OFF
/** 
 * Enable this if you want to make your darkbot bloated
 * with stuff like !KICK, !UP, !WACK, etc. Basically
 * anything dealing with channel modes and kicking commands.
 * This includes PERMBANS. On linux, this makes the final
 * darkbot bin shrink from 82k to 78k
 */

#define	GENERAL_QUESTIONS ON
/**
 * Answer questions that match a topic
 * without addressing the bot (recommended)
 */

#define STATUS OFF
/**
 * Parse luser data? May cause SIG_SEGV on
 * some ircd's (ie; non ircu) Basically all this does
 * is display network info like opers, servers, avg # 
 * of users on the servers, etc.
 */

#define DISPLAY_SYNC OFF
/**
 * When bot joins a channel, sometimes it's hard
 * to tell when it's "synced" and is no longer processing
 * stuff -- this tells the channel the bot has finished
 * syncing
 */

#define BITCH_ABOUT_DEOP OFF
/**
 * will complain in the chan that the bot is deoped in.
 */

#define	DEFAULT_UMODE "+i-ds"
/**
 * What user modes do you want for darkbot?
 */

#define	HELP_GREET OFF
/**
 * Give user's who join a NOTICE with info on how to use the darkbot?
 */

#define	JOIN_GREET ON
/**
 * Do auto-greets for users in your userlist?
 */

#define	CTCP ON
/**
 * Reply to CTCP's? (PING/VERSION)
 */

#define	CHECK_STONED ON
/**
 * Check if servers are not responding, is so
 * connect to next server in list. (recommended)
 */

#define KICK_ON_BAN OFF
/**
 * Kick people out when they are banned?
 */

#define	DO_WHOIS ON
/**
 * Want to be alerted when a user who joins
 * is in "questionable" other channels? This
 * option only works on Undernet - since the
 * NOTICE sent uses /notice @#chan
 */

#define LASTCOMM_TIME 5
/**
 * LASTCOMM_TIME is the length of time (in seconds)
 * that your bot will not reply to a topic already
 * asked. Thus if someone asked your bot about "mirc"
 * that question could not be asked again in the
 * same format for N seconds (or till the question
 * is out of buffer). This prevents the bot from
 * falling prey to users who like to repeat.
 */

#define SLASTCOMM_TIME 60
/**
 * This is the length of time to NOT allow someone
 * to be recounted when they rejoin your channels,
 * which tends to clutter up everyone's screen with
 * the setinfo. This basically keeps track of who
 * joined in the last ___ seconds, and does not do
 * their setinfo during that amount of time.
 */

/**
 * BELOW is the output timers. Darkbot does not
 * output text without first putting it into a
 * que list. If the bot has several lines of text
 * waiting to be sent, it starts to delay longer
 * and longer between output, so it can't flood
 * itself off of IRC. Explanation:
 *
 * If text in que is < OUTPUT1_COUNT, output it.
 * If text in que is > OUTPUT1_COUNT, delay
 * OUTPUT1_DELAY seconds. If que is > OUTPUT2_COUNT,
 * delay OUTPUT2_DELAY seconds. If number of text
 * in que is higher than OUTPUT_PURGE_COUNT, then
 * just delete all unneeded output (ie; any text
 * and notices, but leaving in stuff like kicks
 * and modes) The defaults below are recommended,
 * as the bot isn't going to flood off. If you are
 * having the bot delete output messages and you'd
 * like to increase the que, update the 
 * OUTPUT_PURGE_COUNT to a larger number. Just keep
 * in mind if someone floods your bot with a lot of
 * VERSION requests, the bot will sit there outputting
 * a lot of version replies instead of deleting them,
 * causing it to act as if it's just sitting there not
 * doing anything when you ask it something in your
 * channel.
 */
#define OUTPUT1_COUNT 4
#define OUTPUT1_DELAY 1
#define OUTPUT2_COUNT 6
#define OUTPUT2_DELAY 2
/**
 * If still < OUTPUT_PURGE_COUNT and > OUTPUT2_COUNT
 * then delay OUTPUT3_DELAY secs
 */
#define OUTPUT3_DELAY 3
/**
 * When all else fails... if more than OUTPUT_PURGE_COUNT
 * delete them all! No use in making the bot output slowly
 * over a long period of time... imagine if you set this to
 * 50, and had an OUTPUT3_DELAY of 3 secs.. thats 50*3 secs
 * till the bot is ready to output any new data to you!
 */
#define OUTPUT_PURGE_COUNT 7

#define	ALLOW_RAW_TOPICS_USING_MODE OFF
/**
 * Allow the raw topics to use MODE? This is not
 * recommended because level 1 users can add topics
 * such as "MODE C~ +o N~" and thus get ops, this is
 * not a recommended define, but is here just in case
 * some of you have special needs for the MODE
 */

#define	DO_CHAN_HTML OFF
/**
 * Create a html page with the # of users,
 * nicks, u@h's and idle times of all your
 * users. If you get nukes and DoS attacks,
 * it may be wise to undef this option, since
 * the (ab)user's can find your nicks and
 * u@h's to nuke off IRC
 */

#define	LOG_PRIVMSG ON
/**
 * Do you want to log all privmsg's to your
 * darkbot? (will be saved to logs/ dir)
 */

#define	UPDATE_COUNTER	10
/**
 * This option is only used if the DO_CHAN_HTML is
 * defined. What is the # of joins to update the
 * webpage?
 */

#define DBTIMERS ON
/**
 * Do you want to have your darkbot have online timers?
 * This allows you to have your darkbot have an "alarm
 * clock" type function, where you tell it to give you
 * a notify in X minutes/hours/days for an upcoming 
 * event.
 */

#ifdef	WIN32
#define	CHAN_HTML_PATH	"channel.html"	/* this is for win32 users */
#else
#define	CHAN_HTML_PATH "/home/yourname/darkbot/channel.html"
/**
 * unix ver:
 * path to write your channel's html page (if the DO_CHAN_HTML is defined)
 */
#endif

/*
 * The following should remain defined, however you can change them to
 * whatever you want, to customize your darkbot - They are less important
 * than the above defines, so generally speaking, there isn't a NEED to mess
 * with them.
 */

#define	COMPLAIN_REASON	"grrr, kick me again and I'm going to..."
#define	DONT_THINK_SO	"I don't think so."
#define	WHUT		"hmmm?"
#define	DONNO_Q		"*shrug*"
#define	FLOOD_REASON	"Don't flood!"
#define	DEFAULT_KICK	"Requested!"
#define	CANT_FIND	"Was unable to find"	/* ... */
#define	NO_TOPIC	"Sorry, I don't have any entry for"	/* ... */
#define	EXISTING_ENTRY	"Sorry, there is an existing entry under keyword"
#define	NO_ENTRY	"I was unable to find entry:"
#define	TRY_FIND	"What am I trying to find"
#define	WAKEUP_ACTION	"\1ACTION wakes up from a snooze..\1"
#define	GOSLEEP_ACTION	"\1ACTION falls asleep... ZzzZZzzZZzz\1"
#if BITCH_ABOUT_DEOP == 1
#define BITCH_DEOP_REASON "grr, someone op me!"
#endif

#define	SORT
/**
 * Sort your info2.db on startup? Will take
 * forever if u have a large db
 */

#define	LOG_ADD_DELETES
/** Do you want to log who adds/deletes
 * topics? log saved to logs/add_delete.log
 */

#undef	FIND_DUPS
/**
 * When user's do the INFO command, and at
 * startup, do you want to find and remove
 * duplicates database entries?
 */

#undef	SAVE_DUPS
/** When duplicate topics are found, do you
 * want to save them? (in case some topics
 * are accidently deleted)
 */

#define	MAX_LASTSEEN	604800
/**
 * Max length to keep a lastseen (default is
 * one week (in seconds))
 */

#define	SEEN_REPLY	"in the last week."
/**
 * if you change the above
 * time (MAX_LASTSEEN) from a
 * week, be sure to update
 * the SEEN_REPLY to the
 * respective time length
 */

#define	DO_MATH_STUFF
/**
 * Do you want your bot to do math commands?
 * This _MAY_ cause probs on some boxes, but
 * so far worked fine on all I've seen.
 */

#define	FLOOD_KICK
/**
 * Define this if you want your darkbot to
 * KICK out the people who flood it. NOTE: This
 * means flooding your BOT, not your CHANNEL. If you
 * don't define this, it will just ignore the user
 */

#define	REQ_ACCESS_ADD
/**
 * Require access to add help topics?
 */

#define	REQ_ACCESS_DEL
/**
 * Require access to delete help topics?
 */

#define	RAND_STUFF_TIME	3600
/**
 * Time in seconds to randomly do something
 * from dat/random.ini (default is 1 hour)
 */

#define	RAND_LEVEL	2
/**
 * Level at which user's can on-line add new
 * randomstuff topics, this also is the level at which
 * users will be able to add RDB topics
 */

#define	RANDOM_STUFF
/**
 * Will read from a random line in
 * dat/randomstuff.ini and say something
 * random in the home channel. ALSO if
 * nothing is said in darkbot's home channel
 * it will say a randomstuff every RAND_IDLE
 * as long as no one says anything..
 */

#define RAND_IDLE	1800
/**
 * Time in secs to say something in home chan
 * when no one says anything (this overrides
 * the RAND_STUFF_TIME counter (default is 10
 * min)
 */

#undef	ANTI_IDLE
/**
 * Want your darkbot to always have less than
 * 10 min idle? This isn't usually needed since
 * darkbots tend to talk a lot.
 */

#define	SLEEP_LEVEL	3
/**
 * Level at which user's can make darkbot
 * shut up (aka hush) This is useful for help
 * channels when they want darkbot to quit
 * talking while they address something
 * important in the chan
 */

#define	SLEEP_TIME	400
/**
 * How many seconds to sleep for?
 */

#define	AUTOTOPIC_TIME	1800
/**
 * Autotopic time in seconds - default 
 * is 30 min
 */

#define	MAX_DATA_SIZE	400
/**
 * The max size your database topics will be
 * This will be useful to people loading
 * their database into ram - if your data is
 * all less than 400 chars, then you can save
 * some ram.... Longer allows you to do more
 * with PIPE topics (see README for 4f3)
 */

#define	MAX_TOPIC_SIZE	50
/**
 * Max topic length
 */

#define	VERB
/**
 * Print out extra details while starting up?
 */

/* Below are variables for the data replys */
#define myVariables "data variables are: N~ (Nick), C~ (Chan), T~ (Time/date) \
B~ (Botnick), Q~ (Question asked), R~ (random nick), !~ (command char), S~ (current Server), P~ (current port) \
V~ (botVer), W~ (db WWW site)"

#define mySetinfo "My !setinfo variables are: ^ nick, % Number of joins, & \
Channel, $ user@host. Example: !setinfo ^ has joined & % times!!  (also, if \
you make the first char of your SETINFO a \"+\" the setinfo will be shown as an ACTION)"

#define	L100(s,a,b,c,d,e,f) S("NOTICE %s :I can be triggered by various forms of speech, all which must \
be addressed to me, in one of the following formats:  %s %s %s or even %s .... In my database, you can \
find a topic by saying my nick, <topic> .  eg; \37%s nuke\37  ..........to do a search on a word, or \
partial text, just type:  <mynick>, search <text>   ... eg; \37%s search nuke\37\n",s,a,b,c,d,e,f)

#define	L101(a,b,c,d) S("NOTICE %s :I can also be triggered with even more human formats: \37%s who \
is bill gates?\37  .. You can also phrase it in a question: \37%s where is msie?\37 ...For more info \
about me, visit http://darkbot.net\n",a,b,c,d)

#define	L102(a,b,c,d) S("NOTICE %s :Welcome to %s, %s. Before asking your question, type %cHELP \
for a list of help topics.\n", a,b,c,d)



/* ################################################## */
/* ---- Don't change anything else below! ----------- */
/* ################################################## */
#define	fr		3	/* these two are the # of lines per secs */
#define	ft		3
#define	AIL		1
#define	WSEC 		10
#define	USEC 		0
#define	RECHECK 	45
#define DBTIMERS_PATH	"timers"
#define	LOG_DIR		"logs/"
#define	SEEN_FILE	"dat/seen.db"
#define	URL		"dat/info.db"	/* OLD FORMAT */
#define	URL2		"dat/info2.db"
#define	BACKUP_DUP	"dat/backup_dups.db"
#define	ADD_DELETES	"logs/add_delete.log"
#define	TMP_URL		".info.tmp"
#define	TMP_FILE	".file.tmp"
#define	AUTOTOPIC_F	"dat/autotopic.ini"
#define	HELPER_LIST	"dat/userlist.db"
#define	PERFORM		"dat/perform.ini"
#define	DEOP		"dat/deop.ini"
#define	RAND_SAY	"dat/random.ini"
#define	RAND_FILE	"dat/randomstuff.ini"
#define	SERVERS		"dat/servers.ini"
#define	PERMBAN		"dat/permbans.db"
#define	SETUP		"dat/setup.ini"
#define	MAX_SEARCH_LENGTH	350
/* -------------------------------------------------- */

#include "langs/lang.h"
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#ifdef	WIN32
#include <windows.h>
#endif
#include <signal.h>
#if	DBTIMERS == 1
#include <dirent.h>
#include <sys/stat.h>
#endif

#define db_sleep(x) usleep(x*1000)

#ifndef __cplusplus
typedef enum
{ false, true }
bool;
#endif

#ifdef WIN32
typedef unsigned int size_t;
#endif
