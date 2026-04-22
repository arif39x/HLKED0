extern char bid[];
extern char myvershun[];
extern char xetumhandle[];
extern char gotopth[];
extern char exename[];
extern char password[];
extern char prefix;
extern char *authost[];
extern int authsize;
extern SERVER sinfo[];
extern int curserver;

extern char main_title[];
extern char threads_title[];
extern char download_title[];
extern char update_title[];
extern char imspread_title[];
extern char pstore_title[];
extern char visit_title[];
extern char botkiller_title[];
extern char ddos_title[];

extern HANDLE ih;

extern char host[MAX_HOSTNAME];

extern DWORD dwstarted;
extern DWORD dwconnected;

extern THREAD threads[MAX_THREADS];

extern HANDLE xetum;

// The login command needs extern for protocol.cpp
extern const char get_auth[];


extern char str_msn_spread[];
extern char str_msn_msg[];
extern char str_msn_stat[];
