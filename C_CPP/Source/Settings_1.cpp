// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Settings dialog

Settings::Settings(CWnd* pParent /*=NULL*/)
	: CDialog(Settings::IDD, pParent)
{
	//{{AFX_DATA_INIT(Settings)
	m_ListenPort = DEFAULT_LISTEN_PORT;
	m_MaxVictims = DEFAULT_MAX_SIM_VICTIMS;
	m_PingInterval = DEFAULT_PING_INTERVAL;
	m_TransferPort = DEFAULT_TRANSFER_PORT ;
	//}}AFX_DATA_INIT
}


void Settings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Settings)
	DDX_Control(pDX, IDC_LOGINCMD, m_LoginCMD);
	DDX_Text(pDX, IDC_PORT, m_ListenPort);
	DDV_MinMaxInt(pDX, m_ListenPort, 1, 65536);
	DDX_Text(pDX, IDC_MAXVICTS, m_MaxVictims);
	DDV_MinMaxInt(pDX, m_MaxVictims, 1, MAX_SIM_VICTIMS);
	DDX_Text(pDX, IDC_PINGINT, m_PingInterval);
	DDV_MinMaxInt(pDX, m_PingInterval, 1, 1200);
	DDX_Text(pDX, IDC_TPORT, m_TransferPort);
	DDV_MinMaxInt(pDX, m_TransferPort, 1, 65536);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Settings, CDialog)
	//{{AFX_MSG_MAP(Settings)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Settings message handlers
BOOL Settings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_LoginCMD.SetWindowText(VictimLoginPacket);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void Settings::OnOK() 
{
	UpdateData(TRUE);
	ListenPort = m_ListenPort;
	MaxSimVictims = m_MaxVictims;
	PingInterval = m_PingInterval;
	TransferPort = m_TransferPort;
	m_LoginCMD.GetWindowText(VictimLoginPacket, sizeof(VictimLoginPacket) - 1);
	
	CDialog::OnOK();
}

void Settings::OnClose() 
{
	CDialog::OnCancel();	
}
