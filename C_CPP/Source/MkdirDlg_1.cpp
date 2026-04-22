// MkdirDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "MkdirDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMkdirDlg dialog


CMkdirDlg::CMkdirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMkdirDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMkdirDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMkdirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMkdirDlg)
	DDX_Control(pDX, IDC_DIRNAME, m_DirNameBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMkdirDlg, CDialog)
	//{{AFX_MSG_MAP(CMkdirDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMkdirDlg message handlers

void CMkdirDlg::OnOK() 
{
	m_DirNameBox.GetWindowText(dir, sizeof(dir) - 1);

	CDialog::OnOK();
}

void CMkdirDlg::OnCancel() 
{
	
	CDialog::OnCancel();
}
