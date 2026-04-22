// ProxySettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "ProxySettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProxySettingsDlg dialog


CProxySettingsDlg::CProxySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProxySettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProxySettingsDlg)
	m_Port = DEFAULT_PROXY_PORT;
	m_KeepAlive = TRUE;
	m_Timeout = DEFAULT_PROXY_TIMEOUT;
	m_LoggingEnabled = FALSE;
	//}}AFX_DATA_INIT
}


void CProxySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProxySettingsDlg)
	DDX_Control(pDX, IDC_EDITLOGFILE, m_EditLogFile);
	DDX_Text(pDX, IDC_EDITPORT, m_Port);
	DDV_MinMaxInt(pDX, m_Port, 1, 65536);
	DDX_Check(pDX, IDC_CHECKKA, m_KeepAlive);
	DDX_Text(pDX, IDC_EDITTIMEOUT, m_Timeout);
	DDV_MinMaxInt(pDX, m_Timeout, 0, 100000000);
	DDX_Check(pDX, IDC_PLE, m_LoggingEnabled);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProxySettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CProxySettingsDlg)
	ON_BN_CLICKED(IDC_PLE, OnPle)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProxySettingsDlg message handlers

void CProxySettingsDlg::OnOK() 
{
	UpdateData(TRUE);

	if (m_Port > 65536 || m_Port < 1)
		port = DEFAULT_PROXY_PORT;
	else
		port = m_Port;

	if (m_Timeout < 0)
		timeout = DEFAULT_PROXY_TIMEOUT;
	else
		timeout = m_Timeout;

	keepalive = m_KeepAlive;

	if (m_LoggingEnabled)
		m_EditLogFile.GetWindowText(path, sizeof(path));
	else
		path[0] = 0;
	
	CDialog::OnOK();
}

BOOL CProxySettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProxySettingsDlg::OnPle() 
{
	UpdateData(TRUE);

	if (m_LoggingEnabled)
		m_EditLogFile.EnableWindow(TRUE);
	else
		m_EditLogFile.EnableWindow(FALSE);

	UpdateData(TRUE);
}
