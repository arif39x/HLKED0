// inetspyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "inetspy.h"
#include "inetspyDlg.h"
#include "messagedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInetspyDlg dialog

CInetspyDlg::CInetspyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInetspyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInetspyDlg)
	m_sAddress = _T("");
	m_iPort = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInetspyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInetspyDlg)
	DDX_Text(pDX, IDC_ADDRESS, m_sAddress);
	DDV_MaxChars(pDX, m_sAddress, 128);
	DDX_Text(pDX, IDC_PORT, m_iPort);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInetspyDlg, CDialog)
	//{{AFX_MSG_MAP(CInetspyDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_EN_CHANGE(IDC_ADDRESS, OnChangeAddress)
	ON_BN_CLICKED(IDC_LISTEN, OnListen)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInetspyDlg message handlers

BOOL CInetspyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInetspyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInetspyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInetspyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CInetspyDlg::OnChangeAddress() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();

	if(m_sAddress.IsEmpty())
	{
		GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_LISTEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	}

	
}

void CInetspyDlg::OnListen() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CMessageDlg dlg;
	dlg.m_bIsListen=TRUE;
	strcpy(dlg.m_sAddress,(LPCSTR)m_sAddress);
	dlg.m_iPort=m_iPort;

	dlg.DoModal();

}

void CInetspyDlg::OnConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	CMessageDlg dlg;
	dlg.m_bIsListen=FALSE;
	strcpy(dlg.m_sAddress,(LPCSTR)m_sAddress);
	dlg.m_iPort=m_iPort;

	dlg.DoModal();
	
}
