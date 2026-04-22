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

// DlgConnect.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgConnect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgConnect 


CDlgConnect::CDlgConnect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConnect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgConnect)
	m_sIP = _T("127.0.0.1");
	m_iPort = 4000;
	m_sPass = _T("Skydance");
	m_iPassword = 0;
	m_bUpEnvir = TRUE;
	//}}AFX_DATA_INIT
}


void CDlgConnect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgConnect)
	DDX_Text(pDX, IDC_EDITIP, m_sIP);
	DDX_Text(pDX, IDC_EDITPORT, m_iPort);
	DDX_Text(pDX, IDC_EDITPASS, m_sPass);
	DDX_Radio(pDX, IDC_RADIO1, m_iPassword);
	DDX_Check(pDX, IDC_CHECK1, m_bUpEnvir);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgConnect, CDialog)
	//{{AFX_MSG_MAP(CDlgConnect)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgConnect 

void CDlgConnect::OnRadio1() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	GetDlgItem(IDC_EDITPASS)->EnableWindow(FALSE);
	m_iPassword = 0;
}


void CDlgConnect::OnRadio2() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	GetDlgItem(IDC_EDITPASS)->EnableWindow(TRUE);
	m_iPassword = 1;
}
