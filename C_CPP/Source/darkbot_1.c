/**
 * Darkbot - Internet Relay Chat Help Robot, darkbot.c
 *
 * Copyright (C) 2000 Jason Hamilton <jason@darkbot.net>
 * http://darkbot.net
 * 
 * Join our mailing list! email darkbot-request@darkbot.net
 * with a subject of "subscribe"

Darkbot Revision: 6a4
Oct 13th, 2000

 */

/*********************************************************
 * ATTENTION: If you are looking for the DEFINES that are
 * usually located here, please be aware as of 6.x versions
 * of darkbot, they are all located in "defines.h". You
 * should not need to edit darkbot.c anymore.
 *********************************************************/

#define usage_agreement "\n\
 This program is provided free for non-commercial use only. Any\n\
 commercial use of this source code or binaries compiled thereof requires\n\
 the prior written consent of the author. Contact jason@superlink.net\n\
 with any questions regarding commercial use. Distriubution of modified\n\
 source code or binaries compiled from modified source code is expressly\n\
 forbidden.\n"

#define ON 1
#define OFF 0
#define R return

#define LEGAL_TEXT "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_-"
#define SAFE_LIST "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#define NUMBER_LIST "1234567890"

#ifndef	LANG			/* If not defined, default to english */
#define	LANG 1
#endif

#include "defines.h"

/* ------------ Below are function declarations --------------- */
#if STATUS == 1
void parse_252 (char *s), parse_251 (char *s), parse_255 (char *s);
#endif

#ifndef WIN32
inline size_t min (const size_t a, const size_t b);
#endif
char *db_strndup (const char *dupMe, size_t maxBytes);
char **tokenize (char *theString, size_t * numTokens);

void show_seen (char *nick, char *source, char *target),
count_seen (char *source, char *target),
show_info2 (const char *target, const char *source);
long save_seen (char *nick, char *uh, char *chan);
void do_randomtopic (char *target, char *file, char *nick,
		     char *topic);
#ifdef	RANDOM_STUFF
void do_random_stuff (), get_rand_stuff_time (),
del_autotopic (const char *chan), add_randomstuff (char *source,
						   char *target,
						   char *data),
do_autotopics ();
#endif
void datasearch (const char *nick, char *topic, char *target),
display_url (char *, char *, char *), set_pass (char *nick, char *uh,
						char *pass,
						char *newpass),
do_modes (char *source, char *data), process_nick (char *nick,
						   char *newnick);
long verify_pass (char *nick, char *chan, char *uh, char *pass),
ifexist_autotopic (char *chan);
#if DO_CHANBOT_CRAP == 1
void save_permbans ();
#endif
void do_quit (const char *nick, long);
#ifdef	DO_MATH_STUFF
void do_math (const char *who, char *target, char *math);
#endif
void parse_who (char *data);
void set_autotopic (char *source, char *target, char *topic);
void delete_user_ram (char *source, char *uh), get_s (),
delete_url (const char *nick, char *topic, char *target),
update_setinfo (const char *new_uh, const char *new_greetz,
		const char *nick);
#if DO_CHANBOT_CRAP == 1
void add_permban (const char *uh, size_t counter, const char *reason);
int del_permban (const char *nick, const char *uh);
#endif
int check_existing_url (const char *source, char *topic,
			char *target);
void show_helper_list (const char *nick, long level),
show_banlist (const char *nick), chanserv (char *source, char *target,
					   char *buf),
raw_now (char *type), find_url (const char *nick, char *topic,
				char *target), save_changes (),
show_url (char *nick, char *topic, char *target, long, long, char *uh,
	  long);
char *strlwr (char *buf), *rand_reply (const char *nick), *date (),
  *get_multiword_topic (char *first), *revert_topic (char *input),
  *get_rand_nick (const char *chan), *get_word (long number,
						char *text);
void info (const char *source, char *target), load_helpers (),
scan_chan_users (char *chan, char *nick, char *banned),
do_login (char *nick, char *pass);
int stricmp (const char *s1, const char *s2);
long do_lastcomm (char *nick, char *target, char *rest),
setinfo_lastcomm (char *rest);
void parse (char *), add_helper (const char *chan, const char *uh,
				 long level, size_t num_join,
				 const char *greetz,
				 const char *pass);
int get_connection (const char *hostname, const char *vhostname,
		    int port), readln (), writeln (const char *);
bool check_permban (const char *uh, const char *chan,
		    const char *nick);
long cf (char *host, char *nick, char *chan), f_f (char *host),
get_passwd (char *data);
time_t return_useridle (const char *chan, const char *who,
			int toggle);
void a_f (char *host), reset_ (), delete_user (const char *nick,
					       char *chan),
add_user (char *chan, char *nick, char *uh, long);
void set_fds (), sig_hup (int), sig_segv (int), save_setup ();
void stripline (char *), init_bot (), sig_alrm (int);
void parse_server_msg (fd_set * read_fds);
void trailing_blanks (char *),
log (const char *, const char *, ...),
gs26 (), add_s25 (char *server, long port),
add_banned_server (char *server, char *reason), S (const char *format,
						   ...),
del_sendq (long), clear_sendq (long, long);
char L[524];
int socketfd, alarmed, check_access (char *uh, char *chan, int toggle,
				     char *nick),
match_wild (const char *pattern, const char *str), Send (),
get_sendq_count (long);
#if DBTIMERS == 1
void check_dbtimers ();
#endif

/* ------------ Below are global vars -------------- */
long QUESTIONS = 0, ADDITIONS = 0, DELETIONS = 0, uptime, NO_FLOOD,
  NUM_SERV = 0, L_CLIENTS = 0, IRCOPS = 0, xtried = 0, G_USERS =
  0, rt = 120, fc, spr = 0, snr = 0, BP = 6667, CHECKED =
  1, SEND_DELAY = 1, send_tog = 0, NUM_HELPER = 0, NUMLINESSEEN = 0;
#ifdef	RANDOM_STUFF
long Rand_Stuff = 0, Rand_Idle = 0;
#endif
long AIL4 = 0, Sleep_Toggle = 0, AIL3 = 0, AIL2 = 0, AIL5 = 0,
  JOINs = 0, PERMBAN_counter = 0, ram_load_time = 0, AIL9 =
  0, AIL666 = 0, AIL8 = 0, LastInput = 0, AIL10 = 0, MARK_CHANGE =
  0, html_counter = 0;
char NICK_COMMA[32], COLON_NICK[33], pass_data[512], pass_pass[100];
char lc1[500] = "0", lc2[500] = "0", lc4[500] = "0", lc3[500] =
  "0", MasK[512];
long lcn1 = 0, lcn2 = 0, lcn4 = 0, lcn3 = 0, SeeN = 0, DebuG = 0;
char slc1[500] = "0", slc2[500] = "0", slc4[500] = "0", slc3[500] =
  "0";
long slcn1 = 0, slcn2 = 0, slcn4 = 0, slcn3 = 0;
#ifdef	WIN32
char *rp391 = "niW-4a6tobkraD";
#else
char *rp391 = "4a6tobkraD";
#endif
char BCOLON_NICK[36], DARKBOT_BIN[200] = "", r_reply[512] = "",
  data[512] = "", g_chan[500], dbVersion[13], strbuff[70],
  f_tmp[2048], UID[20] = "database", BS[100] = "204.127.145.17",
  CMDCHAR[2] = "!", CHAN[300] = "#irc_help", s_Mynick[30] =
  "bot", g_host[500], Mynick[30] =
  "bot", sleep_chan[200], VHOST[512] = "0", REALNAME[200] =
  "http://darkbot.net", privmsg_log[100];
#define PBOT "ArchFiend"


/* ------------ Below are structs ------------------ */

struct rusage r_usage;

struct sendq
{
  char data[600];
  struct sendq *next;
}
 *sendqhead = NULL, *sendqtail = NULL;

struct userlist
{				/* internal userlist */
  char chan[200];
  char nick[35];
  char uh[300];
  long level;			/* auth */
  short global;			/* Global user? */
  long idle;
  struct userlist *next;
}
 *userhead = NULL;

	/**
	 * 6/23/00 Dan
	 * - Changed type of num_join to size_t, this should
	 *   be unsigned
	 */
struct helperlist
{
  char chan[100];
  char uh[150];
  char nick[35];
  long level;
  size_t num_join;
  char greetz[90];
  char pass[30];
  struct helperlist *next;
}
 *helperhead = NULL;


	/**
	 * 6/23/00 Dan
	 * - Changed permbanlist to have dynamically allocated
	 *   userhost and reason fields.  
	 * - Added MAXUSERHOST and MAXREASON as the maximum allowed
	 *   user@host and ban reason fields.
	 * - Changed type of counter to size_t, this should be an
	 *   unsigned type.
	 */
#define MAXUSERHOST 150
#define MAXREASON 400

struct permbanlist
{
  char *uh;
  char *reason;
  size_t counter;

  struct permbanlist *next;
}
 *permbanhead = NULL;

struct old
{
  char host[200];
  long time;
  int count;
  int value;
  int kick;
}
ood[200];

struct sl124
{
  char name[100];
  long port;
  struct sl124 *next;
}
 *sh124 = NULL;

/* ------------ Begin function source -------------- */

#ifndef WIN32
const char *
run_program (const char *input)
{
  FILE *read_fp;
  long chars_read;

  read_fp = popen (input, "r");
  if (read_fp != NULL) {
    chars_read = fread (f_tmp, sizeof (char), 2048, read_fp);
    pclose (read_fp);
    if (chars_read > 0) {
      R f_tmp;
    }
    R "No match";
  }
  return NULL;
}
#endif

#if DBTIMERS == 1
void
check_dbtimers ()
{
  DIR *dp;
  long i = 0;
  char filename[500];
  struct dirent *entry;
  struct stat statbuf;
  FILE *fp;
  char b[700], output[700];

  if ((dp = opendir (DBTIMERS_PATH)) == NULL) {
    R;
  }
  while ((entry = readdir (dp)) != NULL) {
    stat (entry->d_name, &statbuf);
    if (S_ISDIR (statbuf.st_mode) && *entry->d_name == '.') {
      continue;			/* it's a dir, ignore it */
    }
    i = time (NULL);
    if (i >= atoi (entry->d_name)) {
      sprintf (filename, "%s/%s", DBTIMERS_PATH, entry->d_name);

      if ((fp = fopen (filename, "r")) == NULL) {
	R;
      }
      while (fgets (b, 700, fp)) {
	stripline (b);
	sprintf (output, "%s\n", b);
	S (output);
      }
      fclose (fp);
      unlink (filename);
    }
  }
  closedir (dp);
}
#endif

char *
get_word (long number, char *string)
{				/* gets a specific word requested */
  long i = 0;
  char *ptr;

  number = number - 49;

  ptr = strtok (string, "+");
  strcpy (f_tmp, ptr);
  if (ptr != NULL) {
    while (ptr != NULL) {
      i++;			/* word number */
      ptr = strtok (NULL, "+");
      if (ptr != NULL) {
	if (i == number) {
	  sprintf (f_tmp, "%s", ptr);
	  R f_tmp;
	}
      }
    }
    R f_tmp;
  }
  else {			/* only one word */
    if (number == 1) {
      R f_tmp;
    }
    else
      R "";			/* no match */
  }
}

char **
tokenize (char *theString, size_t * numTokens)
{
  static char *tokens[256];

  assert (numTokens != NULL && theString != NULL);
  memset (tokens, 0, 256 * sizeof (char *));

  tokens[(*numTokens = 0)] = strtok (theString, " ");
  if (NULL == tokens[0]) {
    /* 0 tokens */
    return tokens;
  }

  while ((tokens[++(*numTokens)] = strtok (NULL, " ")) != NULL) {
    /* NO-OP */ ;
  }
  tokens[*numTokens] = 0;

  return tokens;
}

int
check_access (char *uh, char *chan, int toggle, char *nick)
{
  long i = 0, length = 0, A = 0, X = 0, Y = 0;
  struct helperlist *c;
  struct userlist *c2;
  char temp[700] = "";
  c = helperhead;
  c2 = userhead;
  strlwr (uh);
  if (toggle == 0) {		/* get access level */
    while (c2) {
      if (stricmp (c2->uh, uh) == 0) {
	if (stricmp (c2->chan, chan) == 0) {
	  if (stricmp (c2->nick, nick) == 0) {
	    R c2->level;
	  }
	}
      }
      c2 = c2->next;
    }
    return 0;			/* no matches? */
  }
  else
    while (c != NULL) {
      if (!match_wild (c->uh, uh) == 0) {
	if (*c->pass == '0') {
	  L001 (nick, Mynick);
	  R 0;
	}
	if (c->chan[1] != '*')
	  if (stricmp (c->chan, chan) != 0)
	    R 0;
	c->num_join++;
	if (*c->greetz == '+')
	  A = 1;
	strcpy (data, "");
	length = Y = strlen (c->greetz);
	if (length > 1) {
	  while (length > 0) {
	    length--;
	    X++;
	    if (c->greetz[length] == '^') {
	      i++;
	      sprintf (temp, "%s%s", nick, data);
	    }
	    else if (c->greetz[length] == '%') {
	      i++;
	      sprintf (temp, "%ul%s", c->num_join, data);
	    }
	    else if (c->greetz[length] == '$') {
	      i++;
	      sprintf (temp, "%s%s", uh, data);
	    }
	    else if (c->greetz[length] == '&') {
	      i++;
	      sprintf (temp, "%s%s", chan, data);
	    }
	    else
	      sprintf (temp, "%c%s", c->greetz[length], data);
	    if (X == Y && A == 1)
	      continue;
	    strcpy (data, temp);
	  }			/* While */
#if JOIN_GREET == 1
	  if (i == 0) {
	    if (setinfo_lastcomm (uh) == 0) {
	      S ("PRIVMSG %s :%ld\2!\2\37(\37%s\37)\37\2:\2 %s\n",
		 chan, c->num_join, nick, c->greetz);
	    }
	  }
	  else if (A == 1) {
	    if (setinfo_lastcomm (uh) == 0) {
	      S ("PRIVMSG %s :\1ACTION %s\1\n", chan, data);
	    }
	  }
	  else {
	    if (setinfo_lastcomm (uh) == 0) {
	      S ("PRIVMSG %s :%s\n", chan, data);
	    }
	  }
#endif
	  R c->level;
	}
      }
      c = c->next;
    }
  R 0;
}

void
parse_who (char *data)
{
  char *chan, *nick, *ptr, b[500];
  nick = strtok (data, " ");	/* botnick */
  strcpy (Mynick, nick);
  chan = strtok (NULL, " ");
  ptr = strtok (NULL, " ");
  sprintf (b, "%s@%s", ptr, strtok (NULL, " "));
  nick = strtok (NULL, " ");	/* server */
  nick = strtok (NULL, " ");
  add_user (chan, nick, b, 1);
}

void
delete_user (const char *nick, char *chan)
{
  struct userlist *pNode, *pPrev;

  pNode = userhead;
  pPrev = NULL;

  while (pNode) {
    if (stricmp (pNode->nick, nick) == 0
	&& stricmp (pNode->chan, chan) == 0) {
      save_seen (pNode->nick, pNode->uh, pNode->chan);
      if (pPrev != NULL) {
	pPrev->next = pNode->next;
      }
      else {
	userhead = pNode->next;
      }
      free (pNode);
      pNode = NULL;
      break;
    }
    pPrev = pNode;
    pNode = pNode->next;
  }
}

void
do_modes (char *source, char *data)
{
  char *chan, *mode, *nick, *ptr;
  long PM = 0, j = 0, i = 0;

  chan = strtok (data, " ");
  mode = strtok (NULL, " ");

  if ((ptr = strchr (source, '!')) != NULL)
    *ptr++ = '\0';
  j = strlen (mode);
  i = -1;			/* i needs to start at 0 */
  while (j > 0) {
    j--;
    i++;
    if (mode[i] == '+')
      PM = 1;
    if (mode[i] == '-')
      PM = 0;
    if (mode[i] == 'o') {
      nick = strtok (NULL, " ");
      continue;
    }
    if (mode[i] == 'v') {	/* voice sucks, ignore it */
      nick = strtok (NULL, " ");
      continue;
    }
    if (mode[i] == 'k' || mode[i] == 'b') {
      nick = strtok (NULL, " ");
      if (nick[0] == '*' && nick[1] == '!') {
	nick += 2;
      }
      strlwr (nick);
      if (PM == 1)
	scan_chan_users (chan, source, nick);
      continue;
    }
    if (mode[i] == 'l' && PM == 1) {	/* don't parse if -limit
					 * since no parms */
      nick = strtok (NULL, " ");
      continue;
    }
  }

}

/**
 * do_quit
 *
 * Purpose:
 * 1) delete all instances when a nick matches (nick)
 * 2) delete all users off a given channel
 * 2) delete everything (i.e., when the bot is disconnected from irc)
 *
 * toggle 1 = delete user.
 * toggle 2 = delete chan
 * toggle 3 = everything (when I'm killed).
 */
void
do_quit (const char *nick, long toggle)
{

  struct userlist *pNode = userhead;
  struct userlist *pPrev = NULL;

  if (toggle == 1) {
    /* delete user */
    while (pNode) {
      if (stricmp (pNode->nick, nick) == 0) {
	/* found a match, remove it */
	save_seen (pNode->nick, pNode->uh, pNode->chan);
	if (pPrev != NULL) {
	  pPrev->next = pNode->next;
	  free (pNode);
	  pNode = pPrev->next;
	}
	else {
	  /* first node in the list */
	  userhead = pNode->next;
	  free (pNode);
	  pNode = userhead;
	}
      }
      else {
	/* No match, continue to next node */
	pPrev = pNode;
	pNode = pNode->next;
      }
    }
  }
  else if (toggle == 2) {
    /* delete channel */
    while (pNode) {
      if (stricmp (pNode->chan, nick) == 0) {
	/* found a match, remove it */
	save_seen (pNode->nick, pNode->uh, pNode->chan);
	if (pPrev != NULL) {
	  pPrev->next = pNode->next;
	  free (pNode);
	  pNode = pPrev->next;
	}
	else {
	  /* first node in the list */
	  userhead = pNode->next;
	  free (pNode);
	  pNode = userhead;
	}
      }
      else {
	/* No match, continue to next node */
	pPrev = pNode;
	pNode = pNode->next;
      }
    }
  }
  else if (toggle == 3) {
    struct userlist *tempPtr = userhead;
    while (pNode) {
      tempPtr = pNode->next;
      free (pNode);
      pNode = tempPtr;
    }
  }
}

void
do_login (char *nick, char *pass)
{
  long i = 0, x = 0, D = 0;
  char Data[500] = "", b[500];
  struct userlist *c;
  c = userhead;
  while (c) {
    if (stricmp (nick, c->nick) == 0) {
      x = verify_pass (c->nick, c->chan, c->uh, pass);
      if (x > 0) {
	i++;
	if (c->level == 0 && x >= 2) {
	  /* only if not already authed */
	  S ("MODE %s +ov %s %s\n", c->chan, c->nick, c->nick);
	  D = 1;
	}
	c->level = x;
	sprintf (b, "%s[%d] %s", c->chan, (int) c->level, Data);
	strcpy (Data, b);
      }
    }
    c = c->next;
  }
  if (i != 0) {
    if (!D) {
      S ("NOTICE %s :Already authed on %s\n", nick, Data);
    }
    else
      S ("NOTICE %s :Verified: %s\n", nick, Data);
  }
}

void
process_nick (char *nick, char *newnick)
{
  struct userlist *c;
  c = userhead;
  newnick++;
  while (c) {
    if (stricmp (nick, c->nick) == 0) {
      strcpy (c->nick, newnick);
    }
    c = c->next;
  }
}

void
scan_chan_users (char *chan, char *nick, char *banned)
{
  struct userlist *c;
  c = userhead;
  if (banned[0] == '*' && banned[1] == '!' && banned[2] == '*'
      && banned[3] == '\0') {
    S ("MODE %s -ob %s %s\n", chan, nick, banned);
    R;
  }
#if KICK_ON_BAN == 1
  while (c) {
    if (!match_wild (banned, c->uh) == 0) {
      if (stricmp (c->nick, Mynick) != 0) {
	S ("KICK %s %s :BANNED\n", chan, c->nick);
      }
      else {
	S ("MODE %s -ob %s %s\n", chan, nick, banned);
	R;
      }
    }
    c = c->next;
  }
#endif
}

/**
 * 6/23/00 Dan:
 * - All method arguments are now pointer to const
 * - Return type is now time_t
 * - A for loop is now used instead of a while loop
 */
time_t
return_useridle (const char *chan, const char *who, int toggle)
{				/* toggle=0 is for idle time, toggle=1 is to check if user
				   is in the chan */
  const struct userlist *c = userhead;

  for (; c != NULL; c = c->next) {
    if (!stricmp (who, c->nick) && !stricmp (chan, c->chan)) {
      if (toggle == 1) {
	/* If we only care if user is present or not.. */
	return 1;
      }
      else
	return c->idle;
    }				/* if */
  }				/* for */
  return 0;
}

/**
 * 6/23/00 Dan:
 * - All method arguments are now pointer to const
 * - A for loop is now used instead of a while loop
 */
void
show_chaninfo (const char *nick, const char *chan)
{
  size_t totalUsers = 0, foundUsers = 0;
  const struct userlist *c = userhead;

  for (; c != NULL; c = c->next) {
    ++totalUsers;
    printf ("%d %s %s\n", totalUsers, c->nick, c->chan);
    if (!stricmp (chan, c->chan))
      ++foundUsers;
  }
  S ("PRIVMSG %s :%s, I see %d users in %s (%d users total in ram)\n",
     chan, nick, foundUsers, chan, totalUsers);
}

/**
 * 6/22/00 Dan
 * - Removed srand(), should only be done once, in main()
 * - Changed while to for loop
 */
char *
get_rand_nick (const char *chan)
{
  size_t x = 0;
  size_t i = 0;
  const struct userlist *c = userhead;

/* Iterate through the userlist */
  for (; c != NULL; c = c->next) {
    /* Check if this user is on the channel */
    if (stricmp (chan, c->chan) == 0) {
      strcpy (f_tmp, c->nick);
      i++;
    }
  }

  x = rand () % i + 2;
  i = 0;			/* reinit! */

/* Iterate through the user list again to find the
 * x-th user in the given channel */
  for (c = userhead; c != NULL; c = c->next) {
    if (stricmp (chan, c->chan) == 0) {
      i++;
      if (i == x) {
	if (*c->nick == '0') {
	  return f_tmp;
	}
	strcpy (f_tmp, c->nick);
	return f_tmp;
      }
    }
  }
  return f_tmp;
}

void
add_user (char *chan, char *nick, char *uh, long tog)
{
  /* toggle of 0 means to unidle the client */
  struct userlist *n, *c;
  c = userhead;
  if (strlen (uh) > 399)
    uh[399] = '\0';
  while (c) {			/* don't readd data that already exists */
    if (tog == 0) {
      if (stricmp (c->nick, nick) == 0
	  && stricmp (c->chan, chan) == 0) {
	c->idle = time (NULL);
      }
    }
    if (tog == 1) {
      if (stricmp (c->nick, nick) == 0
	  && stricmp (c->chan, chan) == 0) {
	/* If user is somehow already here, just update his data instead 
	   of readding */
	strcpy (c->chan, chan);
	strcpy (c->uh, uh);
	strlwr (c->uh);
	strcpy (c->nick, nick);
	c->idle = time (NULL);
	c->level = 0;
	R;
      }
    }
    c = c->next;
  }
  if (tog == 0) {
    /* all we wanted to do was unidle, so we can quit now */
    R;
  }
  n = (struct userlist *) malloc (sizeof (struct userlist));
  if (n == NULL) {
    log ("error.log", "AHHH! No ram left! in add_user!\n");
    R;
  }
  memset (n, 0, sizeof (struct userlist));
  if (n != NULL) {
    strcpy (n->chan, chan);
    strcpy (n->uh, uh);
    strlwr (n->uh);
    strcpy (n->nick, nick);
    n->idle = time (NULL);
    n->level = 0;

    n->next = userhead;
    userhead = n;
  }
}

#if DO_CHANBOT_CRAP == 1
/**
 * Remove a permban based on nickname and user@host.
 * 6/23/00 Dan:
 * - Both arguments are now pointers to const data
 * - Added free() for both pNode->uh and pNode->reason now
 *   that the permbanlist has dynamically allocated fields
 * - Changed type of toggle from long to bool
 * - Changed name of toggle variable to (foundBan)
 * - permbanlist pointers are now initialized when declared
 * - Added if statement at end of method, this will only save
 *   the bans if a ban was removed
 */
int
del_permban (const char *nick, const char *uh)
{

  bool foundBan = false;
  struct permbanlist *pNode = permbanhead, *pPrev = 0;

  while (pNode) {
    if (stricmp (pNode->uh, uh) == 0) {
      L002 (nick, PERMBAN_counter, uh);
      PERMBAN_counter--;
      if (pPrev != NULL) {
	pPrev->next = pNode->next;
      }
      else {
	permbanhead = pNode->next;
      }

      free (pNode->uh);
      free (pNode->reason);
      free (pNode);
      foundBan = true;
      pNode = NULL;
      break;
    }
    pPrev = pNode;
    pNode = pNode->next;
  }

  if (foundBan) {
    /* Only need to save bans if ban list has changed */
    save_permbans ();
  }
  return foundBan;
}
#endif

char *
revert_topic (char *input)
{
  char *ptr, b[500];

  ptr = strtok (input, "+");
  strcpy (f_tmp, ptr);
  if (ptr != NULL) {
    while (ptr != NULL) {
      ptr = strtok (NULL, "+");
      if (ptr != NULL) {
	sprintf (b, "%s %s", f_tmp, ptr);
	strcpy (f_tmp, b);
      }
    }
    R f_tmp;
  }
  else
    R f_tmp;
}

char *
get_multiword_topic (char *first)
{
  char *tmp2;

  tmp2 = strtok (NULL, " ");
  if (tmp2 != NULL) {
    sprintf (f_tmp, "%s", first);
    while (tmp2 != NULL) {
      sprintf (f_tmp, "%s+%s", f_tmp, tmp2);
      tmp2 = strtok (NULL, " ");
    }
    R f_tmp;
  }
  else
    R first;
}

/**
 * Delete one or more elements from the sendq
 * 1 = delete all pri/not's
 * 0 = delete first in queue
 * 6/23/00 Dan
 * - Updated to use head and tail pointer queue
 * - All variables now initialized when declared
 * - Optimized the main while loop a bit, reduced amount of code
 */
void
del_sendq (long toggle)
{
  struct sendq *pNode = sendqhead, *pPrev = 0;

  if (NULL == sendqhead) {
    return;
  }

  if (toggle == 0) {
    /* Just delete the head */
    pNode = sendqhead;
    sendqhead = sendqhead->next;

    free (pNode);
  }
  else {
    /* Iterate through the queue and delete each element which is
     * a PRIVMSG or NOTICE
     */
    for (; pNode != NULL; pPrev = pNode, pNode = pNode->next) {
      if (0 == strncmp (pNode->data, "PRI", 3)
	  || 0 == strncmp (pNode->data, "NOT", 3)) {
	/* Found one, let's delete it */
	if (pPrev != NULL) {
	  pPrev->next = pNode->next;
	}
	else {
	  sendqhead = pNode->next;
	}
	free (pNode);
	pNode = NULL;
	break;
      }
    }				/* for */
  }				/* else */

/* Update the tail pointer if needed */
  if (NULL == sendqhead) {
    sendqtail = NULL;
  }
}

/**
 * 6/23/00 Dan
 * - Changed method argument to be pointer to const data
 * - Initialized b
 */
int
Snow (const char *format, ...)
{
  va_list arglist;
  char b[1024] = { 0 };

  va_start (arglist, format);
  vsprintf (b, format, arglist);
  va_end (arglist);
  if (DebuG == 1)
    printf ("OUT: %s\n", b);
  R (writeln (b));
}

/**
 * 6/23/00 Dan
 * - Changed method argument to be pointer to const data
 * - Initialized variables when declared
 * - Changed b to a power of 2
 */
void
S (const char *format, ...)
{
  va_list arglist;
  char b[1024] = { 0 };
  struct sendq *n = 0;

  va_start (arglist, format);
  vsprintf (b, format, arglist);
  va_end (arglist);

  if (send_tog == 0) {
    send_tog = 1;
    if (DebuG == 1) {
      printf ("OUT: %s\n", b);
    }
    writeln (b);
    R;
  }

  n = (struct sendq *) malloc (sizeof (struct sendq));
  if (n == NULL) {
    log ("error.log", "AHH! no ram left! in S!\n");
    R;
  }

  memset (n, 0, sizeof (struct sendq));
  strcpy (n->data, b);

  if (sendqhead == NULL) {
    sendqhead = sendqtail = n;
  }
  else {
    sendqtail->next = n;
    sendqtail = sendqtail->next;
  }
}

void
count_seen (char *source, char *target)
{
  FILE *fp;
  char temp[700] = "";
  long i = 0;
  if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
    L003 (source, SEEN_FILE);
    R;
  }
  while (fgets (temp, 700, fp))
    i++;
  fclose (fp);
  L004 (target, source, i);
}

void
show_seen (char *nick, char *source, char *target)
{
  FILE *fp;
  char temp[700] = "", *intime, *r_nick, *uh, *chan, *ptr;
  long unixtime = 0;

  if (nick == NULL)
    R;
  if (strlen (nick) > 30)
    R;
  if (stricmp (nick, source) == 0) {
    L005 (target, source);
    R;
  }
  if ((ptr = strchr (nick, '?')) != NULL)
    memmove (ptr, ptr + 1, strlen (ptr + 1) + 1);
  if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
    L003 (source, SEEN_FILE);
    R;
  }
  while (fgets (temp, 700, fp)) {
    stripline (temp);
    r_nick = strtok (temp, " ");
    if (stricmp (nick, r_nick) == 0) {
      uh = strtok (NULL, " ");
      chan = strtok (NULL, " ");
      if (uh == NULL || chan == NULL)
	continue;
      intime = strtok (NULL, " ");
      if (intime == NULL)
	continue;
      unixtime = time (NULL) - atoi (intime);
      if (unixtime > 86400)
	S
	  ("PRIVMSG %s :%s, I last saw %s (%s) %d day%s, %02d:%02d ago in %s\n",
	   target, source, r_nick, uh, unixtime / 86400,
	   (unixtime / 86400 == 1) ? "" : "s", (unixtime / 3600) % 24,
	   (unixtime / 60) % 60, chan);
      else if (unixtime > 3600)
	S
	  ("PRIVMSG %s :%s, I last saw %s (%s) %d hour%s, %d min%s ago in %s\n",
	   target, source, r_nick, uh, unixtime / 3600,
	   unixtime / 3600 == 1 ? "" : "s", (unixtime / 60) % 60,
	   (unixtime / 60) % 60 == 1 ? "" : "s", chan);
      else
	S
	  ("PRIVMSG %s :%s, I last saw %s (%s) %d minute%s, %d sec%s ago in %s\n",
	   target, source, r_nick, uh, unixtime / 60,
	   unixtime / 60 == 1 ? "" : "s", unixtime % 60,
	   unixtime % 60 == 1 ? "" : "s", chan);
      fclose (fp);
      R;
    }
  }
  fclose (fp);
  L006 (target, source, nick, SEEN_REPLY);
}

long
save_seen (char *nick, char *uh, char *chan)
{
  FILE *fp;
  char temp[700] = "", *r_nick, *r_chan, *r_uh, *r_time;
  long toggle = 0, unixtime = 0;

#ifdef	WIN32
  printf ("*** Writing seen file: %s (%s)\n", CHAN, SEEN_FILE,
	  date ());
#endif
  unlink (TMP_FILE);
  if ((fp = fopen (SEEN_FILE, "r")) == NULL) {
    log (SEEN_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
    R - 1;
  }
  while (fgets (temp, 512, fp)) {
    stripline (temp);
    r_nick = strtok (temp, " ");
    if (stricmp (nick, r_nick) == 0) {
      toggle = 1;
      log (TMP_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
    }
    else {
      r_uh = strtok (NULL, " ");
      r_chan = strtok (NULL, " ");
      r_time = strtok (NULL, " ");
      if (r_uh == NULL || r_chan == NULL || r_time == NULL)
	continue;
      unixtime = time (NULL) - atoi (r_time);
      if (unixtime < MAX_LASTSEEN)
	log (TMP_FILE, "%s %s %s %s\n", r_nick, r_uh, r_chan, r_time);
    }
  }
  fclose (fp);
  if (toggle == 0) {
    log (TMP_FILE, "%s %s %s %d\n", nick, uh, chan, time (NULL));
  }
  rename (TMP_FILE, SEEN_FILE);
  return toggle;
}

#if DO_CHANBOT_CRAP == 1
/**
 * Save the permban list to file.
 * 6/23/00 Dan:
 * - the permbanlist pointer (c) is now pointer to const, because
 *   this is a read only method, and that is a read only variable
 * - Initialized c when it is declared
 */
void
save_permbans ()
{

  const struct permbanlist *c = permbanhead;

  unlink (TMP_FILE);

#ifdef	WIN32
  printf ("*** Writing permbans: %s (%s)\n", PERMBAN, date ());
#endif

  for (; c != NULL; c = c->next) {
    log (TMP_FILE, "%s %d %s\n", c->uh, c->counter, c->reason);
  }
  rename (TMP_FILE, PERMBAN);

  if (PERMBAN_counter == 0)
    unlink (PERMBAN);
}
#endif

/**
 * Remove the autotopic for a particular channel.
 * 6/23/00 Dan:
 * - Method argument is now pointer to const data
 * - All variables are now initialized when declared
 * - Changed size of b to be power of 2
 * - Changed variable types of toggle and x in accordance
 *   with their use
 */
void
del_autotopic (const char *chan)
{
  FILE *fp = 0;
  char b[512] = { 0 }, *r_chan = 0, *r_data = 0;
  bool toggle = false;
  size_t x = 0;

  unlink (TMP_FILE);
  fp = fopen (AUTOTOPIC_F, "r");
  if (NULL == fp) {
    return;
  }

  while (fgets (b, 512, fp)) {
    x++;
    stripline (b);

    r_chan = strtok (b, " ");
    r_data = strtok (NULL, "");

    if (stricmp (r_chan, chan) == 0) {
      /* Found the channel */
      toggle = true;
    }
    else {
      log (TMP_FILE, "%s %s\n", r_chan, r_data);
    }
  }

  fclose (fp);
  if (x == 1 && toggle) {
    /* The autotopic file is now empty */
    unlink (AUTOTOPIC_F);
    unlink (TMP_FILE);
    R;
  }

  if (toggle) {
    /* We found the topic, change the temp file to the
     * the name of the autotopic file */
    rename (TMP_FILE, AUTOTOPIC_F);
  }
  else {
    /* We were unable to find the channel, just
     * return */
    unlink (TMP_FILE);
  }
}

void
do_autotopics ()
{
  FILE *fp;
  char b[520], *r_chan, *r_data;

  if ((fp = fopen (AUTOTOPIC_F, "r")) == NULL)
    R;
  while (fgets (b, 512, fp)) {
    stripline (b);
    r_chan = strtok (b, " ");
    r_data = strtok (NULL, "");
    if (*r_data != '0') {
      db_sleep (1);
      S ("TOPIC %s :%s\n", r_chan, r_data);
    }
  }
  fclose (fp);
}

long
ifexist_autotopic (char *chan)
{
  FILE *fp;
  char b[520], *r_chan;

  if ((fp = fopen (AUTOTOPIC_F, "r")) == NULL)
    R - 1;
  while (fgets (b, 512, fp)) {
    stripline (b);
    if (*b == '/')
      continue;
    r_chan = strtok (b, " ");
    if (stricmp (r_chan, chan) == 0) {
      fclose (fp);
      R 1;			/* exists */
    }
  }
  fclose (fp);
  R 0;				/* doesn't exist */
}

void
set_autotopic (char *source, char *target, char *topic)
{
  long exist = 0;

  exist = ifexist_autotopic (target);
  if (exist == 0 && *topic == '0') {	/* never existed, lets humor
					 * the guy */
    L007 (source, target);
    R;
  }
  else if (exist == 1 && *topic == '0') {	/* delete it! */
    L008 (source, target);
    S ("TOPIC %s :\n", target);
    del_autotopic (target);
    R;
  }
  if (strlen (topic) >= 400)	/* make sure no overflow */
    topic[400] = '\0';
  if (exist == 0) {		/* no such autotopic, so add it */
    L009 (source, target, topic);
    log (AUTOTOPIC_F, "%s %s\n", target, topic);
    R;
  }
  /* only thing left is if topic exists and you want to update it */
  del_autotopic (target);
  L010 (source, target, topic);
  S ("TOPIC %s :%s\n", target, topic);
  log (AUTOTOPIC_F, "%s %s\n", target, topic);
}

#ifdef	RANDOM_STUFF
void
add_randomstuff (char *source, char *target, char *data)
{
  FILE *fp;
  char b[520], file2[512], *ptr;
  long i = 0, x = 0, TOG = 0;

  if (*data == '~') {
    data++;
    TOG = 1;
    ptr = strtok (data, " ");
    if (ptr != NULL)
      strlwr (ptr);

    if (strspn (ptr, LEGAL_TEXT) != strlen (ptr)) {
      S
	("PRIVMSG %s :%s, rdb file must be made up of letters and or numbers, no other text is accepted.\n",
	 target, source);
      R;
    }
    sprintf (file2, "dat/%s.rdb", ptr);
    data = strtok (NULL, "");
  }
  else
    sprintf (file2, "%s", RAND_FILE);
  if ((fp = fopen (file2, "r")) == NULL) {
    if (TOG == 1) {
      log (file2, "1\n%s\n", data);
      S ("PRIVMSG %s :Done, there is 1 topic under %s\n", target,
	 file2);
    }
    R;
  }
  unlink (TMP_FILE);
  while (fgets (b, 512, fp)) {
    stripline (b);
    i++;
    if (*b == '/')
      continue;
    if (i == 1) {
      if (b != NULL) {
	x = atoi (strtok (b, " "));
	log (TMP_FILE, "%d\n", x + 1);
      }
    }
    else {
      log (TMP_FILE, "%s\n", b);
    }
  }
  log (TMP_FILE, "%s\n", data);
  L011 (target, source, i);
  fclose (fp);
  rename (TMP_FILE, file2);
}
#endif

#ifdef	RANDOM_STUFF
/**
 * 6/22/00 Dan
 * - Removed srand(), should only be done once, in main()
 * - Removed an unused variable
 */
void
get_rand_stuff_time ()
{
  Rand_Stuff = rand () % RAND_STUFF_TIME + 2;
  if (Rand_Stuff < RAND_STUFF_TIME / 2)
    Rand_Stuff = RAND_STUFF_TIME;
}
#endif

#ifdef	RANDOM_STUFF
/**
 * 6/23/00 Dan:
 * - Removed an unused variable
 * - Changed initialization of temp
 * - Changed size of b to be power of 2, and initialized
 * - Initialized all other variables
 */
void
do_random_stuff ()
{
  FILE *fp = 0;
  char temp[512] = { 0 }, b[512] =
  {
  0}
  , *b2 = 0;
  bool A = false;
  size_t i = 0;
  size_t length = 0;
  size_t x = 0, y = 0;

  fp = fopen (RAND_FILE, "r");
  if (NULL == fp) {
    return;
  }

  while (fgets (b, 512, fp)) {
    if (*b == '/') {
      continue;
    }
    i++;
    stripline (b);

    if (i == 1) {
      y = atoi (b);
      x = rand () % y + 2;
    }
    if (i == x) {
      if (*b == '+') {
	A = true;
      }
      length = strlen (b);
      i = 0;

      memset (data, 0, sizeof (data));
      while (length > 0) {
	i++;
	length--;
	if (b[length] == '~') {
	  /* $chan */
	  sprintf (temp, "%s%s", CHAN, data);
	}
	else {
	  sprintf (temp, "%c%s", b[length], data);
	}
	strcpy (data, temp);
      }
      if (!A) {
	S ("PRIVMSG %s :%s\n", CHAN, data);
	fclose (fp);
	R;
      }
      else {
	b2 = data;
	b2++;
	S ("PRIVMSG %s :\1ACTION %s\1\n", CHAN, b2);
	fclose (fp);
	R;
      }
    }
  }				/* while() */

/* fclose(fp); */
}
#endif


void
do_randomtopic (char *target, char *file, char *nick, char *topic)
{
  FILE *fp = 0;

  char temp[512] = { 0 };
  char b2[512] = { 0 };
  char *b = 0;
  char file2[512] = { 0 };
  char Data[700] = { 0 };

  long x = 0;
  long y = 0;
  long A = 0;

  size_t i = 0;
  size_t length = 0;

  bool Tog = false;

  if (file != NULL)
    sprintf (file2, "dat/%s.rdb", file);
  if ((fp = fopen (file2, "r")) == NULL) {
    S ("PRIVMSG %s :Sorry, I cannot answer that topic because "
       "darkbot random text file (rdb) \"%s\" was not found.\n",
       target, file2);
    R;
  }
  db_sleep (1);
  while (fgets (b2, 512, fp)) {
    i++;
    stripline (b2);
    if (i == 1) {
      y = atoi (b2);
      x = rand () % y + 2;
    }
    if (i == x) {
      i = 0;
      b = b2;
      if (*b == '+') {
	b++;
	A = 1;
      }
      length = strlen (b);
      while (length > 0) {
	length--;
	if (Tog) {
	  Tog = false;
	  if (b[length] == 'N') {	/* nick */
	    sprintf (temp, "%s%s", nick, Data);
	  }
	  else if (b[length] == 'C') {	/* chan */
	    sprintf (temp, "%s%s", target, Data);
	  }
	  else if (b[length] == 'T') {	/* time */
	    sprintf (temp, "%s%s", date (), Data);
	  }
	  else if (b[length] == 'R') {	/* rand */
	    sprintf (temp, "%s%s", get_rand_nick (target), Data);
	  }
	  else if (b[length] == 'S') {	/* serv */
	    sprintf (temp, "%s%s", BS, Data);
	  }
	  else if (b[length] == 'P') {	/* port */
	    sprintf (temp, "%d%s", (int) BP, Data);
	  }
	  else if (b[length] == 'Q') {	/* question */
	    sprintf (temp, "%s%s", revert_topic (topic), Data);
	  }
	  else if (b[length] == 'W') {	/* WWW page */
	    sprintf (temp, "http://darkbot.net%s", Data);
	  }
	  else if (b[length] == '!') {	/* cmdchar */
	    sprintf (temp, "%c%s", *CMDCHAR, Data);
	  }
	  else if (b[length] == 'V') {	/* version */
	    sprintf (temp, "%s%s", dbVersion, Data);
	  }
	  else if (b[length] == 'B') {	/* mynick */
	    sprintf (temp, "%s%s", Mynick, Data);
	  }
	  else {
	    sprintf (temp, "%c~%s", b[length], Data);
	  }
	}
	else if (b[length] == '~') {
	  Tog = true;
	}
	else {
	  sprintf (temp, "%c%s", b[length], Data);
	}
	strcpy (Data, temp);
      }				/* While */

      if (A == 0) {
	S ("PRIVMSG %s :%s\n", target, Data);
      }
      else {
	S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
      }
      fclose (fp);
      R;
    }
  }
  fclose (fp);
}


/**
 * Add a permban to the permban list.
 * 6/23/00 Dan:
 * - Both pointer variables are now received as pointer to const data
 * - Changed counter to type size_t, this should be an unsigned type
 * - Initialiazed n to 0 on declaration
 * - Added support for dynamically allocated uh and reason fields
 *   in the struct permban list
 * - Did some extra memory leak prevention
 */
void
add_permban (const char *uh, size_t counter, const char *reason)
{

  struct permbanlist *n = 0;

  n = (struct permbanlist *) malloc (sizeof (struct permbanlist));
  if (n == NULL) {
    log ("error.log", "AHHH! no ram left! in add_permban!\n");
    R;
  }

  memset (n, 0, sizeof (struct permbanlist));

  n->uh = db_strndup (uh, MAXUSERHOST);
  if (NULL == n->uh) {
    log ("error.log", "add_permban> Memory allocation failure\n");

    /* Prevent memory leaks */
    free (n);
    return;
  }

  n->reason = db_strndup (reason, MAXREASON);
  if (NULL == n->reason) {
    log ("error.log", "add_permban> Memory allocation failure\n");

    /* Prevent memory leaks */
    free (n->uh);
    free (n);

    return;
  }

  strlwr (n->uh);
  n->counter = counter;
  PERMBAN_counter++;

  n->next = permbanhead;
  permbanhead = n;
}

/**
 * Check if a permban exists for a given uh/channel/nick set.
 * 6/23/00 Dan:
 * - Changed all method arguments to be pointers to const data
 * - Return type is now bool, returns true if ban is found,
 *   false otherwise
 */
bool
check_permban (const char *uh, const char *chan, const char *nick)
{
  static char tmpBuf[MAXUSERHOST + 1];
  struct permbanlist *c = permbanhead;

  strncpy (tmpBuf, uh, min (MAXUSERHOST, strlen (uh)));
  strlwr (tmpBuf);

  for (; c != NULL; c = c->next) {
    if (!match_wild (c->uh, tmpBuf) == 0) {
      c->counter++;
      S ("MODE %s +b %s\n", chan, c->uh);
      S ("KICK %s %s :\2[\2%d\2]\2: %s\n", chan, nick, c->counter,
	 c->reason);
      R true;
    }
  }
  R false;
}

#ifndef WIN32
size_t min (const size_t a, const size_t b)
{
  return ((a < b) ? a : b);
}
#endif

long
get_pass (char *data)
{
  /* returns 0 for no data */
  /* returns 1 for just pass */
  /* returns 2 for pass and data */
  char b[512], b2[512], *temp;
  long i = 0;

  strcpy (pass_data, "0");	/* init */
  strcpy (pass_pass, "0");
  if (data == NULL)
    R 0;
  strcpy (b2, data);
  temp = strtok (data, " ");
  if (temp == NULL)
    R - 1;
  strcpy (b, temp);
  while (temp != NULL) {
    i++;
    strcpy (pass_pass, temp);
    temp = strtok (NULL, " ");
    if (temp == NULL)
      break;
    sprintf (b, "%s %s", b, temp);
  }
  strcpy (b, "");		/* reinit */
  temp = strtok (b2, " ");
  strcpy (b, temp);
  while (i > 2) {
    i--;
    temp = strtok (NULL, " ");
    sprintf (b, "%s %s", b, temp);
  }
  if (stricmp (b, pass_pass) == 0) {
    strcpy (pass_data, "0");
    R 1;
  }
  strcpy (pass_data, b);
  R 2;
}

void
set_pass (char *nick, char *uh, char *pass, char *newpass)
{
  struct helperlist *c;
  c = helperhead;
  strlwr (uh);
  while (c) {
    if (!match_wild (c->uh, uh) == 0) {
      if (strcmp (c->pass, pass) == 0) {
	strcpy (c->pass, newpass);
	L012 (nick, uh);
	save_changes ();
	R;
      }
      else {
	L013 (nick);
	R;
      }
    }
    c = c->next;
  }
  L014 (nick);
}

long
verify_pass (char *nick, char *chan, char *uh, char *pass)
{
  struct helperlist *c;
  c = helperhead;
  strlwr (uh);
  while (c) {
    if (!match_wild (c->uh, uh) == 0) {
      if (*c->pass == '0')
	R 0;			/* no pass set */
      if (strcmp (c->pass, pass) == 0) {
	if (c->chan[0] == '#' && c->chan[1] == '*')
	  R c->level;
	if (*chan == '*')
	  R c->level;
	if (stricmp (c->chan, chan) == 0)
	  R c->level;
	R 0;			/* don't match chan access */
      }
    }
    c = c->next;
  }
  R 0;
}

void
delete_user_ram (char *source, char *uh)
{
  struct helperlist *pNode, *pPrev;

  pNode = helperhead;
  pPrev = NULL;
  while (pNode) {
    if (stricmp (pNode->uh, uh) == 0) {
      L015 (source, pNode->uh, pNode->level, pNode->num_join);
      if (pPrev != NULL) {
	pPrev->next = pNode->next;
      }
      else
	helperhead = pNode->next;
      free (pNode);
      pNode = NULL;
      break;
    }
    pPrev = pNode;
    pNode = pNode->next;
  }
  save_changes ();
}

#ifdef	DO_MATH_STUFF
void
do_math (const char *who, char *target, char *math)
{
  char input[556];
  char number_string[30];
  char op = 0;

  unsigned int index = 0;
  unsigned int to = 0;
  unsigned int input_length = 0;
  unsigned int number_length = 0;
  double result = 0.0;
  double number = 0.0;

  strcpy (input, math);
  input_length = strlen (input);

  for (to = 0, index = 0; index <= input_length; index++)
    if (*(input + index) != ' ')
      *(input + to++) = *(input + index);
  input_length = strlen (input);
  index = 0;
  if (input[index] == '=')
    index++;
  else {
    number_length = 0;
    if (input[index] == '+' || input[index] == '-')
      *(number_string + number_length++) = *(input + index++);
    for (; isdigit (*(input + index)); index++)
      *(number_string + number_length++) = *(input + index);
    if (*(input + index) == '.') {
      *(number_string + number_length++) = *(input + index++);
      for (; isdigit (*(input + index)); index++)
	*(number_string + number_length++) = *(input + index);
    }
    *(number_string + number_length) = '\0';
    if (number_length > 0)
      result = atof (number_string);
  }
  for (; index < input_length;) {
    op = *(input + index++);
    number_length = 0;
    if (input[index] == '+' || input[index] == '-')
      *(number_string + number_length++) = *(input + index++);
    for (; isdigit (*(input + index)); index++)
      *(number_string + number_length++) = *(input + index);
    if (*(input + index) == '.') {
      *(number_string + number_length++) = *(input + index++);
      for (; isdigit (*(input + index)); index++)
	*(number_string + number_length++) = *(input + index);
    }
    *(number_string + number_length) = '\0';
    number = atof (number_string);
    switch (op) {
    case '+':
      result += number;
      break;
    case '-':
      result -= number;
      break;
    case '*':
      result *= number;
      break;
    case '/':
      if (number == 0) {
	L016 (target, who);
	R;
      }
      else
	result /= number;
      break;
    case '%':
      if ((long) number == 0) {
	L016 (target, who);
	R;
      }
      else
	result = (double) ((long) result % (long) number);
      break;
    default:
      L017 (target, who);
      R;
    }
  }
  S ("PRIVMSG %s :%s\2:\2 %f\n", target, who, result);
}
#endif

long
cf (char *host, char *nick, char *chan)
{
  int f_n;
  if (check_access (host, chan, 0, nick) >= 3)
    R 0;
  f_n = f_f (host);
  if (f_n == -1) {
    a_f (host);
    R 0;
  }
  if (ood[f_n].value)
    R 1;
  ood[f_n].count++;
  if ((time (NULL) - ood[f_n].time) > ft)
    ood[f_n].count = 0;
  else if ((time (NULL) - ood[f_n].time) <= ft
	   && ood[f_n].count >= fr) {
    ood[f_n].value = true;
    if (!ood[f_n].kick) {
      ood[f_n].kick = 1;
#ifdef	FLOOD_KICK
      if (*chan == '#' || *chan == '&') {
	L018 (chan, nick, FLOOD_REASON, fc, host);
      }
      else
	L019 (CHAN, fc, host);
#else
      if (*chan == '#' || *chan == '&') {
	L019 (CHAN, fc, host);
      }
      else
	L019 (CHAN, fc, host);
#endif
    }
    R 1;
  }
  ood[f_n].time = time (NULL);
  R 0;
}

/**
 * 6/23/00 Dan:
 * - Initialized all variables
 */
void
raw_now (char *type)
{
  FILE *fp = 0;
  long i = 0, counter = 0;
  char str[1024] = { 0 }, *dat = 0, *ptr = 0,	/* max length per line */
   *tmp1 = 0, *tmp2 = 0, *tmp3 = 0;
  if (stricmp (type, "PERFORM") == 0)
    if ((fp = fopen (PERFORM, "r")) == NULL)
      R;
  if (stricmp (type, "PERMBAN") == 0)
    if ((fp = fopen (PERMBAN, "r")) == NULL)
      R;
  if (stricmp (type, "DEOP") == 0)
    if ((fp = fopen (DEOP, "r")) == NULL)
      R;
  if (stricmp (type, "SERVERS") == 0)
    if ((fp = fopen (SERVERS, "r")) == NULL) {
      printf ("%s not found. You must create the file with format:\n",
	      SERVERS);
      printf
	("server port ...this list can be as long as you want.\n");
      exit (0);
    }
  if (stricmp (type, "SETUP") == 0)
    if ((fp = fopen (SETUP, "r")) == NULL) {
      printf ("Unable to locate %s! You must run configure!.\n",
	      SETUP);
      exit (0);
    }
  while (!feof (fp)) {
    if (stricmp (type, "SETUP") == 0) {
      printf ("Loading %s data...\n", SETUP);
      SeeN = 1;
      while (fgets (str, 1024, fp)) {
	if (*str == '/')
	  continue;
	stripline (str);
	dat = strtok (str, "");
	if ((ptr = strchr (dat, '=')) != NULL)
	  *ptr++ = '\0';
	if (stricmp (dat, "NICK") == 0) {
	  strcpy (Mynick, ptr);
	  strcpy (s_Mynick, ptr);
#if	LOG_PRIVMSG == 1
	  sprintf (privmsg_log, "%s%s-privmsg.log", LOG_DIR, Mynick);
#endif
	}
	else if (stricmp (dat, "USERID") == 0) {
	  strcpy (UID, ptr);
	}
	else if (stricmp (dat, "CHAN") == 0) {
	  strcpy (CHAN, ptr);
	}
	else if (stricmp (dat, "SEEN") == 0) {
	  SeeN = atoi (ptr);
	}
	else if (stricmp (dat, "VHOST") == 0) {
	  strcpy (VHOST, ptr);
	}
	else if (stricmp (dat, "REALNAME") == 0) {
	  strcpy (REALNAME, ptr);
	}
	else if (stricmp (dat, "CMDCHAR") == 0) {
	  *CMDCHAR = *ptr;
	}
      }
#ifdef	VERB
      printf ("  ...botnick : %s\n", Mynick);
      printf ("  ....userid : %s\n", UID);
      printf ("  ...channel : %s\n", CHAN);
      printf ("  .....vhost : %s\n", VHOST);
      printf ("  ..realname : %s\n", REALNAME);
      printf ("  ...cmdchar : %c\n", *CMDCHAR);
      printf (" .......seen : %s\n", SeeN == 1 ? "On" : "Off");
#endif
    }
    else if (stricmp (type, "PERMBAN") == 0) {
      while (fgets (str, 1024, fp)) {
	stripline (str);
	tmp1 = strtok (str, " ");
	if (tmp1 == NULL)
	  continue;
	tmp2 = strtok (NULL, " ");
	if (tmp2 == NULL)
	  tmp2 = "0";
	tmp3 = strtok (NULL, "");
	if (tmp3 == NULL)
	  tmp3 = "Permbanned!";
	strlwr (tmp1);
	counter = atoi (tmp2);
	add_permban (tmp1, counter, tmp3);
      }
    }
    else if (stricmp (type, "SERVERS") == 0) {
#ifndef	WIN32
      printf ("Loading server:port from %s file\n", SERVERS);
#endif
      while (fgets (str, 1024, fp)) {
	i++;
	printf (".");
	fflush (stdout);

	stripline (str);
	tmp1 = strtok (str, " ");
	if (tmp1 == NULL) {
	  printf
	    ("Found error in %s! Aboring! please re-run configure!\n",
	     SERVERS);
	  exit (0);
	}
	else
	  tmp2 = strtok (NULL, " ");
	if (tmp2 == NULL) {
	  printf ("%s has no matching port in %s!\n", tmp1, SERVERS);
	  exit (0);
	}
	add_s25 (tmp1, atoi (tmp2));
      }
      printf ("\nLoaded %d server%s.\n", (int) i,
	      (i == 1) ? "" : "s");
    }
    else if (fgets (str, 1024, fp))
      S ("%s\n", str);
  }
  fclose (fp);
}

/**
 * 6/22/00 Dan
 * - Removed srand(), it should only be done once, in main()
 * - Function argument is now pointer to const
 * - Fixed a problem where the file was never closed
 * - All variables are now initialized when declared
 * - Removed an unused variable
 * - Changed long variables to type size_t, they should be
 *   unsigned
 * - Changed reinitialization of data
 * - Moved the big if/else structure to a switch
 */
char *
rand_reply (const char *nick)
{

  FILE *fp = 0;
  char temp[512] = { 0 };
  size_t i = 0, x = 0, y = 0, length = 0;

  fp = fopen (RAND_SAY, "r");
  if (NULL == fp) {
    return 0;
  }

  while (fgets (r_reply, 512, fp)) {
    if (*r_reply == '/') {
      continue;
    }
    i++;
    stripline (r_reply);

    if (i == 1) {
      /* Read in random # from top line of * random.ini */
      y = atoi (r_reply);
      x = rand () % y + 2;
    }
    if (i != x) {
      continue;
    }

    /* Found it */
    fclose (fp);

    length = strlen (r_reply);
    i = 0;
    data[0] = 0;

    while (length > 0) {
      i++;
      length--;

      switch (r_reply[length]) {
      case '^':
	sprintf (temp, "%s%s", nick, data);
	break;
      case '%':
	/* Bold */
	sprintf (temp, "\2%s", data);
	break;
      case '&':
	/* Underline */
	sprintf (temp, "\37%s", data);
	break;
      case '~':
	/* Inverse */
	sprintf (temp, "\26%s", data);
	break;
      default:
	sprintf (temp, "%c%s", r_reply[length], data);
	break;
      }				/* switch */
      strcpy (data, temp);
    }				/* while( length > 0 ) */

    return data;		/* Found random line */

  }				/* while(fgets()) */

/* Unable to find match */
  fclose (fp);

/* A space is returned to prevent crashing */
  return " ";

}

/**
 * Update a nick's channel greeting and user@host.
 * 6/23/00 Dan:
 * - All method arguments are now pointers to const data
 * - Rewrote to use a for loop, and fewer variables
 * - Info is only saved to disk if changes are made
 */
void
update_setinfo (const char *new_uh, const char *new_greetz,
		const char *nick)
{
  struct helperlist *c = helperhead;
  bool madeChange = false;
  size_t i = 0;

  for (; c != NULL; c = c->next) {
    ++i;
    if (!match_wild (c->uh, new_uh) == 0) {
      strcpy (c->greetz, new_greetz);
      strlwr (c->uh);
      L020 (nick, i, c->uh, new_greetz);
      madeChange = true;
    }
  }
  if (madeChange) {
    save_changes ();
  }
}

void
save_setup ()
{
#ifdef	WIN32
  printf ("*** Writing setup file: %s (%s)\n", SETUP, date ());
#endif
  unlink (TMP_FILE);
  log (TMP_FILE, "NICK=%s\n", s_Mynick);
  log (TMP_FILE, "USERID=%s\n", UID);
  log (TMP_FILE, "CHAN=%s\n", CHAN);
  log (TMP_FILE, "VHOST=%s\n", VHOST);
  log (TMP_FILE, "REALNAME=%s\n", REALNAME);
  log (TMP_FILE, "CMDCHAR=%c\n", *CMDCHAR);
  log (TMP_FILE, "SEEN=%d\n", SeeN);
  rename (TMP_FILE, SETUP);
}

void
save_changes ()
{
  long i = 0;
  struct helperlist *c;
  c = helperhead;
  unlink (TMP_FILE);
  while (c != NULL) {
    i++;
    log (TMP_FILE, "%s %s %d %d %s %s\n", c->chan, c->uh, c->level,
	 c->num_join, c->pass, c->greetz);
    c = c->next;
  }
  rename (TMP_FILE, HELPER_LIST);
}

void
datasearch (const char *nick, char *topic, char *target)
{
  FILE *fp;
  long i = 0, FOUND = 0, x = 0;
  char b[700], *dorf, *subj, *ptr2, DATA[500] = "";

  if (strlen (topic) > MAX_TOPIC_SIZE)
    topic[MAX_TOPIC_SIZE] = '\0';
  strlwr (topic);
  if ((fp = fopen (URL2, "r")) == NULL) {
    L003 (nick, URL2);
    R;
  }
  while (fgets (b, 700, fp)) {
    x++;
    stripline (b);
    strlwr (b);
    subj = strtok (b, " ");
    dorf = strtok (NULL, "");
    ptr2 = strstr (dorf, topic);
    if (ptr2 != NULL) {
      i++;
      FOUND = 1;
      sprintf (DATA, "%s %s", DATA, subj);
      if (strlen (DATA) >= MAX_SEARCH_LENGTH)
	break;
    }
  }
  fclose (fp);
  if (FOUND == 0) {
    L021 (target, NO_TOPIC, topic, x);
  }
  else if (i > 19) {
    L022 (target, i, DATA);
  }
  else if (i == 1) {
    L023 (target, nick, DATA);
  }
  else
    L024 (target, i, nick, DATA);
}

/**
 * 6/23/00 Dan:
 * - All variables now initialized when declared
 * - Altered variable types to reflect usage
 */
void
info (const char *source, char *target)
{
  FILE *fp = 0;
  clock_t starttime = 0;
  char b[1024] = { 0 };
  size_t topics = 0, dup = 0;
  time_t t2time = 0, c_uptime = 0;

#ifdef FIND_DUPS
  char *ptr = 0, *subj = 0;
  size_t last = 0, last2 = 0;
#endif

  t2time = time (NULL);
  unlink (TMP_URL);
  starttime = clock ();

  fp = fopen (URL2, "r");
  if (NULL == fp) {
    L003 (source, URL2);
    R;
  }
  while (fgets (b, 700, fp)) {
    topics++;
#ifdef	FIND_DUPS
    stripline (b);
    subj = strtok (b, " ");
    ptr = strtok (NULL, "");
    strlwr (subj);
    if (stricmp (last, subj) == 0) {
      dup++;
#ifdef	SAVE_DUPS
      log (BACKUP_DUP, "%s %s\n", subj, ptr);
#endif
    }
    else {
      log (TMP_URL, "%s %s\n", subj, ptr);
    }
    strcpy (last2, subj);
    last = last2;
#endif
  }

  fclose (fp);
  rename (TMP_URL, URL2);

#ifdef	FIND_DUPS
  if (dup > 0) {
    L025 (target, dup);
  }
#endif
  c_uptime = time (NULL) - uptime;
  topics -= dup;
  if (c_uptime > 86400) {
    L026 (target,
	  dbVersion,
	  topics,
	  c_uptime / 86400,
	  (c_uptime / 86400 == 1) ? "" : "s",
	  (c_uptime / 3600) % 24,
	  (c_uptime / 60) % 60, QUESTIONS, ADDITIONS, DELETIONS,
	  (double) (clock () - starttime) / CLOCKS_PER_SEC,
	  (((double) (clock () - starttime) / CLOCKS_PER_SEC) ==
	   1) ? "" : "s");
  }
  else if (c_uptime > 3600) {
    L027 (target,
	  dbVersion,
	  topics,
	  c_uptime / 3600,
	  c_uptime / 3600 == 1 ? "" : "s",
	  (c_uptime / 60) % 60, (c_uptime / 60) % 60 == 1 ? "" : "s",
	  QUESTIONS, ADDITIONS, DELETIONS,
	  (double) (clock () - starttime) / CLOCKS_PER_SEC,
	  (((double) (clock () - starttime) / CLOCKS_PER_SEC) ==
	   1) ? "" : "s");
  }
  else {
    L028 (target,
	  dbVersion,
	  topics,
	  c_uptime / 60,
	  c_uptime / 60 == 1 ? "" : "s",
	  c_uptime % 60,
	  c_uptime % 60 == 1 ? "" : "s", QUESTIONS, ADDITIONS,
	  DELETIONS, (double) (clock () - starttime) / CLOCKS_PER_SEC,
	  (((double) (clock () - starttime) / CLOCKS_PER_SEC) ==
	   1) ? "" : "s");
  }
}

int
check_existing_url (const char *source, char *topic, char *target)
{
  FILE *fp;
  char b[700], *subj;

  if ((fp = fopen (URL2, "r")) == NULL) {
    L003 (source, URL2);
    R 0;
  }
  while (fgets (b, 700, fp)) {
    stripline (b);
    subj = strtok (b, " ");
    if (stricmp (subj, topic) == 0) {
      fclose (fp);
      R 1;
    }
  }
  fclose (fp);
  R 0;
}

void
find_url (const char *nick, char *topic, char *target)
{
  FILE *fp;
  long i = 0, FOUND = 0, x = 0;
  char b[700], *subj, *ptr2, DATA[500] = "";

  if (strlen (topic) > MAX_TOPIC_SIZE)
    topic[MAX_TOPIC_SIZE] = '\0';
  strlwr (topic);
  if ((fp = fopen (URL2, "r")) == NULL) {
    L003 (nick, URL2);
    R;
  }
  while (fgets (b, 700, fp)) {
    x++;
    stripline (b);
    subj = strtok (b, " ");
    strlwr (subj);
    ptr2 = strstr (subj, topic);
    if (ptr2 != NULL) {
      i++;
      FOUND = 1;
      sprintf (DATA, "%s %s", DATA, subj);
      if (strlen (DATA) >= MAX_SEARCH_LENGTH)
	break;
    }
  }
  fclose (fp);
  if (FOUND == 0) {
    L021 (target, NO_TOPIC, topic, x);
  }
  else if (i > 19) {
    L022 (target, i, DATA);
  }
  else if (i == 1) {
    L023 (target, nick, DATA);
  }
  else
    L024 (target, i, nick, DATA);
}

void
display_url (char *target, char *nick, char *topic)
{
  FILE *fp;
  long x = 0;
  char b[700], *subj, *ptr;

  strlwr (topic);
  if ((fp = fopen (URL2, "r")) == NULL) {
    R;
  }
  while (fgets (b, 700, fp)) {
    x++;
    stripline (b);
    subj = strtok (b, " ");
    ptr = strtok (NULL, "");
    if (stricmp (subj, topic) == 0 || !match_wild (subj, topic) == 0) {
      QUESTIONS++;
      S ("PRIVMSG %s :Raw data for %s is: %s\n", target, topic, ptr);
      fclose (fp);
      R;
    }				/* Subject match */
  }
  fclose (fp);
  S ("PRIVMSG %s :%s, I do not know of any topic named %s\n", target,
     nick, topic);
}

void
delete_url (const char *nick, char *topic, char *target)
{
  FILE *fp;
  long i = 0, FOUND = 0;
  char b[700], *subj, *ptr, DATA[512] = "";


  if (*topic == '~') {
    topic++;
    if (topic != NULL)
      strlwr (topic);
    sprintf (DATA, "dat/%s.rdb", topic);

    if (strspn (topic, LEGAL_TEXT) != strlen (topic)) {
      S
	("PRIVMSG %s :%s, rdb files are made up of letters and or numbers, no other text is accepted.\n",
	 target, nick);
      R;
    }

    if ((fp = fopen (DATA, "r")) == NULL) {
      S ("PRIVMSG %s :%s, %s.rdb does not exist.\n", target, nick,
	 topic);
      R;
    }
    fclose (fp);
    unlink (DATA);
    S ("PRIVMSG %s :I have unlinked %s.\n", target, DATA);
    R;
  }

  if ((fp = fopen (URL2, "r")) == NULL) {
    L003 (nick, URL2);
    R;
  }
  unlink (TMP_URL);
  while (fgets (b, 700, fp)) {
    stripline (b);
    subj = strtok (b, " ");
    ptr = strtok (NULL, "");
    i++;
    if (stricmp (subj, topic) == 0) {
      FOUND = 1;
      DELETIONS++;
      L029 (target, nick, i, topic);
    }
    else if (strstr (subj, " ") == NULL)
      log (TMP_URL, "%s %s\n", subj, ptr);
  }
  fclose (fp);
  rename (TMP_URL, URL2);
  if (FOUND == 0)
    L030 (target, nick, topic);
}

void
chanserv (char *source, char *target, char *buf)
{
  char *cmd, *s, *s2, *s3, *s4, *s5, *ptr, *ptr3, temp[1024];
  long sn2 = 0, sn = 0, i = 0, unixtime = 0;

#ifdef	RANDOM_STUFF
  if (stricmp (target, CHAN) == 0)
    Rand_Idle = 0;
#endif
  stripline (buf);
  stripline (source);
  if (buf == NULL || target == NULL || source == NULL)
    R;
  cmd = strtok (buf, " ");
  if (cmd == NULL)
    R;
  if (*cmd == ':')
    cmd++;
  if ((ptr = strchr (source, '!')) != NULL)
    *ptr++ = '\0';
  /* ------ commands that require a privmsg ------ */
  if (*target != '#' && *target != '&' && *target != '+') {
    if (stricmp (cmd, "PASS") == 0 || stricmp (cmd, "PASSWORD") == 0
	|| stricmp (cmd, "PASSWD") == 0) {
      s = strtok (NULL, " ");
      s2 = strtok (NULL, " ");
      if (s == NULL || s2 == NULL) {
	L031 (source, Mynick);
	R;
      }
      if (strlen (s2) > 25)
	s2[25] = '\0';
      set_pass (source, ptr, s, s2);
      R;
    }
    else if (stricmp (cmd, "RAW") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, "");
	if (s != NULL)
	  S ("%s\n", s);
      }
    }
    else if (stricmp (cmd, "ADDUSER") == 0) {
      if (check_access (ptr, "#*", 0, source) >= 3) {
	s4 = strtok (NULL, " ");
	s = strtok (NULL, " ");
	s2 = strtok (NULL, " ");
	s5 = strtok (NULL, " ");
	if (s == NULL || s4 == NULL || s2 == NULL || s5 == NULL) {
	  L055 (source);
	  R;
	}
	sn = atoi (s2);
	if (sn > 10 || sn <= 0)
	  R;
	if (strlen (s) < 7)
	  R;
	L056 (*CMDCHAR);
	add_helper (s4, s, sn, 0, temp, s5);
	L057 (source, s, sn);
	save_changes ();
      }
    }
    else if (stricmp (cmd, "DIE") == 0 || stricmp (cmd, "QUIT") == 0) {
      s = strtok (NULL, "");
      if (check_access (ptr, target, 0, source) >= 3) {
	if (s == NULL) {
	  L032 (source);
	}
	else
	  Snow ("QUIT :K\2\2illed (%s (%s))\n", source, s);
	db_sleep (1);
#ifdef	WIN32
	printf ("\n\nGood-bye! %s (c) Jason Hamilton\n\n", dbVersion);
	uptime = time (NULL) - uptime;
	printf ("Time elapsed: %d hour%s, %d min%s\n\n",
		uptime / 3600, uptime / 3600 == 1 ? "" : "s",
		(uptime / 60) % 60,
		(uptime / 60) % 60 == 1 ? "" : "s");
	db_sleep (5);
#endif
	exit (0);
      }
#if CTCP == 1
    }
    else if (stricmp (cmd, "\1VERSION\1") == 0) {
      if (cf (ptr, source, target))
	R;
      if (cf (ptr, source, target))
	R;
      S ("NOTICE %s :\1VERSION %s\1\n", source, VERSION_REPLY);
    }
    else if (stricmp (cmd, "\1PING") == 0) {
      if (cf (ptr, source, target))
	R;
      if (cf (ptr, source, target))
	R;
      s2 = strtok (NULL, "");
      if (s2 != NULL) {
	if (strlen (s2) > 21)
	  s2[21] = '\0';
	S ("NOTICE %s :\1PING %s\n", source, s2);
      }
#endif
    }
    else if (stricmp (cmd, "LOGIN") == 0) {
      s = strtok (NULL, " ");
      if (s == NULL)
	R;
      do_login (source, s);
    }
    R;
  }
  add_user (target, source, ptr, 0);	/* Unidle */
  /* ------ Commands that require a CMDCHAR to activate ------ */
  if (*cmd == *CMDCHAR) {
    if (Sleep_Toggle == 1)
      R;
    cmd++;
    if (cf (ptr, source, target))
      R;
    if (stricmp (cmd, "USERLIST") == 0 || stricmp (cmd, "HLIST") == 0
	|| stricmp (cmd, "ACCESS") == 0) {
      if (check_access (ptr, target, 0, source) == 0)
	R;
      s = strtok (NULL, " ");
      if (s != NULL) {
	show_helper_list (source, atoi (s));
      }
      else
	show_helper_list (source, 0);
    }
    else if (stricmp (cmd, "BANLIST") == 0) {
      if (check_access (ptr, target, 0, source) == 0)
	R;
      show_banlist (source);
    }
    else if (stricmp (cmd, "LANG") == 0
	     || stricmp (cmd, "LANGUAGE") == 0) {
      S ("PRIVMSG %s :%s, %s\n", target, source, I_SPEAK);
    }
    else if (stricmp (cmd, "CHANINFO") == 0) {
      show_chaninfo (source, target);
    }
    else if (stricmp (cmd, "IDLE") == 0) {
      s2 = strtok (NULL, " ");
      if (s2 == NULL)
	R;
      if (stricmp (s2, source) == 0) {
	S ("PRIVMSG %s :%s, don't be lame.\n", target, source);
	R;
      }
      unixtime = return_useridle (target, s2, 0);
      if (unixtime == 0) {
	S ("PRIVMSG %s :%s, I do not see %s in %s.\n", target, source,
	   s2, target);
	return;
      }
      unixtime = time (NULL) - unixtime;
      if (unixtime > 86400)
	S ("PRIVMSG %s :%s, %s has been idle %d day%s, %02d:%02d\n",
	   target, source, s2, unixtime / 86400,
	   (unixtime / 86400 == 1) ? "" : "s", (unixtime / 3600) % 24,
	   (unixtime / 60) % 60);
      else if (unixtime > 3600)
	S ("PRIVMSG %s :%s, %s has been idle %d hour%s, %d min%s\n",
	   target, source, s2, unixtime / 3600,
	   unixtime / 3600 == 1 ? "" : "s", (unixtime / 60) % 60,
	   (unixtime / 60) % 60 == 1 ? "" : "s");
      else
	S ("PRIVMSG %s :%s, %s has been idle %d minute%s, %d sec%s\n",
	   target, source, s2, unixtime / 60,
	   unixtime / 60 == 1 ? "" : "s", unixtime % 60,
	   unixtime % 60 == 1 ? "" : "s");
    }
    else if (stricmp (cmd, "N") == 0 || stricmp (cmd, "NICK") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L036 (source);
	  R;
	}
	strcpy (Mynick, s);
	sprintf (NICK_COMMA, "%s,", Mynick);
	sprintf (COLON_NICK, "%s:", Mynick);
	sprintf (BCOLON_NICK, "%s\2:\2", Mynick);
	L037 (source, Mynick);
	S ("NICK %s\n", Mynick);
      }
      else
	L038 (source, source);
    }
    else if (stricmp (cmd, "L") == 0 || stricmp (cmd, "PART") == 0
	     || stricmp (cmd, "LEAVE") == 0
	     || stricmp (cmd, "P") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("PART %s\n", target);
	}
	else {
	  S ("PART %s\n", s);
	  L039 (target, s);
	}
      }
    }
    else if (stricmp (cmd, "VARIABLES") == 0) {
      S ("PRIVMSG %s :%s, %s\n", target, source, myVariables);
    }
    else if (stricmp (cmd, "JOIN") == 0 || stricmp (cmd, "J") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("JOIN %s\n", target);
	}
	else {
	  S ("JOIN %s\n", s);
	  L040 (target, s);
	}
      }
#if DO_CHANBOT_CRAP == 1
    }
    else if (stricmp (cmd, "OP") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("PRIVMSG %s :Specify a nick!\n", target);
	  return;
	}
	else {
	  S ("MODE %s +oooooo %s\n", target, s);
	}
      }
    }
    else if (stricmp (cmd, "DEOP") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("PRIVMSG %s :Specify a nick!\n", target);
	  return;
	}
	else {
	  S ("MODE %s -oooooo %s\n", target, s);
	}
      }
    }
    else if (stricmp (cmd, "DOWN") == 0) {
      if (check_access (ptr, target, 0, source) >= 2)
	S ("MODE %s -o %s\n", target, source);
    }
    else if (stricmp (cmd, "UP") == 0) {
      if (check_access (ptr, target, 0, source) >= 2)
	S ("MODE %s +o %s\n", target, source);
    }
    else
      if (
	  (stricmp (cmd, "KICK") == 0 || stricmp (cmd, "WACK") == 0
	   || stricmp (cmd, "K") == 0 || stricmp (cmd, "NAIL") == 0)) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  S ("PRIVMSG %s :Specify a nick/chan!\n", target);
	  return;
	}
	else {
	  if (*s != '#' && *s != '&') {
	    s2 = strtok (NULL, "");
	    if (s2 == NULL) {
	      if (stricmp (s, Mynick) == 0) {
		S ("KICK %s %s :hah! As *IF*\n", target, source);
	      }
	      else
		S ("KICK %s %s :\2%s\2'ed: %s\n", target, s, cmd,
		   DEFAULT_KICK);
	    }
	    else if (stricmp (s, Mynick) == 0) {
	      S ("KICK %s %s :%s\n", target, s, s2);
	    }
	    else
	      S ("KICK %s %s :\2%s\2'ed: %s\n", target, s, cmd, s2);
	  }
	  else {
	    s2 = strtok (NULL, " ");
	    if (s2 == NULL) {
	      S
		("NOTICE %s :You must specify a nick to kick from %s!\n",
		 source, s);
	    }
	    else {
	      s3 = strtok (NULL, "");
	      if (s3 == NULL) {
		if (stricmp (s2, Mynick) == 0) {
		  S ("KICK %s %s :hah! As *IF*\n", s, source);
		}
		else
		  S ("KICK %s %s :\2%s\2ed: %s\n", s, s2, cmd,
		     DEFAULT_KICK);
	      }
	      else {

		if (stricmp (s2, Mynick) == 0) {
		  S ("KICK %s %s :hah! As *IF* (%s)\n", s, source);
		}
		else
		  S ("KICK %s %s :\2%s\2ed: %s\n", s, s2, cmd, s3);
	      }
	    }
	  }
	}
      }
#endif
    }
    else if (stricmp (cmd, "CYC") == 0 || stricmp (cmd, "CYCLE") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("PART %s\n", target);
	  S ("JOIN %s\n", target);
	}
	else {
	  S ("PART %s\n", s);
	  S ("JOIN %s\n", s);
	  S ("PRIVMSG %s :Cycling %s\n", target, s);
	}
      }
    }
    else if (stricmp (cmd, "DIE") == 0 || stricmp (cmd, "QUIT") == 0) {
      s = strtok (NULL, "");
      if (check_access (ptr, target, 0, source) >= 3) {
	if (s == NULL) {
	  L032 (source);
	}
	else
	  Snow ("QUIT :K\2\2illed (%s (%s))\n", source, s);
	db_sleep (1);
#ifdef	WIN32
	printf ("\n\nGood-bye! %s (c) Jason Hamilton\n\n", dbVersion);
	uptime = time (NULL) - uptime;
	printf ("Time elapsed: %d hour%s, %d min%s\n\n",
		uptime / 3600, uptime / 3600 == 1 ? "" : "s",
		(uptime / 60) % 60,
		(uptime / 60) % 60 == 1 ? "" : "s");
	db_sleep (5);
#endif
	exit (0);
      }
#if DO_CHANBOT_CRAP == 1
    }
    else if (stricmp (cmd, "DEV") == 0 || stricmp (cmd, "DV") == 0
	     || stricmp (cmd, "DEVOICE") == 0
	     || stricmp (cmd, "DVOICE") == 0) {
      if (check_access (ptr, target, 0, source) >= 1) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  L041 (target);
	  R;
	}
	else
	  S ("MODE %s -vvvvvvv %s\n", target, s);
      }
    }
    else if (stricmp (cmd, "VOICE") == 0 || stricmp (cmd, "V") == 0) {
      if (check_access (ptr, target, 0, source) >= 1) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  L041 (target);
	  R;
	}
	else
	  S ("MODE %s +vvvvvvv %s\n", target, s);
      }
    }
    else if (stricmp (cmd, "T") == 0 || stricmp (cmd, "TOPIC") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("TOPIC %s :\n", target);
	  R;
	}
	else {
	  S ("TOPIC %s :%s\n", target, s);
	}
      }
#endif
    }
    else if (stricmp (cmd, "JUMP") == 0
	     || stricmp (cmd, "SERVER") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  S ("NOTICE %s :Syntax: JUMP <server> [port]\n", source);
	  R;
	}
	s2 = strtok (NULL, " ");
	if (s2 == NULL) {
	  sn = 6667;
	}
	else
	  sn = atoi (s2);
	S ("QUIT :Jumping to %s:%d\n", s, sn);
	db_sleep (1);
	socketfd = get_connection (s, VHOST, sn);
	init_bot ();
      }
#if DO_CHANBOT_CRAP == 1
    }
    else if (stricmp (cmd, "DELBAN") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L042 (source);
	  R;
	}
	if (del_permban (source, s) == 1)
	  S ("MODE %s -b %s\n", target, s);
	else
	  L043 (source);
      }
#endif
    }
    else if (stricmp (cmd, "DELUSER") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L044 (source);
	  R;
	}
	delete_user_ram (source, s);
      }
    }
    else if (stricmp (cmd, "TEASEOP") == 0
	     || stricmp (cmd, "TO") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L036 (target);
	  R;
	}
	if (stricmp (s, Mynick) == 0) {
	  L045 (source);
	}
	else
	  S ("MODE %s +o-o+o-o+o-o %s %s %s %s %s %s\n", target, s, s,
	     s, s, s, s);
      }
#ifndef	WIN32
    }
    else if (stricmp (cmd, "BACKUP") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	sprintf (temp, "/bin/cp -rf %s \"%s.bak @ `date`\"\n", URL2,
		 URL2);
	system (temp);
	L046 (target);

      }
#endif
    }
    else if (stricmp (cmd, "AUTOTOPIC") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  L047 (source, *CMDCHAR);
	  R;
	}
	set_autotopic (source, target, s);
      }
    }
    else if (stricmp (cmd, "SETCHAN") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L048 (source);
	  R;
	}
	strcpy (CHAN, s);
	L049 (source, CHAN);
	save_setup ();
      }
    }
    else if (stricmp (cmd, "SETCHAR") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L050 (source);
	  R;
	}
	*CMDCHAR = *s;
	L051 (source, *CMDCHAR);
	save_setup ();
      }
    }
    else if (stricmp (cmd, "SETUSER") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L052 (source);
	  R;
	}
	strcpy (UID, s);
	L053 (source, UID);
	save_setup ();
      }
    }
    else if (stricmp (cmd, "VHOST") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L091 (source);
	  R;
	}
	strcpy (VHOST, s);
	L092 (source, VHOST);
	save_setup ();
      }
    }
    else if (stricmp (cmd, "SETNICK") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L054 (source);
	  R;
	}
	S ("NICK %s\n", s);
	strcpy (s_Mynick, s);
	strcpy (Mynick, s);
	sprintf (NICK_COMMA, "%s,", Mynick);
	sprintf (COLON_NICK, "%s:", Mynick);
	sprintf (BCOLON_NICK, "%s\2:\2", Mynick);
	save_setup ();
      }
    }
    else if (stricmp (cmd, "RAW") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, "");
	if (s != NULL)
	  S ("%s\n", s);
      }
    }
    else if (stricmp (cmd, "SEEN") == 0 && SeeN == 1) {
      s = strtok (NULL, " ");
      if (s == NULL) {
	count_seen (source, target);
      }
      if (return_useridle (target, s, 1) == 1) {
	S ("PRIVMSG %s :%s is right here in the channel!\n", target,
	   s);
	R;
      }
      show_seen (s, source, target);
#if STATUS == 1
    }
    else if (stricmp (cmd, "LUSERS") == 0) {
      if (check_access (ptr, target, 0, source) >= 1)
	S ("LUSERS\n");
#endif
    }
    else if (stricmp (cmd, "ADDUSER") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s4 = strtok (NULL, " ");
	s = strtok (NULL, " ");
	s2 = strtok (NULL, " ");
	s5 = strtok (NULL, " ");
	if (s == NULL || s4 == NULL || s2 == NULL || s5 == NULL) {
	  L055 (source);
	  R;
	}
	sn = atoi (s2);
	if (sn > 10 || sn <= 0)
	  R;
	if (strlen (s) < 7)
	  R;
	L056 (*CMDCHAR);
	add_helper (s4, s, sn, 0, temp, s5);
	L057 (source, s, sn);
	save_changes ();
      }
#if DO_CHANBOT_CRAP == 1
    }
    else if (stricmp (cmd, "PERMBAN") == 0
	     || stricmp (cmd, "SHITLIST") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	if (s == NULL) {
	  L058 (source, *CMDCHAR, cmd);
	  R;
	}
	s2 = strtok (NULL, "");
	if (s2 == NULL)
	  s2 = "Permbanned!";
	add_permban (s, 0, s2);
	L059 (source, PERMBAN_counter, s, s2);
	save_permbans ();
	S ("MODE %s +b %s\n", target, s);
      }
#endif
#if DBTIMERS == 1
    }
    else if (stricmp (cmd, "ALARM") == 0
	     || stricmp (cmd, "ALARMCLOCK") == 0) {
      if (check_access (ptr, target, 0, source) >= 2) {
	s = strtok (NULL, " ");
	s2 = strtok (NULL, "");
	if (s == NULL || s2 == NULL) {
	  S
	    ("NOTICE %s :Syntax: <time type: d/h/m><time> <text to say>\n",
	     source);
	  R;
	}
	if (strlen (s) < 2) {
	  S
	    ("NOTICE %s :Syntax: <time type: d/h/m><time> <text to say>\n",
	     source);
	  R;
	}
	if (*s == 'd') {
	  sn = 86400;
	  s++;
	}
	else if (*s == 'h') {
	  sn = 3600;
	  s++;
	}
	else if (*s == 'm') {
	  sn = 60;
	  s++;
	}
	else {
	  S
	    ("NOTICE %s :Syntax: <time type: \2d/h/m\2><time> <text to say>\n",
	     source);
	  R;
	}
	if (strspn (s, NUMBER_LIST) != strlen (s)) {
	  S ("NOTICE %s :Time must be a number.\n", source);
	  R;
	}
	i = (atoi (s) * sn) + time (NULL);
	sprintf (temp, "%s/%d", DBTIMERS_PATH, (int) i);
	log (temp, "PRIVMSG %s :\2ALARMCLOCK\2 by %s!%s: %s\n",
	     target, source, ptr, s2);

	unixtime = atoi (s) * sn;
	if (unixtime > 86400)
	  S
	    ("PRIVMSG %s :%s, alarmclock set to go off in %d day%s, %02d:%02d\n",
	     target, source, unixtime / 86400,
	     (unixtime / 86400 == 1) ? "" : "s",
	     (unixtime / 3600) % 24, (unixtime / 60) % 60);
	else if (unixtime > 3600)
	  S
	    ("PRIVMSG %s :%s, alarmclock set to go off in %d hour%s, %d min%s\n",
	     target, source, unixtime / 3600,
	     unixtime / 3600 == 1 ? "" : "s", (unixtime / 60) % 60,
	     (unixtime / 60) % 60 == 1 ? "" : "s");
	else
	  S
	    ("PRIVMSG %s :%s, alarmclock set to go off in %d minute%s, %d sec%s\n",
	     target, source, unixtime / 60,
	     unixtime / 60 == 1 ? "" : "s", unixtime % 60,
	     unixtime % 60 == 1 ? "" : "s");
      }
#endif
    }
    else if (stricmp (cmd, "REPEAT") == 0
	     || stricmp (cmd, "TIMER") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	s = strtok (NULL, " ");
	s2 = strtok (NULL, " ");
	s3 = strtok (NULL, "");
	if (s == NULL || s2 == NULL || s3 == NULL) {
	  L060 (source);
	  R;
	}
	sn = atoi (s);
	sn2 = atoi (s2);
	while (sn > 0) {
	  S ("%s\n", s3);
	  sn--;
	  db_sleep (sn2);
	}
      }
#ifndef	WIN32
    }
    else if (stricmp (cmd, "REHASH") == 0
	     || stricmp (cmd, "RESTART") == 0) {
      if (check_access (ptr, target, 0, source) >= 3) {
	L062 (dbVersion);
	sprintf (temp, "sleep 2; %s", DARKBOT_BIN);
	system (temp);
	db_sleep (1);
	exit (0);
      }
#endif
    }
    else if (stricmp (cmd, "PING") == 0) {
      if (check_access (ptr, target, 0, source) == 0) {
	S ("NOTICE %s PONG!\n", source);
      }
      else
	S ("PRIVMSG %s :PONG!\n", target);
    }
    else if (stricmp (cmd, "HELP") == 0) {
      L100 (source, NICK_COMMA, COLON_NICK, BCOLON_NICK, Mynick,
	    NICK_COMMA, NICK_COMMA);
      db_sleep (3);
      if (cf (ptr, source, target))
	R;
      L101 (source, NICK_COMMA, NICK_COMMA, NICK_COMMA);
      db_sleep (2);
    }
    else if (stricmp (cmd, "SETINFO") == 0) {
      if (check_access (ptr, target, 0, source) >= 1) {
	s = strtok (NULL, "");
	if (s == NULL) {
	  S ("NOTICE %s :%s\n", source, mySetinfo);
	  R;
	}
	update_setinfo (ptr, s, source);
	save_changes ();
      }
    }
#if CTCP == 1
  }
  else if (stricmp (cmd, "\1VERSION\1") == 0) {	/* these are #chan
						 * ctcp's */
    if (cf (ptr, source, target))
      R;
    if (cf (ptr, source, target))
      R;
    S ("NOTICE %s :\1VERSION %s\1\n", source, VERSION_REPLY);
  }
  else if (stricmp (cmd, "\1PING") == 0) {
    if (cf (ptr, source, target))
      R;
    if (cf (ptr, source, target))
      R;
    s2 = strtok (NULL, "");
    if (s2 != NULL) {
      if (strlen (s2) > 21)
	s2[21] = '\0';
      S ("NOTICE %s :\1PING %s\n", source, s2);
    }
#endif
  }
  else if (stricmp (cmd, "\2\2DARKBOT") == 0) {
    if (Sleep_Toggle == 1)
      R;
    if (cf (ptr, source, target))
      R;
    S ("PRIVMSG %s :%s reporting! My cmdchar is %c\n", target,
       dbVersion, *CMDCHAR);
  }
  else if (stricmp (cmd, NICK_COMMA) == 0
	   || stricmp (cmd, COLON_NICK) == 0
	   || stricmp (cmd, BCOLON_NICK) == 0
	   || stricmp (cmd, Mynick) == 0) {
    s = strtok (NULL, " ");
    if (s != NULL) {
      if (stricmp (s, "WAKEUP") == 0) {
	if (Sleep_Toggle == 0)
	  R;
	if (check_access (ptr, target, 0, source) >= SLEEP_LEVEL) {
	  Sleep_Toggle = 0;
	  AIL4 = 0;
	  S ("PRIVMSG %s :%s\n", target, WAKEUP_ACTION);
	  if (stricmp (sleep_chan, target) != 0)
	    S ("PRIVMSG %s :%s\n", sleep_chan, WAKEUP_ACTION);
	  R;
	}
      }
    }
    if (Sleep_Toggle == 1)
      R;
    if (cf (ptr, source, target))
      R;
    if (s != NULL) {
#ifdef	RANDOM_STUFF
      if (stricmp (s, "RANDOMSTUFF") == 0
	  || stricmp (s, "RANDSTUFF") == 0) {
	if (check_access (ptr, target, 0, source) >= RAND_LEVEL) {
	  s2 = strtok (NULL, "");
	  if (s2 == NULL) {
	    L064 (target, source);
	    R;
	  }
	  add_randomstuff (source, target, s2);
	}
      }
      else
#endif
	if (stricmp (s, "ADD") == 0 || stricmp (s, "REMEMBER") == 0
	    || stricmp (s, "SAVE") == 0 || stricmp (s, "STORE") == 0) {
#ifdef	REQ_ACCESS_ADD
	if (check_access (ptr, target, 0, source) >= 1) {
#endif
	  s2 = strtok (NULL, " ");
	  if (s2 == NULL) {
	    L065 (target, source);
	    R;
	  }
	  if (strlen (s2) > MAX_TOPIC_SIZE) {
	    s2[MAX_TOPIC_SIZE] = '\0';
	    S
	      ("PRIVMSG %s :%s, topic is over the limit, and has characters truncated.\n",
	       target, source);
	  }
	  s3 = strtok (NULL, "");
	  if (s3 == NULL) {
	    L066 (target, source, s2);
	    R;
	  }
	  if (strlen (s3) > MAX_DATA_SIZE)
	    s3[MAX_DATA_SIZE] = '\0';
	  strlwr (s2);
	  if (*s2 == '~') {
	    S
	      ("PRIVMSG %s :%s, rdb files can only be called from the data of a topic, they cannot be used in the topic itself.\n",
	       target, source);
	    R;
	  }
	  if (check_existing_url (source, s2, target) == 1) {
	    S ("PRIVMSG %s :%s \37%s\37\n", target, EXISTING_ENTRY,
	       s2);
	    R;
	  }
#ifdef	LOG_ADD_DELETES
	  log (ADD_DELETES, "[%s] %s!%s ADD %s %s\n", date (), source,
	       ptr, s2, s3);
#endif
	  ADDITIONS++;
	  if (s2[0] == 'i' && s2[1] == 'l' && s2[2] == 'c') {
	    log (URL2, "%s ([%s] %s!%s): %s\n", s2, date (), source,
		 ptr, s3);
	  }
	  else
	    log (URL2, "%s %s\n", s2, s3);
	  L067 (target, source);
#ifdef	REQ_ACCESS_ADD
	}
#endif
      }
      else if (stricmp (s, "DATE") == 0 || stricmp (s, "TIME") == 0) {
	S ("PRIVMSG %s :%s, %s\n", target, source, date ());
      }
      else if (stricmp (s, "REPLACE") == 0) {
#ifdef	REQ_ACCESS_ADD
	if (check_access (ptr, target, 0, source) >= 1) {
#endif
	  s2 = strtok (NULL, " ");
	  if (s2 == NULL) {
	    L068 (target, source);
	    R;
	  }
	  if (strlen (s2) > MAX_TOPIC_SIZE)
	    s2[MAX_TOPIC_SIZE] = '\0';
	  s3 = strtok (NULL, "");
	  if (s3 == NULL) {
	    L069 (target, source, s2);
	    R;
	  }
	  if (strlen (s3) > MAX_DATA_SIZE)
	    s3[MAX_DATA_SIZE] = '\0';
	  strlwr (s2);
	  if (check_existing_url (source, s2, target) != 1) {
	    S ("PRIVMSG %s :%s \37%s\37\n", target, NO_ENTRY, s2);
	    R;
	  }
	  delete_url (source, s2, target);
#ifdef	LOG_ADD_DELETES
	  log (ADD_DELETES, "[%s] %s!%s REPLACE %s %s\n", date (),
	       source, ptr, s2, s3);
#endif
	  ADDITIONS++;
	  log (URL2, "%s %s\n", s2, s3);
	  L070 (target, source, s2);
#ifdef	REQ_ACCESS_ADD
	}
#endif
#if DO_CHANBOT_CRAP == 1
      }
      else if (stricmp (s, "PERMBANS?") == 0) {
	L071 (target, (PERMBAN_counter == 1) ? "is" : "are",
	      PERMBAN_counter, (PERMBAN_counter == 1) ? "" : "s");
#endif
#ifdef	RANDOM_STUFF
      }
      else if (stricmp (s, "RANDOMSTUFF?") == 0
	       || stricmp (s, "RANDSTUFF?") == 0) {
	L073 (target, source, Rand_Stuff);
#endif
      }
      else if (stricmp (s, "LENGTH") == 0) {
	s2 = strtok (NULL, "");
	if (s2 == NULL)
	  R;
	L074 (target, source, strlen (s2));
      }
      else if (stricmp (s, "CHAR") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL)
	  R;
	S ("PRIVMSG %s :%s, %c -> %d\n", target, source, s2[0],
	   s2[0]);
      }
      else if (stricmp (s, "SEEN") == 0 && SeeN == 1) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL)
	  R;
	show_seen (s2, source, target);
      }
      else if (stricmp (s, "SENDQ?") == 0 || stricmp (s, "QUE?") == 0) {
	L075 (target, source, get_sendq_count (2),
	      (get_sendq_count (2) == 1) ? "" : "s");
      }
      else if (stricmp (s, "JOINS?") == 0) {
	L076 (target, JOINs);
      }
      else if (stricmp (s, "LOCATION?") == 0) {
	L077 (target, (snr == 1) ? "is" : "are", snr,
	      (snr == 1) ? "" : "s", spr);
      }
      else if (stricmp (s, "CMDCHAR?") == 0) {
	L078 (target, source, *CMDCHAR);
      }
      else if (stricmp (s, "DATASEARCH") == 0
	       || stricmp (s, "DSEARCH") == 0) {
	s2 = strtok (NULL, "");
	if (s2 == NULL) {
	  L079 (target, s, source);
	  R;
	}
	datasearch (source, s2, target);
      }
      else if (stricmp (s, "SEARCH") == 0 || stricmp (s, "LOOK") == 0
	       || stricmp (s, "FIND") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL) {
	  if (stricmp (s, "FIND") == 0) {
	    S ("PRIVMSG %s :%s, %s?\n", target, TRY_FIND, source);
	  }
	  else
	    L079 (target, s, source);
	  R;
	}
	find_url (source, s2, target);
      }
      else if (stricmp (s, "INFO2") == 0) {
	show_info2 (target, source);
      }
      else if (stricmp (s, "INFO") == 0) {
	info (source, target);
#ifdef	DO_MATH_STUFF
      }
      else if (stricmp (s, "CALC") == 0 || stricmp (s, "MATH") == 0) {
	s2 = strtok (NULL, "");
	if (s2 == NULL)
	  R;
	if (strlen (s2) > 200)
	  s2[200] = '\0';
	do_math (source, target, s2);
#endif
      }
      else if (stricmp (s, "SLEEP") == 0 || stricmp (s, "HUSH") == 0) {
	if (check_access (ptr, target, 0, source) >= SLEEP_LEVEL) {
	  Sleep_Toggle = 1;
	  S ("PRIVMSG %s :%s\n", target, GOSLEEP_ACTION);
	  strcpy (sleep_chan, target);
	}
      }
      else if (stricmp (s, "UNIXTIME") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL)
	  R;
	unixtime = atoi (s2) - time (NULL);
	if (unixtime > 86400)
	  S ("PRIVMSG %s :%s, %d day%s, %02d:%02d\n", target, source,
	     unixtime / 86400, (unixtime / 86400 == 1) ? "" : "s",
	     (unixtime / 3600) % 24, (unixtime / 60) % 60);
	else if (unixtime > 3600)
	  S ("PRIVMSG %s :%s, %d hour%s, %d min%s\n", target, source,
	     unixtime / 3600, unixtime / 3600 == 1 ? "" : "s",
	     (unixtime / 60) % 60,
	     (unixtime / 60) % 60 == 1 ? "" : "s");
	else
	  S ("PRIVMSG %s :%s, %d minute%s, %d sec%s\n", target,
	     source, unixtime / 60, unixtime / 60 == 1 ? "" : "s",
	     unixtime % 60, unixtime % 60 == 1 ? "" : "s");
      }
      else if (stricmp (s, "CPU?") == 0) {
	getrusage (RUSAGE_SELF, &r_usage);
	S ("PRIVMSG %s :CPU usage: %ld.%06ld, System = %ld.%06ld\n",
	   target, r_usage.ru_utime.tv_sec, r_usage.ru_utime.tv_usec);
      }
      else if (stricmp (s, "DISPLAY") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL)
	  R;
	display_url (target, source, s2);
#ifndef	WIN32
      }
      else if (stricmp (s, "UPTIME") == 0) {
	sprintf (temp, "uptime\n");
	S ("PRIVMSG %s :Uptime: %s\n", target, run_program (temp));
      }
      else if (stricmp (s, "OS") == 0) {
	sprintf (temp, "uname\n");
	S ("PRIVMSG %s :I an running %s\n", target,
	   run_program (temp));
      }
      else if (stricmp (s, "MEM") == 0 || stricmp (s, "RAM") == 0) {
	sprintf (temp, "ps -ux | grep %s\n", DARKBOT_BIN);
	S ("PRIVMSG %s :ps: %s\n", target, run_program (temp));
      }
      else if (stricmp (s, "RDB") == 0) {
	s2 = strtok (NULL, "");
	if (s2 == NULL) {
	  sprintf (temp, "ls dat/*.rdb | wc\n");
	  S ("PRIVMSG %s :RDB: %s\n", target, run_program (temp));
	}
	else {
	  if (strspn (s2, SAFE_LIST) != strlen (s2)) {
	    S
	      ("PRIVMSG %s :%s, rdb files are made up of letters and or numbers, no other text is accepted.\n",
	       target, source);
	    R;
	  }
	  sprintf (temp, "ls -la dat/*.rdb | grep %s | tail 3\n", s2);
	  S ("PRIVMSG %s :%s\n", target, run_program (temp));
	}
#endif
      }
      else if (stricmp (s, "DELETE") == 0
	       || stricmp (s, "REMOVE") == 0
	       || stricmp (s, "FORGET") == 0
	       || stricmp (s, "DEL") == 0) {
#ifdef	REQ_ACCESS_DEL
	if (check_access (ptr, target, 0, source) >= 1) {
#endif
	  s2 = strtok (NULL, " ");
	  if (s2 == NULL) {
	    S ("PRIVMSG %s :%s what, %s?\n", target, s, source);
	    R;
	  }
	  if (strlen (s2) > MAX_TOPIC_SIZE)
	    s2[MAX_TOPIC_SIZE] = '\0';
#ifdef	LOG_ADD_DELETES
	  log (ADD_DELETES, "[%s] %s!%s DEL %s\n", date (), source,
	       ptr, s2);
#endif
	  if (*s2 == '~') {	/* need level 2 to delete .rdb files */
	    if (check_access (ptr, target, 0, source) >= 2) {
	      delete_url (source, s2, target);
	    }
	    R;
	  }
	  delete_url (source, s2, target);
#ifdef	REQ_ACCESS_DEL
	}
#endif
      }
      else if (stricmp (s, "TELL") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL) {
	  L085 (target, source);
	  R;
	}
	s3 = strtok (NULL, " ");
	if (s3 == NULL) {
	  L083 (target, source, s2);
	  R;
	}
	if (stricmp (s3, Mynick) == 0)
	  R;			/* don't bother telling
				 * myself about stuff */
	if (stricmp (s3, "ABOUT") == 0) {
	  s4 = strtok (NULL, " ");
	  if (s4 == NULL) {
	    L084 (target, source, s2);
	    R;
	  }
	  strlwr (s4);
	  show_url (s2, get_multiword_topic (s4), target, 1, 0, "0",
		    1);
	}
	else {
	  strlwr (s3);
	  show_url (s2, get_multiword_topic (s3), target, 1, 0, "0",
		    1);
	}
      }
      else if (stricmp (s, "WHERE") == 0 || stricmp (s, "WHO") == 0
	       || stricmp (s, "WHAT") == 0) {
	s2 = strtok (NULL, " ");
	if (s2 == NULL) {
	  L086 (target, source);
	  R;
	}
	s3 = strtok (NULL, " ");
	if (s3 == NULL)
	  R;
	strlwr (s3);
	ptr3 = strchr (s3, '?');
	if (ptr3 != NULL)
	  memmove (ptr3, ptr3 + 1, strlen (ptr3 + 1) + 1);
	ptr3 = strchr (s3, '!');
	if (ptr3 != NULL)
	  memmove (ptr3, ptr3 + 1, strlen (ptr3 + 1) + 1);
	if (stricmp (s3, "A") == 0 || stricmp (s3, "AN") == 0) {
	  s4 = strtok (NULL, " ");
	  if (s4 == NULL) {
	    L087 (target, s, s2, s3, *CMDCHAR);
	    R;
	  }
	  show_url (source, get_multiword_topic (s4), target, 1, 0,
		    "0", 0);
	}
	else
	  show_url (source, get_multiword_topic (s3), target, 1, 0,
		    "0", 0);
      }
      else
	show_url (source, get_multiword_topic (s), target, 1, 0, "0",
		  0);
    }
    else
      S ("PRIVMSG %s :%s\n", target, WHUT);
#if GENERAL_QUESTIONS == 1
  }
  else {
    if (Sleep_Toggle == 1)
      R;
    show_url (source, get_multiword_topic (cmd), target, 0, 1, ptr,
	      0);
#endif
  }
}

void
gs26 ()
{
  long i = 0;
  struct sl124 *c;

  c = sh124;
  spr++;
  if (spr > snr)
    spr = 1;
  while (c != NULL) {
    i++;
    if (i == spr) {
      strcpy (BS, c->name);
      BP = c->port;
    }
    c = c->next;
  }
}

void
add_s25 (char *server, long port)
{
  struct sl124 *n;
  n = (struct sl124 *) malloc (sizeof (struct sl124));
  if (n == NULL) {
    log ("error.log", "AHHH! No ram left! in add_s25!\n");
    R;
  }
  memset (n, 0, sizeof (struct sl124));
  snr++;
  if (n != NULL) {
    strcpy (n->name, server);
    n->port = port;

    n->next = sh124;
    sh124 = n;
  }
}

void
show_url (char *nick, char *topic, char *target, long donno,
	  long floodpro, char *uh, long no_pipe)
{
  FILE *fp;
  long x = 0, length = 0, toggle = 0, A = 0, gotit = 0, D = 0, F =
    0, Tog = 0;
  char b[700], *subj, *ptr, *ptr3, temp[700] = "", Data[700] =
    "", *ptr8 = "";

  if (strstr (nick, "|") != NULL)
    R;
  /* Rather than redoing the code, lets just ignore people who have
     a | in their nick. To allow them to be replied to means
     they can crash the bot */
  strlwr (topic);
  ptr3 = strchr (topic, '?');
  if (ptr3 != NULL)
    memmove (ptr3, ptr3 + 1, strlen (ptr3 + 1) + 1);
  if ((fp = fopen (URL2, "r")) == NULL) {
    if (donno == 1)
      L003 (nick, URL2);
    R;
  }
  while (fgets (b, 700, fp)) {
    x++;
    stripline (b);
    subj = strtok (b, " ");
    if (subj == NULL)
      continue;
    ptr = strtok (NULL, "");
    if (ptr == NULL)
      continue;
    if (stricmp (subj, topic) == 0 || !match_wild (subj, topic) == 0) {
      QUESTIONS++;
      if (floodpro == 1)
	if (cf (uh, nick, nick)) {
	  fclose (fp);
	  R;
	}
      gotit = 1;
      if (*ptr == '+') {
	ptr++;
	A = 1;
      }
      else if (*ptr == '-') {
	if (no_pipe == 1) {
	  fclose (fp);
	  R;
	}
	ptr++;
	D = 1;
      }
      else if (*ptr == '~') {
	ptr++;
	fclose (fp);
	do_randomtopic (target, ptr, nick, topic);
	return;

      }
      length = strlen (ptr);
      if (length > 3) {
	if (ptr[0] == 'i' && ptr[1] == 'l' && ptr[2] == 'c') {
	  toggle++;
	}
      }
      while (length > 0) {
	length--;
	if (Tog == 1) {
	  Tog = 0;
	  if (ptr[length] == 'N') {	/* nick */
	    toggle++;
	    sprintf (temp, "%s%s", nick, Data);
	  }
	  else if (ptr[length] == 'C') {	/* chan */
	    toggle++;
	    sprintf (temp, "%s%s", target, Data);
/* Not finished yet. 12.04.99
					} else if (ptr[length] == '1' || ptr[length] == '1' ||
						ptr[length] == '2' || ptr[length] == '3' || ptr[length] == '4' ||
						ptr[length] == '5' || ptr[length] == '6' || ptr[length] == '7' ||
						ptr[length] == '8' || ptr[length] == '9' || ptr[length] == '0') {
						toggle++;
						sprintf(temp, "%s%s", get_word(ptr[length], topic), Data);	
*/
	  }
	  else if (ptr[length] == 'T') {	/* time */
	    toggle++;
	    sprintf (temp, "%s%s", date (), Data);
	  }
	  else if (ptr[length] == 'W') {	/* WWW page */
	    toggle++;
	    sprintf (temp, "http://darkbot.net%s", Data);
	  }
	  else if (ptr[length] == 'S') {	/* serv */
	    toggle++;
	    sprintf (temp, "%s%s", BS, Data);
	  }
	  else if (ptr[length] == 'R') {	/* rand */
	    toggle++;
	    sprintf (temp, "%s%s", get_rand_nick (target), Data);
	  }
	  else if (ptr[length] == 'P') {	/* port */
	    toggle++;
	    sprintf (temp, "%d%s", (int) BP, Data);
	  }
	  else if (ptr[length] == 'Q') {	/* question */
	    toggle++;
	    sprintf (temp, "%s%s", revert_topic (topic), Data);
	  }
	  else if (ptr[length] == 'V') {	/* version */
	    toggle++;
	    sprintf (temp, "%s%s", dbVersion, Data);
	  }
	  else if (ptr[length] == '!') {	/* cmdchar */
	    toggle++;
	    sprintf (temp, "%c%s", *CMDCHAR, Data);
	  }
	  else if (ptr[length] == 'B') {	/* mynick */
	    toggle++;
	    sprintf (temp, "%s%s", Mynick, Data);
	  }
	  else
	    sprintf (temp, "%c~%s", ptr[length], Data);
	}
	else if (ptr[length] == '~') {
	  Tog = 1;
	}
	else
	  sprintf (temp, "%c%s", ptr[length], Data);
	strcpy (Data, temp);
      }				/* While */
      if (D == 1) {
	ptr8 = strtok (Data, "|");
	while (ptr8 != NULL) {
	  if (ptr8[0] == ' ')
	    ptr8++;
	  ptr8[0] = tolower (ptr8[0]);
	  ptr8[1] = tolower (ptr8[1]);
	  if (ptr8[0] == 'p' && ptr8[1] == 'r')
	    F = 1;
	  if (ptr8[0] == 'k' && ptr8[1] == 'i')
	    F = 1;
	  if (ptr8[0] == 'n' && ptr8[1] == 'o')
	    F = 1;
	  if (ptr8[0] == 't' && ptr8[1] == 'o')
	    F = 1;
	  if (F == 1)
	    S ("%s\n", ptr8);
	  F = 0;
	  ptr8 = strtok (NULL, "|");
	}
	fclose (fp);
	R;
      }
      if (toggle == 0) {
	if (A == 0) {
	  S ("PRIVMSG %s :%s%s\n", target, rand_reply (nick), Data);
	}
	else
	  S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
      }
      else if (A == 0) {
	S ("PRIVMSG %s :%s\n", target, Data);
      }
      else
	S ("PRIVMSG %s :\1ACTION %s\1\n", target, Data);
      fclose (fp);
      R;
    }				/* Subject match */
  }
  fclose (fp);
  if (donno == 1) {
    if (strlen (topic) > 3) {
      strlwr (topic);
      if (topic[0] == 'i' && topic[1] == 'l' && topic[2] == 'c') {
	S
	  ("PRIVMSG %s :%s, I found no matching ILC for that channel.\n",
	   target, nick);
	R;
      }
    }
    S ("PRIVMSG %s :%s, %s\n", target, nick, DONNO_Q);
  }
}

/**
 * 6/23/00 Dan:
 * - Moved declaration of newact into #ifdef
 */
int
main (int argc, char **argv)
{
  char temp[300];
  struct timeval timeout;
  fd_set fdvar;

#if (SGI == 1) || (NEED_LIBC5 == 1)
  struct sigaction newact;
#endif

#ifdef	DEBUG
  DebuG = 1;
#endif
#ifndef	WIN32			/* not win32 */
#endif
  get_s ();
  srand (time (0));
  uptime = time (NULL);
  if (argv[1] != NULL) {
    if (stricmp (argv[1], "-SEEN") == 0) {
      SeeN = 1;
      printf ("\nSEEN ENABLED.\n\n");
    }
    else if (stricmp (argv[1], "-DEBUG") == 0) {
      DebuG = 1;
      printf ("\nDEBUG ENABLED.\n\n");
    }
    else {
      printf ("\n\n%s HELP:\n\n", dbVersion);
      printf ("%s          (Launches Darkbot to IRC)\n", argv[0]);
      printf
	("%s -SEEN    (Enables SEEN [Even if SEEN is undefined])\n",
	 argv[0]);
      printf ("%s -DEBUG   (Launch in debug mode)\n", argv[0]);
      exit (0);
    }
  }
  strcpy (DARKBOT_BIN, argv[0]);
#if (SGI == 1) || (NEED_LIBC5 == 1)
  newact.sa_handler = sig_alrm;
  sigemptyset (&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction (SIGALRM, &newact, NULL);
  newact.sa_handler = sig_segv;
  sigemptyset (&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction (SIGSEGV, &newact, NULL);
  newact.sa_handler = sig_hup;
  sigemptyset (&newact.sa_mask);
  newact.sa_flags = 0;
  sigaction (SIGHUP, &newact, NULL);
#else /* ----------------------- */

  signal (SIGALRM, sig_alrm);
  signal (SIGSEGV, sig_segv);
  signal (SIGHUP, sig_hup);
#endif
#ifndef	WIN32
#ifdef	FORK
  if (fork ())
    exit (0);
#endif
#endif

#ifdef	RANDOM_STUFF
  get_rand_stuff_time ();
#endif
  printf
    ("\n\n--------------------------------------------------------------------------\n");
  printf ("%s (c) Jason Hamilton http://darkbot.net\n", dbVersion);
  printf
    ("--------------------------------------------------------------------------\n");
#ifndef	WIN32
  sprintf (temp, "echo \"%d\" > %s.pid", getpid (), DARKBOT_BIN);
  system (temp);
#endif
  printf ("%s\n", usage_agreement);
#ifndef	WIN32
  db_sleep (2);
#endif
#ifndef	WIN32
#ifdef	SORT
  printf ("\n\nSorting %s database...\n", dbVersion);
  sprintf (temp, "sort %s -o %s\n", URL2, URL2);
  system (temp);
#endif
#endif
  load_helpers ();
  raw_now ("SERVERS");
  raw_now ("SETUP");
  raw_now ("PERMBAN");
  gs26 ();
#ifndef	WIN32
#if	LOG_PRIVMSG == 1
  printf ("\n\n*** Logging PRIVMSG's -> %s ***\n\n\n", privmsg_log);
#endif
#endif
  alarm (AIL);
  printf ("Connecting to %s:%d .....\n", BS, (int) BP);
  socketfd = get_connection (BS, VHOST, BP);
  if (socketfd == -1) {
    exit (0);
  }
  init_bot ();
  while (1) {
    timeout.tv_sec = WSEC;
    timeout.tv_usec = USEC;
    FD_ZERO (&fdvar);
    FD_SET (socketfd, &fdvar);
    switch (select
	    (NFDBITS, &fdvar, (fd_set *) 0, (fd_set *) 0, &timeout)) {
    case 0:
      break;
    case -1:
      if (!alarmed) {
	db_sleep (RECHECK);
      }
      else
	alarmed = 0;
      break;
    default:
      parse_server_msg (&fdvar);
      break;
    }
  }
}

void
sig_hup (int notUsed)
{
  char temp[1024];

#ifdef	WIN32
  S ("QUIT :Window killed - %s terminating\n", dbVersion);
#else
  S ("QUIT :SIGHUP - Restarting %s ...\n", dbVersion);
#endif
  sprintf (temp, "sleep 2; %s", DARKBOT_BIN);
  system (temp);
  db_sleep (1);
  exit (0);

}

void
sig_alrm (int notUsed)
{
  alarmed = 1;
  alarm (AIL);
#if DBTIMERS == 1
  check_dbtimers ();		/* timers :) */
#endif
  AIL8 += AIL;
  if (AIL8 >= SEND_DELAY) {
    AIL8 = 0;
    Send ();
  }
  LastInput += AIL;
  if (LastInput >= 500) {
    LastInput = 0;
#if CHECK_STONED == 1
    L088 (BS);
#ifdef	WIN32
    printf ("\nNo responce from %s in 5 mins, reconnecting...\n", BS);
#endif
    gs26 ();
    socketfd = get_connection (BS, VHOST, BP);
    init_bot ();
#endif
  }
  AIL10 += AIL;
  if (AIL10 >= 900) {		/* 15 mins */
    AIL10 = 0;
    if (MARK_CHANGE == 1) {
      MARK_CHANGE = 0;
      save_setup ();		/* save settings */
    }
  }
  AIL666 += AIL;
  if (AIL666 >= 60) {		/* 60 sec timer */
    AIL666 = 0;
    S ("PING :%s\n", BS);
  }
  AIL9 += AIL;
  if (AIL9 >= 30) {
    AIL9 = 0;
    if (stricmp (s_Mynick, Mynick) != 0) {
      S ("NICK %s\n", s_Mynick);
      strcpy (Mynick, s_Mynick);
      sprintf (NICK_COMMA, "%s,", Mynick);
      sprintf (COLON_NICK, "%s:", Mynick);
      sprintf (BCOLON_NICK, "%s\2:\2", Mynick);
    }
  }
  if (Sleep_Toggle == 1) {
    AIL4 += AIL;
    if (AIL4 >= SLEEP_TIME) {
      Sleep_Toggle = 0;
      AIL4 = 0;
      L089 (sleep_chan);
    }
  }
  AIL2 += AIL;
  AIL3 += AIL;
#ifdef	RANDOM_STUFF
  Rand_Idle++;
  if (RAND_IDLE <= Rand_Idle) {
    Rand_Idle = 0;
    do_random_stuff ();
    get_rand_stuff_time ();
  }
  Rand_Stuff -= AIL;
  if (Rand_Stuff <= 0) {
    if (Sleep_Toggle != 1)
      do_random_stuff ();
    get_rand_stuff_time ();
  }
#endif
  if (AIL3 >= AUTOTOPIC_TIME) {
    AIL3 = 0;
    do_autotopics ();
  }
  AIL5 += AIL;
  if (AIL5 >= 600) {
#ifdef	ANTI_IDLE
    S ("PRIVMSG ! :\2\n");
#endif
    AIL5 = 0;
  }
  if (AIL2 >= 300) {
    AIL2 = 0;
#if STATUS == 1
    S ("LUSERS\n");
#endif
    S ("JOIN %s\n", CHAN);
    S ("MODE %s %s\n", Mynick, DEFAULT_UMODE);
    reset_ ();
    save_changes ();
  }
}

void
sig_segv (int notUsed)
{
  long uptime2 = 0, p = 0;
  uptime2 = time (NULL) - uptime;
  printf
    ("ERROR! Aborting program. (SIG_SEGV) Uptime: %d hour%s, %d min%s\n",
     (int) (uptime2 / 3600), (uptime2 / 3600 == 1) ? "" : "s",
     (int) ((uptime2 / 60) % 60),
     ((uptime2 / 60) % 60) == 1 ? "" : "s");
  Snow
    ("QUIT :Caught SIG_SEGV! Aborting connection. Uptime: %d hour%s, %d min%s\n",
     uptime2 / 3600, uptime2 / 3600 == 1 ? "" : "s",
     (uptime2 / 60) % 60, (uptime2 / 60) % 60 == 1 ? "" : "s");

  db_sleep (2);
  p = getpid ();
  if (fork () > 0) {
    log ("error.log",
	 "Caught SIGSEGV.. Sent kill -3 and kill -9...\n");
    kill (p, 3);
    kill (p, 9);
  }
  db_sleep (1);
  exit (0);
}

int
get_sendq_count (long toggle)
{
  struct sendq *c;
  long i = 0, x = 0;
  c = sendqhead;

  while (c != NULL) {
    i++;
    if (c->data[0] == 'P' && c->data[1] == 'R' && c->data[2] == 'I')
      x++;
    else if (c->data[0] == 'N' && c->data[1] == 'O'
	     && c->data[2] == 'T') x++;
    c = c->next;
  }
  if (toggle == 1)
    clear_sendq (i, 1);
  if (toggle == 2)
    R i;
  if (i < OUTPUT1_COUNT)
    SEND_DELAY = OUTPUT1_DELAY;
  else if (i < OUTPUT2_COUNT)
    SEND_DELAY = OUTPUT2_DELAY;
  else
    SEND_DELAY = OUTPUT3_DELAY;
  if (x > OUTPUT_PURGE_COUNT)
    clear_sendq (x, 0);
  R i;
}

void
clear_sendq (long count, long toggle)
{
  long i = 0;
  i = count;
  while (i > 1) {
    i--;
    del_sendq (1);
  }
  send_tog = 1;
  if (toggle != 1)
    L090 (CHAN, count);
}

void
init_bot ()
{
  get_sendq_count (1);
  Snow ("NICK %s\n", Mynick);
  strlwr (UID);
  Snow ("USER %s %d %d :%s \2%d\2\n", UID, time (NULL), time (NULL),
	REALNAME, NUM_HELPER);
}

/**
 * TODO: No function should ever be this long...move to the Command Pattern
 */
void
parse (char *line)
{
  char *s, *s1, *s2, *s3, *cmd, *ptr;
  long TOG = 0, seen_value = 0;

  LastInput = 0;
  if (DebuG == 1)
    printf ("IN :%s\n", line);
#ifdef	DEBUG2
  log ("darkbot_debug.log", "IN :%s\n", line);
#endif
  stripline (line);
  s = strtok (line, " ");
  if (stricmp (s, "PING") == 0) {
    s1 = strtok (NULL, " ");
    Snow ("PONG %s\n", s1);
  }
  else if (stricmp (s, "ERROR") == 0) {
    s1 = strtok (NULL, "");
    if (s1 != NULL) {
      if (strstr (s1, "Excess Flood") != NULL) {
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
      else if (strstr (s1, "throttled") != NULL) {
	gs26 ();
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
      else if (strstr (s1, "oo many c") != NULL) {
	gs26 ();
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
      else if (strstr (s1, "o more c") != NULL) {
	gs26 ();
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
      else {
	S ("QUIT :Caught ERROR from %s :(\n", BS);
	db_sleep (5);
	gs26 ();
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
    }
  }
  else if (strstr (s, "!") == NULL) {	/* From Server */
    cmd = strtok (NULL, " ");
    if (stricmp (cmd, "004") == 0) {	/* Connected! */
      save_changes ();
      s2 = strtok (NULL, " ");	/* Copy the current nick */
      strcpy (Mynick, s2);
      sprintf (NICK_COMMA, "%s,", Mynick);
      sprintf (COLON_NICK, "%s:", Mynick);
      sprintf (BCOLON_NICK, "%s\2:\2", Mynick);
      S ("JOIN %s\n", CHAN);
      db_sleep (2);
      s2 = strtok (NULL, " ");	/* Got server name */
      printf ("%s has connected to %s! [%d pid]\n", Mynick, s2,
	      getpid ());
    }
    else if (stricmp (cmd, "315") == 0) {
#if DISPLAY_SYNC == 1
      s2 = strtok (NULL, " ");	/*mynick */
      strcpy (Mynick, s2);
      s2 = strtok (NULL, " ");	/* chan */
      S ("PRIVMSG %s :Sync with %s completed.\n", s2, s2);
#endif
    }
    else if (stricmp (cmd, "311") == 0) {
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, " ");
      strcpy (g_host, s1);
    }
    else if (stricmp (cmd, "319") == 0) {
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, " ");
      s2 = strtok (NULL, "");
      if (*s2 == ':')
	s2++;
      strlwr (s2);
      if (strstr (s2, "arez") != NULL)
	TOG = 1;
      if (strstr (s2, "kidd") != NULL)
	TOG = 1;
      if (strstr (s2, "hack") != NULL)
	TOG = 1;
      if (strstr (s2, "sex") != NULL)
	TOG = 1;
      if (strstr (s2, "fuck") != NULL)
	TOG = 1;
      if (strstr (s2, "porn") != NULL)
	TOG = 1;
      if (strstr (s2, "pic") != NULL)
	TOG = 1;
      if (TOG == 1) {
	if (strlen (s2) > 160)
	  s2 = '\0';
	S ("NOTICE @%s :%s is on \2%s\2\n", g_chan, s1, s2);
	R;
      }
    }
    else if (stricmp (cmd, "432") == 0 || stricmp (cmd, "461") == 0
	     || stricmp (cmd, "468") == 0) {	/* Invalid nick/user */
      s2 = strtok (NULL, "");
      printf ("Server Reported error %s\n\nDarkbot exiting.\n", s2);
      db_sleep (2);
      exit (0);
    }
    else if (stricmp (cmd, "376") == 0) {
      raw_now ("PERFORM");	/* End of MOTD, run
				 * perform_now() */
    }
    else if (stricmp (cmd, "482") == 0) {
#if BITCH_ABOUT_DEOP == 1
      s2 = strtok (NULL, " ");	/* mynick */
      strcpy (Mynick, s2);
      s2 = strtok (NULL, " ");	/* chan */
      S ("PRIVMSG %s :%s\n", s2, BITCH_DEOP_REASON);
#endif
      raw_now ("DEOP");		/* Deoped, run list of
				 * commands.. */
    }
    else if (stricmp (cmd, "352") == 0) {
      s2 = strtok (NULL, "");
      parse_who (s2);
#if STATUS == 1
    }
    else if (stricmp (cmd, "252") == 0) {
      s2 = strtok (NULL, "");
      parse_252 (s2);
    }
    else if (stricmp (cmd, "404") == 0 || stricmp (cmd, "475") == 0
	     || stricmp (cmd, "474") == 0 || stricmp (cmd, "473") == 0) {	/* Can't join? */
      s2 = strtok (NULL, " ");
      s2 = strtok (NULL, " ");
      db_sleep (5);
      S ("JOIN %s\n", s2);
    }
    else if (stricmp (cmd, "251") == 0) {
      s2 = strtok (NULL, "");
      parse_251 (s2);
    }
    else if (stricmp (cmd, "255") == 0) {
      s2 = strtok (NULL, "");
      parse_255 (s2);
#endif
    }
    else if (stricmp (cmd, "433") == 0) {
      s2 = strtok (NULL, " ");
      if (*s2 != '*') {
	strcpy (Mynick, s2);
	sprintf (NICK_COMMA, "%s,", Mynick);
	sprintf (COLON_NICK, "%s:", Mynick);
	sprintf (BCOLON_NICK, "%s\2:\2", Mynick);
	s3 = strtok (NULL, " ");
      }
      else {
	Snow ("NICK %s%d\n", Mynick, xtried);
	xtried++;
	if (xtried > 15)
	  Snow ("NICK _`^%s%d\n", Mynick, xtried);
	if (xtried > 5)
	  Snow ("NICK _%s%d\n", Mynick, xtried);
      }
    }
  }
  else {			/* Info from user */
    if (*s == ':')		/* Remove the colon prefix */
      s++;
    cmd = strtok (NULL, " ");	/* Read in command  */
    if (stricmp (cmd, "NOTICE") == 0) {
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, " ");
      s1 = strtok (NULL, "-");
      s1 = strtok (NULL, " ");
      s2 = strtok (NULL, " ");
    }
    else if (stricmp (cmd, "PRIVMSG") == 0) {	/* PRIVMSG  */
      s1 = strtok (NULL, " ");	/* Target */
      s2 = strtok (NULL, "");	/* Rest  */
#if	LOG_PRIVMSG == 1
      if (*s1 != '#' && *s1 != '&') {
	log (privmsg_log, "[%s] %s %s %s\n", date (), s, s1, s2);
      }
#endif
      if (*s1 == '#' || *s1 == '&' || *s1 == '+')
	if (do_lastcomm (s, s1, s2) == 1)
	  R;
      chanserv (s, s1, s2);	/* Process PRIVMSG commands */
    }
    else if (stricmp (cmd, "KILL") == 0) {
      s1 = strtok (NULL, " ");	/* Kill nick */
      if (stricmp (s1, Mynick) == 0) {
	do_quit (s1, 3);	/* delete all users from ram since I'm gone */
	gs26 ();
	socketfd = get_connection (BS, VHOST, BP);
	init_bot ();
      }
    }
    else if (stricmp (cmd, "KICK") == 0) {
      s1 = strtok (NULL, " ");	/* #chan */
      s2 = strtok (NULL, " ");	/* Who got kicked? */
      if (stricmp (s2, Mynick) == 0) {	/* Rejoin if I was
					 * kicked */
	do_quit (s1, 2);
	db_sleep (5);
	S ("JOIN %s\n", s1);
	S ("PRIVMSG %s :%s\n", s1, COMPLAIN_REASON);
      }
      else
	delete_user (s2, s1);
    }
    else if (stricmp (cmd, "INVITE") == 0) {
      s1 = strtok (NULL, " ");	/* Mynick */
      s2 = strtok (NULL, " ");	/* Target */
      if (*s2 == ':')
	s2++;
      if (stricmp (s2, CHAN) == 0)
	S ("JOIN %s\n", s2);
    }
    else if (stricmp (cmd, "PART") == 0) {
      s1 = strtok (NULL, " ");	/* Target */
      if ((ptr = strchr (s, '!')) != NULL)
	*ptr++ = '\0';
      strlwr (ptr);
      if (stricmp (s, Mynick) != 0)
	delete_user (s, s1);
      else			/* I left, so delete all */
	do_quit (s1, 2);
    }
    else if (stricmp (cmd, "QUIT") == 0) {
      if ((ptr = strchr (s, '!')) != NULL)
	*ptr++ = '\0';
      do_quit (s, 1);
    }
    else if (stricmp (cmd, "MODE") == 0) {
      do_modes (s, strtok (NULL, ""));
    }
    else if (stricmp (cmd, "NICK") == 0) {
      if ((ptr = strchr (s, '!')) != NULL)
	*ptr++ = '\0';
      s1 = strtok (NULL, " ");
      process_nick (s, s1);
    }
    else if (stricmp (cmd, "JOIN") == 0) {
      JOINs++;
      s1 = strtok (NULL, " ");	/* TARGET */
      if (*s1 == ':')
	s1++;
      if ((ptr = strchr (s, '!')) != NULL)
	*ptr++ = '\0';
      strlwr (ptr);
      if (SeeN == 1 && *s1 == '#')
	seen_value = save_seen (s, ptr, s1);
      if (stricmp (s, Mynick) != 0) {
	if (check_permban (ptr, s1, s) == 1)
	  R;
	add_user (s1, s, ptr, 1);
#if DO_WHOIS == 1
	strcpy (g_chan, s1);
	S ("WHOIS %s\n", s);
#endif
	if (check_access (ptr, s1, 0, s) >= 4) {
	  S ("MODE %s +o %s\n", s1, s);
	}
	else if (check_access (ptr, s1, 1, s) >= 1) {
	  S ("MODE %s +v %s\n", s1, s);
#if HELP_GREET == 1
	}
	else if (check_access (ptr, s1, 0, s) >= 1) {
	  return;		/* don't greet if the guy has
				 * access (and no setinfo) */
	}
	else if (stricmp (s1, CHAN) == 0) {
	  if (SeeN == 1) {
	    if (seen_value == 0)	/* don't show people the
					   * notice every join */
	      if (setinfo_lastcomm (s1) == 0)	/* don't do it if you just did it! */
		L102 (s, s1, s, *CMDCHAR);
	  }
	  else {
	    if (setinfo_lastcomm (s1) == 0)
	      L102 (s, s1, s, *CMDCHAR);
	  }
	}
#else
	}
#endif
      }
      else
	S ("WHO %s\n", s1);	/* Check who is in the
				 * chan */
    }				/* JOIN */
  }
}


long
setinfo_lastcomm (char *rest)
{				/* Disallows joins to more than one channel (or the same chan)
				 * to artifically raise join counters */
  long c_uptime = 0;

  if (stricmp (rest, slc1) == 0)
    R 1;			/* don't reply if already asked in LASTCOMM_TIME sec */
  if (stricmp (rest, slc2) == 0)
    R 1;
  if (stricmp (rest, slc3) == 0)
    R 1;
  if (stricmp (rest, slc4) == 0)
    R 1;

  if (*slc1 == '0') {		/* init lastcomms */
    strcpy (slc1, rest);
    slcn1 = time (NULL);
  }
  if (*slc2 == '0') {
    strcpy (slc2, rest);
    slcn2 = time (NULL);
  }
  if (*slc3 == '0') {
    strcpy (slc3, rest);
    slcn3 = time (NULL);
  }
  if (*slc4 == '0') {
    strcpy (slc4, rest);
    slcn4 = time (NULL);
  }
  if ((c_uptime = time (NULL) - slcn1) > SLASTCOMM_TIME) {	/* reinit old data */
    slcn1 = 0;
    *slc1 = '0';
  }
  if ((c_uptime = time (NULL) - slcn2) > SLASTCOMM_TIME) {
    slcn2 = 0;
    *slc2 = '0';
  }
  if ((c_uptime = time (NULL) - slcn3) > SLASTCOMM_TIME) {
    slcn3 = 0;
    *slc3 = '0';
  }
  if ((c_uptime = time (NULL) - slcn4) > SLASTCOMM_TIME) {
    slcn4 = 0;
    *slc4 = '0';
  }
  strcpy (slc4, slc3);		/* no matches, move em on down */
  strcpy (slc3, slc2);
  strcpy (slc2, slc1);
  strcpy (slc1, rest);
  slcn4 = slcn3;
  slcn3 = slcn2;
  slcn2 = slcn1;
  slcn1 = time (NULL);
  R 0;
}

long
do_lastcomm (char *nick, char *target, char *rest)
{
  long c_uptime = 0;


  if (stricmp (rest, lc1) == 0)
    R 1;			/* don't reply if already asked in LASTCOMM_TIME sec */
  if (stricmp (rest, lc2) == 0)
    R 1;
  if (stricmp (rest, lc3) == 0)
    R 1;
  if (stricmp (rest, lc4) == 0)
    R 1;

  if (*lc1 == '0') {		/* init lastcomms */
    strcpy (lc1, rest);
    lcn1 = time (NULL);
  }
  if (*lc2 == '0') {
    strcpy (lc2, rest);
    lcn2 = time (NULL);
  }
  if (*lc3 == '0') {
    strcpy (lc3, rest);
    lcn3 = time (NULL);
  }
  if (*lc4 == '0') {
    strcpy (lc4, rest);
    lcn4 = time (NULL);
  }
  if ((c_uptime = time (NULL) - lcn1) > LASTCOMM_TIME) {	/* reinit old data */
    lcn1 = 0;
    *lc1 = '0';
  }
  if ((c_uptime = time (NULL) - lcn2) > LASTCOMM_TIME) {
    lcn2 = 0;
    *lc2 = '0';
  }
  if ((c_uptime = time (NULL) - lcn3) > LASTCOMM_TIME) {
    lcn3 = 0;
    *lc3 = '0';
  }
  if ((c_uptime = time (NULL) - lcn4) > LASTCOMM_TIME) {
    lcn4 = 0;
    *lc4 = '0';
  }
  strcpy (lc4, lc3);		/* no matches, move em on down */
  strcpy (lc3, lc2);
  strcpy (lc2, lc1);
  strcpy (lc1, rest);
  lcn4 = lcn3;
  lcn3 = lcn2;
  lcn2 = lcn1;
  lcn1 = time (NULL);
  R 0;
}

#if STATUS == 1
void
parse_252 (char *s)
{
  char *tmp;
  int numb = 0;

  tmp = strtok (s, " ");
  tmp = strtok (NULL, " ");
  sscanf (tmp, "%d", &numb);
  IRCOPS = numb;
}
#endif

#if STATUS == 1
void
parse_251 (char *s)
{
  char *tmp;
  int numb = 0, r = 0, i = 0;

  /*- Read and chuck useless data from line 'b' -*/
  tmp = strtok (s, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  sscanf (tmp, "%d", &r);
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  sscanf (tmp, "%d", &i);
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  sscanf (tmp, "%d", &numb);
  NUM_SERV = numb;
  G_USERS = r + i;
}
#endif

#if STATUS == 1
void
parse_255 (char *s)
{
  char *tmp, Stat[1];
  int numb = 0, pre_CLIENTS = 0;

  /* test321 :I have 1313 clients and 1 servers */
  strlwr (s);
  tmp = strtok (s, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  tmp = strtok (NULL, " ");
  numb = atoi (tmp);
  pre_CLIENTS = L_CLIENTS;
  L_CLIENTS = numb;
  if (L_CLIENTS < pre_CLIENTS) {
    strcpy (Stat, "-");
    pre_CLIENTS = pre_CLIENTS - L_CLIENTS;
  }
  else {
    strcpy (Stat, "+");
    pre_CLIENTS = L_CLIENTS - pre_CLIENTS;
  }
  sprintf (tmp, "%3.2f",
	   (float) (((float) L_CLIENTS / (float) G_USERS) * 100));
#if PLAY == 1
  if (pre_CLIENTS == 0 || pre_CLIENTS == L_CLIENTS) {
    S
      ("PRIVMSG %s :!SENDQ %d srvs, %d ops, %d users (%s%% of %d, %ld avg)\n",
       PBOT, NUM_SERV, IRCOPS, L_CLIENTS, tmp, G_USERS,
       G_USERS / NUM_SERV);
  }
  else
    S
      ("PRIVMSG %s :!SENDQ %d srvs, %d ops, %d users [%c%2d] (%s%% of %d, %ld avg)\37\n",
       PBOT, NUM_SERV, IRCOPS, L_CLIENTS, Stat[0], pre_CLIENTS, tmp,
       G_USERS, G_USERS / NUM_SERV);
  log (".ubcount", "%d\n%d\n0\n0\n", L_CLIENTS, L_CLIENTS);
  rename (".ubcount",
	  "/usr/local/apache/htdocs/usage/userbase/userbase.dat");
  log (".glcount", "%d\n%d\n0\n0\n", G_USERS, G_USERS);
  rename (".glcount",
	  "/usr/local/apache/htdocs/usage/global/global.dat");
#else
  if (pre_CLIENTS == 0 || pre_CLIENTS == L_CLIENTS) {
    S
      ("PRIVMSG %s :\1ACTION \37(\37%2d servers\37)\37: %2d opers + \2%4d\2 users \37(\37%s%% %5d global \2!\2 %3ld avg\37)\37\1\n",
       CHAN, NUM_SERV, IRCOPS, L_CLIENTS, tmp, G_USERS,
       G_USERS / NUM_SERV);
  }
  else
    S
      ("PRIVMSG %s :\1ACTION \37(\37%2d servers\37)\37: %2d opers + \2%4d\2 users [\37%c%2d\37] \37(\37%s%% %5d global \2!\2 %3ld avg\37)\37\1\n",
       CHAN, NUM_SERV, IRCOPS, L_CLIENTS, Stat[0], pre_CLIENTS, tmp,
       G_USERS, G_USERS / NUM_SERV);
#endif
}
#endif

void
load_helpers ()
{
  FILE *fp;
  char b[700], *user_host, *greetz, *numb_join, *chan, *w_level,
    *pass;
  long num_join = 0, i = 0, level = 0, T = 0;

  if ((fp = fopen (HELPER_LIST, "r")) == NULL) {
    printf ("Unable to open %s! Aborting connection.\n", HELPER_LIST);
    printf ("Please run ./configure to setup your darkbot.\n");
    exit (0);
  }
#ifndef	WIN32
  printf ("Loading %s file\n", HELPER_LIST);
#endif
  while (fgets (b, 700, fp)) {
    if (b == NULL)
      continue;
    stripline (b);
    if (*b == '/')
      continue;
    i++;
    printf (".");
    fflush (stdout);
    chan = strtok (b, " ");
    if (chan == NULL)
      continue;
    user_host = strtok (NULL, " ");
    if (user_host == NULL)
      continue;
    w_level = strtok (NULL, " ");
    if (w_level == NULL)
      continue;
    numb_join = strtok (NULL, " ");
    if (numb_join == NULL)
      continue;
    pass = strtok (NULL, " ");
    if (pass == NULL) {
      pass = "0";		/* duh */
    }
    greetz = strtok (NULL, "");
    if (greetz == NULL)
      greetz = "I haven't used \2SETINFO\2 yet!";
    if (w_level != NULL)
      level = atoi (w_level);
    else
      level = 1;
    if (numb_join != NULL)
      num_join = atoi (numb_join);
    else
      num_join = 0;
    if (strlen (pass) > 25)
      pass[25] = '\0';
    if (DebuG == 1)
      printf ("loading helperlist: %s %s l:%d j:%d %s\n", chan,
	      user_host, (int) level, (int) num_join, greetz);
    if (T == 1) {
      add_helper ("#*", chan, level, num_join, greetz, pass);
    }
    else
      add_helper (chan, user_host, level, num_join, greetz, pass);
  }
  if (T == 1) {
    printf
      ("\n\nYour userlist was in the older userlist format, I have\n");
    printf
      ("Converted them - userlists now include a #chan to give\n");
    printf
      ("user's access on, thus you can now give a user in #chan1\n");
    printf
      ("level 2 access, to get ops, but while your bot also has\n");
    printf
      ("ops in #chan2, that user cannot get ops in #chan2 from your\n");
    printf
      ("bot. The default I've set it to is #* (all chans), feel free\n");
    printf ("to update your user list as you see fit.\n\n");
    db_sleep (5);
  }
  else
    printf ("\nLoaded %d helpers.\n", (int) i);
  fclose (fp);
  save_changes ();
  if (DebuG == 1)
    db_sleep (2);
}

/**
 * Add a channel helper.
 * 6/22/00 Dan
 * n now initialized where declared
 * All pointer arguments now received as pointer to const data.
 */
void
add_helper (const char *chan, const char *uh, long level,
	    size_t num_join, const char *greetz, const char *pass)
{
  struct helperlist *n = 0;

  n = (struct helperlist *) malloc (sizeof (struct helperlist));
  if (n == NULL) {
    log ("error.log", "AHHH! No ram left! in add_helper!\n");
    R;
  }

  memset (n, 0, sizeof (struct helperlist));
  NUM_HELPER++;

  if (chan[0] == '#') {
    strcpy (n->chan, chan);
  }
  else {
    strcpy (n->chan, "#*");
  }

  strcpy (n->uh, uh);
  strlwr (n->uh);

  strcpy (n->pass, pass);
  n->num_join = num_join;
  n->level = level;

  strncpy (n->greetz, greetz,
	   min (sizeof (n->greetz) - 1, strlen (greetz)));

  n->next = helperhead;
  helperhead = n;
}


/**
 * 6/22 Dan
 * - Changed DATA to be 512 bytes, a power of 2
 * - DATA now initialized properly
 * - c is now a pointer to const, this is a read only method
 * - c is now initialized where declared
 * - Changed type of i, x to size_t, these variables should be
 *   unsigned.
 * - Changed while loop to for loop.
 * - Changed reinitialization of DATA to use memset()
 */
void
show_banlist (const char *nick)
{
  char DATA[512] = { 0 };
  size_t i = 0;
  size_t x = 0;
  const struct permbanlist *c = 0;

  for (c = permbanhead; c != NULL; c = c->next) {
    i++;
    ++x;
    sprintf (DATA, "%s %s:%d", DATA, c->uh, (int) c->counter);
    if (i > 8) {
      S ("NOTICE %s :%s\n", nick, DATA);
      i = 0;
      memset (DATA, 0, sizeof (DATA));
      db_sleep (2);
    }
  }
  S ("NOTICE %s :%s\n", nick, DATA);
  S ("NOTICE %s :End of PERMBAN list; %d ban%s found.\n", nick, x,
     (x == 1) ? "" : "s");
}

/**
 * Output the helper list to a nickname.
 * 6/22, Dan:
 * - Changed helperlist* c to be a pointer to const data
 * - Changed initialization of DATA, and size to be a
 *   power of 2
 * - Added initialization of c
 * - Changed while loop to for loop
 * - Changed types of i, x to size_t since they should be
 *   unsigned.
 * - Added reinitialization of DATA using memset() (changed from
 *   strcpy(DATA,""))
 */
void
show_helper_list (const char *nick, long level)
{
  char DATA[512] = { 0 };
  size_t i = 0, x = 0;
  const struct helperlist *c = 0;

  for (c = helperhead; c != NULL; c = c->next) {
    i++;
    x++;
    if (level == 0) {
      i++;
      sprintf (DATA, "%s %s[%s:%d:%d]", DATA, c->uh, c->chan,
	       (int) c->level, (int) c->num_join);
    }
    else {
      if (level == c->level) {
	i++;
	sprintf (DATA, "%s %s[%s:%d:%d]", DATA, c->uh, c->chan,
		 (int) c->level, (int) c->num_join);
      }
    }
    if (i > 6) {
      S ("NOTICE %s :%s\n", nick, DATA);
      i = 0;
      memset (DATA, 0, sizeof (DATA));
      db_sleep (2);
    }
  }				/* for() */

  S ("NOTICE %s: %s\n", nick, DATA);
  S ("NOTICE %s :End of Helper Userlist; %d user%s found.\n", nick, x,
     (x == 1) ? "" : "s");
}

int
match_wild (const char *pattern, const char *str)
{
  char c;
  const char *s;

  for (;;) {
    switch (c = *pattern++) {
    case 0:
      if (!*str)
	R 1;
      R 0;
    case '?':
      ++str;
      break;
    case '*':
      if (!*pattern)
	R 1;
      s = str;
      while (*s) {
	if (*s == *pattern && match_wild (pattern, s))
	  R 1;
	++s;
      }
      break;
    default:
      if (*str++ != c)
	R 0;
      break;
    }				/* switch */
  }
}


#ifndef WIN32
int
stricmp (const char *s1, const char *s2)
{
  return strcasecmp (s1, s2);
}
#else
int
stricmp (const char *s1, const char *s2)
{
  register int c;

  while ((c = tolower (*s1)) == tolower (*s2)) {
    if (c == 0)
      return 0;
    s1++;
    s2++;
  }
  if (c < tolower (*s2))
    return -1;
  return 1;
}
#endif


/**
 * Removed trailing newline and carriage returns.
 * 6/22/00 Dan
 * Rewrote to be more efficient, reduced from O(2n) to O(n)
 */
void
stripline (char *ptr)
{
  for (; ptr && *ptr; ++ptr) {
    if ('\r' == *ptr || '\n' == *ptr) {
      *ptr = 0;
      return;
    }
  }
}


int
Send ()
{
  struct sendq *c;
  char output[610];
  c = sendqhead;

  get_sendq_count (0);
  if (c == NULL) {
    send_tog = 0;
    R - 1;
  }
  if (DebuG == 1)
    printf ("OUT: %s\n", c->data);
#ifdef	DEBUG2
  log ("darkbot_debug.log", "OUT: %s\n", c->data);
#endif
  strcpy (output, c->data);
  del_sendq (0);
  R (writeln (output));
}


/** 
 * Write a character array to a socket connection.
 * 6/22/00 Dan
 * Method argument now pointer to const data.
 */
int
writeln (const char *b)
{
  return (write (socketfd, b, strlen (b)) < 0) ? 0 : 1;
}


int
get_connection (const char *hostname, const char *vhostname, int port)
{
  struct sockaddr_in sa;
  struct hostent *hp;
  int sckfd, f = 1;

  if (vhostname == NULL)
    f = 0;
  else if (strlen (vhostname) < 1)
    f = 0;
  if ((sckfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    R (-1);
  memset (&sa, 0, sizeof (struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = (f ? inet_addr (vhostname) : INADDR_ANY);
  if ((INADDR_NONE == sa.sin_addr.s_addr) && f) {
    hp = gethostbyname (vhostname);
    if (hp) {
      bcopy (hp->h_addr, (char *) &sa.sin_addr, hp->h_length);
    }
    else {
      sa.sin_addr.s_addr = INADDR_ANY;
    }
  }
  if (bind
      (sckfd, (struct sockaddr *) &sa,
       sizeof (struct sockaddr_in)) < 0) {
    R (-9);
  }
  memset (&sa, 0, sizeof (struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_port = htons (port);
  sa.sin_addr.s_addr = inet_addr (hostname);
  setsockopt (sckfd, SOL_SOCKET, SO_LINGER, 0, 0);
  setsockopt (sckfd, SOL_SOCKET, SO_REUSEADDR, 0, 0);
  setsockopt (sckfd, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
  if (INADDR_NONE == sa.sin_addr.s_addr) {
    if ((hp = gethostbyname (hostname)) == NULL) {
      errno = ECONNREFUSED;
      printf ("Can't find hostname: %s\n", hostname);
      R (-1);
    }
    memcpy (&sa.sin_addr, hp->h_addr, hp->h_length);
  }
  if (connect (sckfd, (struct sockaddr *) &sa, sizeof (sa)) < 0) {
    close (sckfd);
    printf ("\nUnable to make connection to host `%s:%d'\n\n", BS,
	    (int) BP);
    R (-1);
  }
  R (sckfd);
}

int
readln ()
{
  char ch;
  int i = 0;

  do {
    if (read (socketfd, &ch, 1) < 1)
      R (0);
    if (ch >= 32 || ch <= 126)
      if (i < 524 - 1)
	L[i++] = ch;
  }
  while (ch != '\n');
  L[i] = '\0';
  R 1;
}

void
log (const char *filename, const char *format, ...)
{
  va_list arglist;
  FILE *fp;

  fp = fopen (filename, "a");
  if (NULL == fp) {
    /* I guess there's no sense in trying to log the error :) */
    return;
  }

  va_start (arglist, format);
  vfprintf (fp, format, arglist);
  va_end (arglist);
  fclose (fp);
}

/**
 * Convert a character array to all lowercase.
 * 6/23/00 Dan:
 * - Rewrote to be more compact and a bit more efficient
 */
char *
strlwr (char *buf)
{
  char *ptr = buf;
  for (; ptr && *ptr; ++ptr) {
    *ptr = tolower (*ptr);
  }
  return buf;
}

void
trailing_blanks (char *str)
{
  int i;

  if (str == NULL)
    R;
  for (i = strlen (str); i > 0; i--) {
    if (isspace (str[i - 1]))
      str[i - 1] = '\0';
    else
      R;
  }
}


void
parse_server_msg (fd_set * read_fds)
{
  if (FD_ISSET (socketfd, read_fds)) {
    if (readln () > 0) {
      NUMLINESSEEN++;
      parse (L);
    }
    else {
      close (socketfd);
    }
  }
}


long
f_f (char *host)
{
  int i;
  for (i = 0; i < fc; i++)
    if (!strcasecmp (ood[i].host, host))
      R i;
  R - 1;
}

void
get_s ()
{
  char temp[20];
  long i = 0;
  i = strlen (rp391);
  while (i > 0) {
    i--;
    sprintf (temp, "%s%c", dbVersion, rp391[i]);
    strcpy (dbVersion, temp);
  }
}

void
a_f (char *host)
{
  if (++fc > 100)
    fc = 0;
  fc--;
  strcpy (ood[fc].host, host);
  ood[fc].time = time (NULL);
  ood[fc].count = 0;
  ood[fc].value = false;
  fc++;
}

void
reset_ ()
{
  int i;
  for (i = 0; i < fc; i++) {
    if (ood[i].value && (time (NULL) - ood[i].time) > rt) {
      ood[i].count = 0;
      ood[i].time = time (NULL);
      ood[i].value = false;
      ood[i].kick = 0;
    }
  }
}

char *
date ()
{
  time_t timer;

  time (&timer);
  strcpy (strbuff, ctime (&timer));
  stripline (strbuff);
  return strbuff;
}

/** 
 * Allocate a new character array.  Copy into it at most
 * maxBytes bytes.
 */
char *
db_strndup (const char *dupMe, size_t maxBytes)
{
  char *ptr = 0;
  char *retMe = 0;

  /* Configure maxBytes to be the number of bytes to copy */
  maxBytes = min (strlen (dupMe), maxBytes);

  /* Allocate the return buffer. */
  retMe = (char *) malloc (maxBytes + 1);

  /* Was the allocation successful? */
  if (NULL == retMe) {
    return NULL;
  }

  /*
   * ptr will point to the byte to which to copy the next
   * source byte.
   */
  ptr = retMe;

  /*
   * Continue while dupMe is valid and we are < maxBytes number
   * of bytes copied. This is typecase here because size_t is
   * unsigned, so comparing against > 0 *should* produce a
   * warning :)
   */
  while (dupMe && (int) maxBytes-- > 0) {
    *ptr++ = *dupMe++;
  }

  /* Be sure to NULL terminate the array */
  *ptr = 0;

  return retMe;
}

/**
 * Output information about the bot's database to a target.
 * 6/22 Dan:
 * - Changed both method arguments to be pointers to const data,
 *   this is a read only method.
 */
void
show_info2 (const char *target, const char *source)
{
  S ("PRIVMSG %s :%s, src: %s (%d lines of code), compiled @ %s. "
     "I have processed %ld lines of text since startup...\n", target,
     source, __FILE__, __LINE__, __DATE__, NUMLINESSEEN);
}
