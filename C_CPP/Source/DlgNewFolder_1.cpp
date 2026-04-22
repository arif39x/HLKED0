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
// DlgNewFolder.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgNewFolder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgNewFolder 


CDlgNewFolder::CDlgNewFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewFolder)
	m_sFolderName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgNewFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewFolder)
	DDX_Text(pDX, IDC_EDIT1, m_sFolderName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewFolder, CDialog)
	//{{AFX_MSG_MAP(CDlgNewFolder)
		// HINWEIS: Der Klassen-Assistent fügt hier Zuordnungsmakros für Nachrichten ein
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgNewFolder 
