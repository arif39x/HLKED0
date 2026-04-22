/*
	Lithium Remote Administration Tool
	Main Server Source by Olympus
	Copyright (c) 2001 Olympus.

	-= Common Shared Stuff Header =-
*/

#ifndef _SHARED_
#define _SHARED_

// Standard constants

#define LC_VERSION					"v1.00 Beta 5"
#define LC_QUERYPORT				31416

// Clearance levels

#define LC_INVALID					0
#define LC_GUEST					1
#define LC_LOW						2
#define LC_NORMAL					3
#define LC_HIGH						4
#define LC_FULL						5

// Messages received by client

#define LCC_NEGOTIATE				0
#define LCC_INVALIDLOGIN			1
#define LCC_PLUGINMSG				5
#define LCC_PINGREPLY				10
#define LCC_CONSOLEMSG				11

#define LCC_DIRLISTING_ENTITY		20
#define LCC_DIRLISTING_BEGIN		21
#define LCC_DIRLISTING_INVALID		22

#define LCC_FILE_EDITREAD			30
#define LCC_FILE_EDITWRITE			31

#define LCC_DRIVELISTING_ENTITY		40
#define LCC_DRIVELISTING_BEGIN		41
#define LCC_DRIVELISTING_END		42

#define LCC_PROCLISTING_ENTITY		50
#define LCC_PROCLISTING_BEGIN		51
#define LCC_PROCLISTING_END			52
#define LCC_MODLISTING_ENTITY		60
#define LCC_MODLISTING_BEGIN		61
#define LCC_MODLISTING_END			62

#define LCC_WINDOWLISTING_ENTITY	70
#define LCC_WINDOWLISTING_BEGIN		71
#define LCC_WINDOWLISTING_END		72
#define LCC_APPLISTING_ENTITY		73
#define LCC_APPLISTING_BEGIN		74
#define LCC_APPLISTING_END			75

#define LCC_REGLISTING_ENTITY		80
#define LCC_REGLISTING_BEGIN		81
#define LCC_REGLISTING_END			82
#define LCC_REGLISTING_INVALID		83

#define LCC_REGISTRYREAD			84
#define LCC_REGISTRYWRITE			85

#define LCC_MESSAGEBOXREPLY			100

#define LCC_FILESEARCH_ENTITY		110
#define LCC_FILESEARCH_BEGIN		111
#define LCC_FILESEARCH_END			112
#define LCC_FILESEARCH_CHDIR		113

#define LCC_NETSHARESLISTING_ENTITY	120
#define LCC_NETSHARESLISTING_BEGIN	121
#define LCC_NETSHARESLISTING_END	122

#define LCC_CLIENTSLISTING_ENTITY	130
#define LCC_CLIENTSLISTING_BEGIN	131
#define LCC_CLIENTSLISTING_END		132

#define LCC_PLUGINLISTING_ENTITY	150
#define LCC_PLUGINLISTING_BEGIN		151
#define LCC_PLUGINLISTING_END		152

#define LCC_SERVICELIST_ENTITY		500
#define LCC_SERVICELIST_BEGIN		501
#define LCC_SERVICELIST_END			502

#define LCC_DOWNLOADINIT			1000
#define LCC_DOWNLOADERROR			1001
#define LCC_UPLOADINIT				1100
#define LCC_UPLOADERROR				1101

// Messages received by server

#define LSC_NEGOTIATE				0
#define LSC_PLUGINMSG				5
#define LSC_PINGQUERY				10
#define LSC_CLOSESERVER				11
#define LSC_REMOVESERVER			12
#define LSC_RESTARTSERVER			13

#define LSC_DIRLISTING				20

#define LSC_FILE_EXECUTE			30
#define LSC_FILE_COPY				31
#define LSC_FILE_MOVE				32
#define LSC_FILE_DELETE				33
#define LSC_FILE_CREATEDIR			34
#define LSC_FILE_DELETEDIR			35

#define LSC_FILE_EDITREAD			36
#define LSC_FILE_EDITWRITE			37

#define LSC_DRIVELISTING			40

#define LSC_PROCLISTING				50
#define LSC_PROCTERMINATE			51
#define LSC_PROCPRIORITY			52
#define LSC_MODLISTING				60

#define LSC_WINDOWLISTING			70
#define LSC_APPLISTING				71
#define LSC_WINDOWMESSAGE			72

#define LSC_REGISTRYLISTING			80
#define LSC_REGISTRYDELETE			81
#define LSC_REGISTRYCREATEVALUE		82
#define LSC_REGISTRYCREATEKEY		83
#define LSC_REGISTRYREAD			84
#define LSC_REGISTRYWRITE			85

#define LSC_MESSAGEBOX				100

#define LSC_FILESEARCH_START		110
#define LSC_FILESEARCH_STOP			111

#define LSC_NETSHARESLISTING		120

#define LSC_CLIENTSLISTING			130

#define LSC_SHUTDOWNWINDOWS			140

#define LSC_PLUGINLISTING			150
#define LSC_PLUGINADD				151
#define LSC_PLUGINDEL				152

#define LSC_SERVICE_CREATE			500
#define LSC_SERVICE_STOP			501
#define LSC_SERVICE_LIST			502
#define LSC_SERVICE_REMOVE			503
#define LSC_SERVICE_START			504

#define LSC_DOWNLOADINIT			1000
#define LSC_DOWNLOADBEGIN			1001
#define LSC_UPLOADINIT				1100
#define LSC_UPLOADBEGIN				1101

// Server query messages

#define SQ_QUERYREQUEST				0
#define SQ_SCANREQUEST				1

// Client query messages

#define CQ_QUERYREPLY				0
#define CQ_SCANREPLY				1


// Lithium window messages

#define LWM_CLOSE			0
#define LWM_SHOW			1
#define LWM_HIDE			2
#define LWM_ENABLE			3
#define LWM_DISABLE			4
#define LWM_MINIMIZE		5
#define LWM_MAXIMIZE		6
#define LWM_RESTORE			7
#define LWM_BRINGTOFRONT	8
#define LWM_SETCAPTION		9

#endif
