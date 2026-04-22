// MessageBox.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "MessageBox.h"
#include "net.h"
#include "clientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char	MsgBoxText[MAX_BUFF_SIZE];
/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog


CMessageBox::CMessageBox(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMessageBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageBox)
	DDX_Control(pDX, IDC_MSGBOXTEXT, m_MsgBoxText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageBox, CDialog)
	//{{AFX_MSG_MAP(CMessageBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageBox message handlers

void CMessageBox::OnOK() 
{
	m_MsgBoxText.GetWindowText(MsgBoxText, sizeof(MsgBoxText));
	
	CDialog::OnOK();
}

void CMessageBox::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
