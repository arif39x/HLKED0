// client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString		NetMessage;
int			ListenPort;
int			PingInterval;
int			MaxSimVictims;
char		VictimLoginPacket[32];
int			TransferPort;
UINT		CurrentSelection;
CRITICAL_SECTION	cs;


/////////////////////////////////////////////////////////////////////////////
// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	//{{AFX_MSG_MAP(CClientApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientApp construction

CClientApp::CClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientApp object

CClientApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientApp initialization

BOOL CClientApp::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	_snprintf(VictimLoginPacket, sizeof(VictimLoginPacket) - 1, "%s", DEFAULT_VICTIM_LOGIN_PACKET);

	Settings d;
	if (d.DoModal() != IDOK)
		return FALSE;

	//InitializeCriticalSectionAndSpinCount(&cs, 0x80000400);
	InitializeCriticalSection(&cs);

	CClientDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDCLOSE)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
