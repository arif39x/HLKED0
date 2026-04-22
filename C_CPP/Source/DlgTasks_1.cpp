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
// DlgTasks.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "DlgTasks.h"
#include "SkydanceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgTasks 


CDlgTasks::CDlgTasks(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTasks::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTasks)
	m_sListTasks = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTasks::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTasks)
	DDX_Control(pDX, IDC_LISTTASKS, m_ctrlListTasks);
	DDX_LBString(pDX, IDC_LISTTASKS, m_sListTasks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTasks, CDialog)
	//{{AFX_MSG_MAP(CDlgTasks)
	ON_LBN_SELCHANGE(IDC_LISTTASKS, OnSelchangeListtasks)
	ON_BN_CLICKED(IDC_BTUPDATE, OnBtupdate)
	ON_BN_CLICKED(IDC_BTTCLOSE, OnBttclose)
	ON_BN_CLICKED(IDC_BTCLOSEALL, OnBtcloseall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgTasks 



void CDlgTasks::OnSelchangeListtasks() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BTTCLOSE)->EnableWindow(TRUE);
}

void CDlgTasks::OnBtupdate() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	GetDlgItem(IDC_BTTCLOSE)->EnableWindow(FALSE);
	pSkydanceDoc->SendPack("ListTasks</");
}

void CDlgTasks::OnBttclose() 
{
	if(m_sListTasks != "")
	{
		int iCurSel;
		int iCurSelData;
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		iCurSel = m_ctrlListTasks.GetCurSel();
		if(iCurSel != LB_ERR) 
		{
			//m_ctrlListTasks.SetItemData(iCurSel, 2);
			iCurSelData = (int)m_ctrlListTasks.GetItemData(iCurSel);
			//if( iCurSelData == 2 ) {AfxMessageBox("Blalab!"); return;}
			char cCurSelData[3];
			itoa(iCurSelData, cCurSelData, 10);
			CString sCurSelData(cCurSelData);
			pSkydanceDoc->SendPack("CloseTask<" + sCurSelData + "</");
		}
	}
}

void CDlgTasks::OnBtcloseall() 
{
	if(MessageBox("Are you sure you want to close all running Applications", "WARNING", MB_OKCANCEL) == IDOK)
	{
		CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
		pSkydanceDoc->SendPack("CloseAllTasks</");
	}	
}
