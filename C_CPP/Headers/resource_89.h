/**********************************************************************************/
// Read this First!
// Skydance 2.291 Beta Release source code for MFC-VC++ 6.0
// by Edrin@gmx.net
//
// http://skd.box.sk
// - USE SKYDANCE/SKYDANCE SOURCE-CODE AND/OR IT`S COMPONENTS ON YOUR OWN RISK -
//
// Introduction:
//	This Source is FREEWARE!
//	SkyServer is a component of Skydance!
//	Skydance is designed to control other Win95/98/NT systems in tcp/ip-nets like Internet and Intranets.
//	Skydance 2.291 Beta Release is a program that will remain "under construction" and i will NEVER be
//	responsible for any HARM or harmfull things that happend because SKYDANCE(or skydance-source) was USED! 
//	That means I do NOT	guarantee for any features and security-thing
//	and i do not guarantee that this program works proper!!!
//	Indeed i guarantee for nothing ;)
//good luck, Edrin
/**********************************************************************************/
//{{NO_DEPENDENCIES}}
// Microsoft Developer Studio generated include file.
// Used by Skydance.rc
//
#define IDD_ABOUTBOX                    100
#define IDP_SOCKETS_INIT_FAILED         104
#define ID_VIEW_ARRANGE                 127
#define IDR_MAINFRAME                   128
#define IDR_NEWCONTYPE                  129
#define IDD_DLGCONNECT                  130
#define IDD_DLGMSG                      131
#define IDD_DLGFTP                      132
#define IDD_DLGFTPSTATUS                133
#define IDD_DLGRUN                      134
#define IDD_DLGDELETE                   135
#define IDD_DLGCOPY                     136
#define IDB_BMPHD                       138
#define IDB_BMPSMFILES                  139
#define IDD_DLGSPECIAL                  140
#define IDR_MENFILES                    143
#define IDB_BMP_SKYDANCE                147
#define IDD_DLGTASKS                    148
#define IDD_DLGMOVE                     149
#define IDD_DLGNEWFOLDER                150
#define IDR_MENFOLDERS                  151
#define IDD_DLGRENAME                   152
#define IDB_BMPREC                      153
#define IDD_DLGREG                      154
#define IDC_EDITIP                      1000
#define IDC_EDITPORT                    1001
#define IDC_RADIO1                      1002
#define IDC_RADIO2                      1003
#define IDC_EDITPASS                    1004
#define IDC_EMSGTITLE                   1009
#define IDC_EMSGTEXT                    1010
#define IDC_MSGQUESTION                 1013
#define IDC_MSGINFO                     1014
#define IDC_MSGERROR                    1015
#define IDC_MSGWARNING                  1016
#define IDC_EDITCLIENT                  1018
#define IDC_EDITSERVER                  1019
#define IDC_BTUPDOWN                    1020
#define IDC_BTPATH                      1021
#define IDC_STATICFILEN                 1026
#define IDC_STATICSTAT                  1027
#define IDC_STATICGOAL                  1028
#define IDC_STATICKIND                  1029
#define IDC_EDIT1                       1030
#define IDC_EDITTARGET                  1031
#define IDC_EDITSOURCE                  1032
#define IDC_CHECK1                      1034
#define IDC_COMBOACTION                 1037
#define IDC_EDIT2                       1045
#define IDC_EDIT3                       1046
#define IDC_BTCHANGE                    1047
#define IDC_BTACTIONSEND                1048
#define IDC_BTINFO                      1049
#define IDC_BTREMO                      1049
#define IDC_STATICS1                    1051
#define IDC_STATICS2                    1052
#define IDC_STATICS3                    1053
#define IDC_CHECK2                      1054
#define IDC_EDIT4                       1055
#define IDC_STATICS4                    1056
#define IDC_BTSSERVER                   1057
#define IDC_LISTTASKS                   1061
#define IDC_BTTCLOSE                    1062
#define IDC_BTUPDATE                    1064
#define IDC_BTCLOSEALL                  1065
#define IDC_BUTTON1                     1066
#define IDC_BUTTON2                     1067
#define IDC_BUTTON3                     1068
#define IDC_BUTTON4                     1069
#define IDC_BTKLSTART                   1071
#define IDC_BTKLSTOP                    1072
#define IDC_EDITLOGFILE                 1073
#define IDC_EDITDLLFILE                 1074
#define IDC_STATICS5                    1075
#define IDC_STATICS6                    1076
#define IDC_CHECKKEYLOG                 1077
#define IDC_EDITUIN                     1078
#define IDC_STATIC7                     1080
#define IDC_BTREQUEST                   1081
#define IDC_BTSET                       1082
#define IDC_RSTRING                     1083
#define IDC_RBIN                        1084
#define IDC_RDWORD                      1085
#define IDC_BTDELKEY                    1086
#define IDC_BTCOMPINFO                  1087
#define IDC_BTREGLOCK                   1088
#define IDC_LISTREG                     1089
#define IDC_COMBOREGKEY                 1090
#define IDC_EDITVALUENAME               1091
#define ID_BTCONNECT                    32771
#define ID_BTRUN                        32772
#define ID_BUTTON32773                  32773
#define ID_BTREGEDIT                    32774
#define ID_BUTTON32775                  32775
#define ID_BUTTON32776                  32776
#define ID_BTFTP                        32777
#define ID_BTSPECIAL                    32778
#define ID_BTMSG                        32779
#define ID_BTCOPY                       32780
#define ID_BTDELETE                     32781
#define ID_STEALTH_MODE                 32782
#define ID_DISCOVERY_MODE               32783
#define ID_UPDATE_MODE                  32784
#define ID_FOLDERFILE                   32785
#define ID_FOLDER                       32786
#define ID_BTDISCONNECT                 32787
#define ID_CON_DOWNLOAD                 32788
#define ID_CON_COPY                     32789
#define ID_CON_DELETE                   32790
#define ID_CON_EXECUTE_WITHOUTPARAMETER 32791
#define ID_CON_EXECUTE_WITHPARAMETERS   32792
#define ID_CON_COPYPATH                 32793
#define ID_CON_EXECUTE_WITHCOPY         32794
#define ID_BTTASKS                      32795
#define ID_BUTTON32796                  32796
#define ID_BTNEWFOLDER                  32797
#define ID_BTMOVE                       32799
#define ID_LCON_CREATE                  32800
#define ID_LCON_DELETE                  32801
#define ID_LCON_RENAME                  32802
#define ID_CON_MOVE                     32803
#define ID_LCON_COPYPATH                32804
#define ID_CON_PAST                     32805
#define ID_CON_MOVETOBUFF               32806
#define ID_CON_COPYTOBUFFER             32807
#define ID_CON_UPLOAD                   32808
#define ID_LCON_PAST                    32810
#define ID_BTUPDATE                     32811
#define ID_INDICATOR_STAT1              59142
#define ID_INDICATOR_STAT2              59143

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_3D_CONTROLS                     1
#define _APS_NEXT_RESOURCE_VALUE        158
#define _APS_NEXT_COMMAND_VALUE         32812
#define _APS_NEXT_CONTROL_VALUE         1094
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
