// CaptureScreenDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "CaptureScreenDlg.h"
#include "net.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCaptureScreenDlg dialog


CCaptureScreenDlg::CCaptureScreenDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCaptureScreenDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCaptureScreenDlg)
	//}}AFX_DATA_INIT
}


void CCaptureScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCaptureScreenDlg)
	DDX_Control(pDX, IDC_FILEINPUT, m_InputFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCaptureScreenDlg, CDialog)
	//{{AFX_MSG_MAP(CCaptureScreenDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCaptureScreenDlg message handlers

void CCaptureScreenDlg::OnOK() 
{
	UpdateData(TRUE);
	m_InputFile.GetWindowText(file, sizeof(file));

	if (strlen(file) < 1)
	{
		AfxMessageBox("File name can't be empty.", MB_ICONERROR | MB_OK);
		return;
	}

	strncat(file, ".bmp", sizeof(file) - 1);

	CDialog::OnOK();
}
