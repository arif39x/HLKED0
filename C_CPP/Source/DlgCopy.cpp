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
// DlgCopy.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgCopy.h"
#include "SkydanceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgCopy 


CDlgCopy::CDlgCopy(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgCopy::IDD, pParent)
{
	m_sCurrent = "";
	//{{AFX_DATA_INIT(CDlgCopy)
	m_sSource = _T("");
	m_sTarget = _T("");
	//}}AFX_DATA_INIT
}


void CDlgCopy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgCopy)
	DDX_Text(pDX, IDC_EDITSOURCE, m_sSource);
	DDX_Text(pDX, IDC_EDITTARGET, m_sTarget);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgCopy, CDialog)
	//{{AFX_MSG_MAP(CDlgCopy)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgCopy 

void CDlgCopy::OnButton1() 
{
	m_sSource = m_sCurrent;
	UpdateData(FALSE);
}

void CDlgCopy::OnButton2() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();	
	m_sSource = pSkydanceDoc->m_sCopyPathOfFile;
	UpdateData(FALSE);
}

void CDlgCopy::OnButton3() 
{
	m_sTarget = m_sCurrent;
	UpdateData(FALSE);
}

void CDlgCopy::OnButton4() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();	
	m_sTarget = pSkydanceDoc->m_sCopyPathOfFile;
	UpdateData(FALSE);
}
