// KeyloggerPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "KeyloggerPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerPathDlg dialog


CKeyloggerPathDlg::CKeyloggerPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyloggerPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyloggerPathDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CKeyloggerPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyloggerPathDlg)
	DDX_Control(pDX, IDC_INPUTPATH, m_InputPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyloggerPathDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyloggerPathDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerPathDlg message handlers

void CKeyloggerPathDlg::OnOK() 
{
	m_InputPath.GetWindowText(path, sizeof(path));

	if (strlen(path) == 0)
		AfxMessageBox("No file specified.", MB_ICONERROR | MB_OK);
	else
		CDialog::OnOK();
}
