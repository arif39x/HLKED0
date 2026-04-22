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
// DlgMessageBox.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgMessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgMessageBox 


CDlgMessageBox::CDlgMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgMessageBox)
	m_sMsgText = _T("");
	m_sMsgTitle = _T("");
	m_iMsgOption = 0;
	//}}AFX_DATA_INIT
}


void CDlgMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgMessageBox)
	DDX_Text(pDX, IDC_EMSGTEXT, m_sMsgText);
	DDX_Text(pDX, IDC_EMSGTITLE, m_sMsgTitle);
	DDX_Radio(pDX, IDC_MSGERROR, m_iMsgOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgMessageBox, CDialog)
	//{{AFX_MSG_MAP(CDlgMessageBox)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgMessageBox 
