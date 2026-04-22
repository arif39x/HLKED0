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
// DlgRegEdit.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgRegEdit.h"
#include "SkydanceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgRegEdit 


CDlgRegEdit::CDlgRegEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRegEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRegEdit)
	m_sKey = _T("\\Software\\Microsoft\\Windows\\CurrentVersion");
	m_sValue = _T("");
	m_iValueType = 0;
	m_iComboKey = 2;
	m_sValName = _T("SystemRoot");
	//}}AFX_DATA_INIT
	m_bLock = TRUE;
}


void CDlgRegEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRegEdit)
	DDX_Control(pDX, IDC_LISTREG, m_lstReg);
	DDX_Control(pDX, IDC_BTREGLOCK, m_BtLock);
	DDX_Text(pDX, IDC_EDIT1, m_sKey);
	DDX_Text(pDX, IDC_EDIT2, m_sValue);
	DDX_Radio(pDX, IDC_RSTRING, m_iValueType);
	DDX_CBIndex(pDX, IDC_COMBOREGKEY, m_iComboKey);
	DDX_Text(pDX, IDC_EDITVALUENAME, m_sValName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRegEdit, CDialog)
	//{{AFX_MSG_MAP(CDlgRegEdit)
	ON_BN_CLICKED(IDC_BTREQUEST, OnBtrequest)
	ON_BN_CLICKED(IDC_BTDELKEY, OnBtdelkey)
	ON_BN_CLICKED(IDC_BTSET, OnBtset)
	ON_BN_CLICKED(IDC_BTREGLOCK, OnBtreglock)
	ON_BN_CLICKED(IDC_BTCOMPINFO, OnBtcompinfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgRegEdit 

void CDlgRegEdit::OnBtrequest() 
{
	if(!CheckEdit()) return;
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	
	char sCombo[3];		//HKEY?
	itoa(m_iComboKey, sCombo, 10);
	
	pSkydanceDoc->SendPack("RequestRegKey<" + (CString)sCombo + "<" + m_sKey + "<" + m_sValName + "</");

	m_lstReg.AddString("Requesting: " + m_sKey + m_sValName);
	UpdateData(FALSE);
}

void CDlgRegEdit::OnBtdelkey() 
{
	if(!CheckEdit()) return;
	if(MessageBox("Are you sure you want to delete the Value-field \n" + m_sKey + "\\"+ m_sValName + " ?",
		"Delete Key", MB_OKCANCEL) == IDOK)
	{
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		char sCombo[3];
		itoa(m_iComboKey, sCombo, 10);
		pSkydanceDoc->SendPack("DeleteRegKey<" + (CString)sCombo + "<" + m_sKey + "<" + m_sValName + "</");
		m_lstReg.AddString("Deleting: " +m_sKey );
		UpdateData(FALSE);
	}
}

void CDlgRegEdit::OnBtset() 
{
	if(!CheckEdit()) return;
	if(MessageBox("Are you sure you want to set the Key?\nWrong Values can cause fatal errors!",
		"Edit Key", MB_OKCANCEL) == IDOK)
	{
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		
		char sCombo[3];		//HKEY?
		itoa(m_iComboKey, sCombo, 10);
		
		char sValueType[5]; //DWORD oder String?
		itoa(m_iValueType, sValueType, 10);		

		pSkydanceDoc->SendPack("SetRegKey<" + (CString)sCombo + "<" + 
				m_sKey + "<" + m_sValName + "<" + m_sValue + "<" + sValueType + "</");
		
		m_lstReg.AddString("Creating: " + m_sValName);
		UpdateData(FALSE);
	}
}


BOOL CDlgRegEdit::CheckEdit()
{
	UpdateData(TRUE);
	if(m_sKey == "") {AfxMessageBox("Enter a key first!");return FALSE;}
	else return TRUE;
}

void CDlgRegEdit::OnBtreglock() 
{
	if(m_bLock)
	{
		if(MessageBox("Are you sure? Editing REGISTRY can cause BIG PROBLEMS!",
										"WARNING", MB_OKCANCEL) != IDOK) return;
		GetDlgItem(IDC_BTSET)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTDELKEY)->EnableWindow(TRUE);
		GetDlgItem(IDC_RSTRING)->EnableWindow(TRUE);
		//GetDlgItem(IDC_RBIN)->EnableWindow(TRUE);
		//GetDlgItem(IDC_RDWORD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
		m_bLock = FALSE;
		m_BtLock.SetWindowText("Lock");
	}
	else
	{	

		GetDlgItem(IDC_BTSET)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTDELKEY)->EnableWindow(FALSE);
		GetDlgItem(IDC_RSTRING)->EnableWindow(FALSE);
		//GetDlgItem(IDC_RBIN)->EnableWindow(FALSE);
		//GetDlgItem(IDC_RDWORD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		m_bLock = TRUE;
		m_BtLock.SetWindowText("Unlock");
	}
}

void CDlgRegEdit::OnBtcompinfo() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	CString sMain;
	sMain = "RequestRegKey<2<\\Software\\Microsoft\\Windows\\CurrentVersion";
	m_lstReg.AddString("Requesting SYSTEMINFO");
//	pSkydanceDoc->SendPack("RequestRegKey<2<" + m_sKey + "<" + m_sValName + "</");
	pSkydanceDoc->SendPack(sMain + "<RegisteredOrganization</");
	pSkydanceDoc->SendPack(sMain + "<RegisteredOwner</");
	pSkydanceDoc->SendPack(sMain + "<Version</");
	pSkydanceDoc->SendPack(sMain + "<VersionNumber</");
	pSkydanceDoc->SendPack(sMain + "<ProductKey</");
}
