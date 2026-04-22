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
// DlgFtp.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgFtp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgFtp 


CDlgFtp::CDlgFtp(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFtp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgFtp)
	m_sFileNameC = _T("");
	m_sFileNameS = _T("");
	m_iFtpPort = 6050;
	m_iDirection = 0;
	//}}AFX_DATA_INIT
	m_bUpDown = TRUE;
}


void CDlgFtp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFtp)
	DDX_Control(pDX, IDC_BTUPDOWN, m_BtUpDown);
	DDX_Text(pDX, IDC_EDITCLIENT, m_sFileNameC);
	DDX_Text(pDX, IDC_EDITSERVER, m_sFileNameS);
	DDX_Text(pDX, IDC_EDITPORT, m_iFtpPort);
	DDX_Radio(pDX, IDC_RADIO1, m_iDirection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgFtp, CDialog)
	//{{AFX_MSG_MAP(CDlgFtp)
	ON_BN_CLICKED(IDC_BTPATH, OnBtpath)
	ON_BN_CLICKED(IDC_BTUPDOWN, OnBtupdown)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgFtp 

void CDlgFtp::OnBtpath() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CFileDialog fileDlg(TRUE);
	if (fileDlg.DoModal() == IDOK)
	{
		m_sFileNameC = fileDlg.GetPathName();
		m_sFileNameS += fileDlg.GetFileName();
		UpdateData(FALSE);
	}
}

void CDlgFtp::OnBtupdown() 
{
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	if(m_bUpDown == TRUE)
	{
		m_BtUpDown.SetWindowText("<-");
		m_bUpDown = FALSE;
	}
	else
	{
		m_BtUpDown.SetWindowText("->");
		m_bUpDown = TRUE;
	}
}


void CDlgFtp::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if(m_bUpDown == FALSE)
	{
		m_BtUpDown.SetWindowText("<-");
	}
	else
	{
		m_BtUpDown.SetWindowText("->");
	}
}
