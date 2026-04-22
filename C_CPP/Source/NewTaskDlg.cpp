// NewTaskDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "NewTaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg dialog


CNewTaskDlg::CNewTaskDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewTaskDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewTaskDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CNewTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewTaskDlg)
	DDX_Control(pDX, IDC_NTINPUT, m_NTInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewTaskDlg, CDialog)
	//{{AFX_MSG_MAP(CNewTaskDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg message handlers

void CNewTaskDlg::OnOK() 
{
	m_NTInput.GetWindowText(command, sizeof(command));
	
	CDialog::OnOK();
}
