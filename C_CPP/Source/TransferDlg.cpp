// TransferDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "TransferDlg.h"
#include "net.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransferDlg dialog


CTransferDlg::CTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransferDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransferDlg)
	DDX_Control(pDX, IDC_RSTATUS, m_RemoteStatus);
	DDX_Control(pDX, IDCANCEL, m_Button);
	DDX_Control(pDX, IDC_TEXT, m_InfoText);
	DDX_Control(pDX, IDC_PROGBAR, m_ProgressBar);
	DDX_Control(pDX, IDC_SIZES, m_ProgressText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CTransferDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_INOUT_FILE_MESSAGE_NEWSTEP, OnNewStep)
	ON_MESSAGE(WM_NEW_INOUT_FILE_MESSAGE_FINISHED, OnFinished)
	ON_MESSAGE(WM_NEW_INOUT_FILE_MESSAGE_ERROR, OnError)
END_MESSAGE_MAP()

DWORD WINAPI ListenThread(LPVOID param);
//UINT ListenThread(LPVOID param);
typedef struct
{
	HWND			t_h;
	int				t_bindport;
	unsigned long	t_fsize;
	char			t_file[MAX_PATH];
	int				t_transfer;
} TDATA;

unsigned long	transfered;
int				barsize;
FILE			*f;
HANDLE			tHandle;
SOCKET			sock, clsock;

/////////////////////////////////////////////////////////////////////////////
// CTransferDlg message handlers

void CTransferDlg::OnCancel() 
{
	// TODO: Add extra cleanup here

	// file not finished yet
	if (transfered < fsize)
	{
		NET_Send(CurrentSelection, FILES, "stop");
	}

	if (tHandle != NULL)
		TerminateThread(tHandle, 0);

	if (f != NULL)
		fclose(f);

	closesocket(sock);
	closesocket(clsock);

	cndata[CurrentSelection].hWnd = NULL;

	CDialog::OnCancel();
}

BOOL CTransferDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	int		nLower, nUpper;
	m_ProgressBar.GetRange(nLower, nUpper);
	barsize = nUpper - nLower;

	m_ProgressText.SetWindowText("Starting to transfer file.");
	m_InfoText.SetWindowText(remotefile);

	cndata[CurrentSelection].hWnd = GetSafeHwnd();
	
	TDATA td;
	
	td.t_h = GetSafeHwnd();
	td.t_bindport = bindport;
	td.t_fsize = fsize;
	if (transfer == 1)
		_snprintf(td.t_file, sizeof(td.t_file) - 1, "%s", remotefile);
	else
		_snprintf(td.t_file, sizeof(td.t_file) - 1, "%s", localfile);
	td.t_transfer = transfer;

	tHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenThread, (LPVOID)&td, 0, 0);

	Sleep(50);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTransferDlg::SetProgressBar()
{
	if (fsize == 0)
		m_ProgressBar.SetPos(barsize);
	else if (fsize < 100000)
		m_ProgressBar.SetPos((barsize * transfered) / fsize);
	else
		m_ProgressBar.SetPos((barsize * (transfered / 1024)) / (fsize / 1024));
}

LRESULT CTransferDlg::OnNewStep(UINT wParam, LONG lParam)
{
	char	buffer[MAX_BUFF_SIZE];

	if (transfered < 10000)
		_snprintf(buffer, sizeof(buffer) - 1, "%d Bytes /", transfered);
	else if (transfered < 10000000)
		_snprintf(buffer, sizeof(buffer) - 1, "%d KBytes /", transfered / 1024);
	else 
		_snprintf(buffer, sizeof(buffer) - 1, "%d Mbytes /", transfered / 1048576);

	if (fsize < 10000)
		_snprintf(buffer, sizeof(buffer) - 1, "%s %d Bytes", buffer, fsize);
	else if (fsize < 10000000)
		_snprintf(buffer, sizeof(buffer) - 1, "%s %d KBytes", buffer, fsize / 1024);
	else 
		_snprintf(buffer, sizeof(buffer) - 1, "%s %d MBytes", buffer, fsize / 1048576);

	m_ProgressText.SetWindowText(buffer);

	SetProgressBar();

	UpdateData(TRUE);

	return 0;
}

LRESULT CTransferDlg::OnFinished(UINT wParam, LONG lParam)
{
	char	buffer[MAX_BUFF_SIZE];

	if (transfered < 10000)
		_snprintf(buffer, sizeof(buffer) - 1, "Finished transfering %d Bytes", transfered);
	else if (transfered < 10000000)
		_snprintf(buffer, sizeof(buffer) - 1, "Finished transfering %d KBytes", transfered / 1024);
	else 
		_snprintf(buffer, sizeof(buffer) - 1, "Finished transfering %d Mbytes", transfered / 1048576);

	m_ProgressText.SetWindowText(buffer);

	m_Button.SetWindowText("OK");

	SetProgressBar();

	UpdateData(TRUE);

	return 0;
}

LRESULT CTransferDlg::OnError(UINT wParam, LONG lParam)
{
	char	buffer[MAX_BUFF_SIZE];

	if (wParam)
	{
		_snprintf(buffer, sizeof(buffer) - 1, "Remote status: success");
		m_RemoteStatus.SetWindowText(buffer);

		return 0;
	}

	_snprintf(buffer, sizeof(buffer) - 1, "Error transfering: %d / %d", transfered, fsize);
	m_ProgressText.SetWindowText(buffer);

	m_Button.SetWindowText("Close");

	return 0;
}

int CheckTimeout(SOCKET sock)
{
	struct timeval		timeout;
	fd_set				fd;

	timeout.tv_sec = DEFAULT_TRANSMIT_TIMEOUT;
	timeout.tv_usec = 0;

	FD_ZERO(&fd); 
	FD_SET(sock, &fd);

	return select(sock, &fd, NULL, NULL, &timeout);
}

void Accept(HWND h, SOCKET clsock, unsigned long fsize, char *fname, int transfer)
{
	char			buff[MAX_RECEIVE_BUFFER] = {0};
	int				r;

	transfered = 0;

	if (transfer == 1)
		f = fopen(fname, "wb");
	else
		f = fopen(fname, "rb");

	if (f == NULL)
	{
		AfxMessageBox("Failed to open local file.", MB_OK|MB_ICONERROR);
		closesocket(clsock);
	}

	if (transfer == 1)
	{
		while (CheckTimeout(clsock) > 0)
		{
			memset(buff, 0, sizeof(buff));
			r = recv(clsock, buff, MAX_RECEIVE_BUFFER, 0);
			if (r > 0)
			{
				fwrite(buff, r, 1, f);
				transfered += r;
				::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_NEWSTEP, (WPARAM)0, (LPARAM)0);
			}
			else if (r == 0)
			{
				::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_FINISHED, (WPARAM)0, (LPARAM)0);
				fclose(f);
				closesocket(clsock);
				return;
			}
			else
			{
				::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_ERROR, (WPARAM)0, (LPARAM)0);
				fclose(f);
				closesocket(clsock);
				return;
			}
		}

		::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_ERROR, (WPARAM)0, (LPARAM)1);
		fclose(f);
		closesocket(clsock);

		return;
	}
	else
	{
		while ((r = fread(buff, 1, MAX_RECEIVE_BUFFER, f)) >= 1)
		{
			if (clsock <= 0)
			{
				::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_ERROR, (WPARAM)0, (LPARAM)0);
				fclose(f);
				closesocket(clsock);
				return;
			}
			send(clsock, buff, r, 0);
			transfered += r;
			::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_NEWSTEP, (WPARAM)0, (LPARAM)0);
			memset(buff, 0, sizeof(buff));
		}

		::PostMessage(h, WM_NEW_INOUT_FILE_MESSAGE_FINISHED, (WPARAM)0, (LPARAM)0);
		fclose(f);
		closesocket(clsock);

		return;
	}
}

DWORD WINAPI ListenThread(LPVOID param)
//UINT ListenThread(LPVOID param)
{
	TDATA			td = *((TDATA *)param);
	
	int				on;
	sockaddr_in		sin;
	char			*p;

	if (td.t_transfer == 1)
	{
		p = strrchr(td.t_file, '\\');
		p++;
	}
	else
		p = td.t_file;
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	if (sock == INVALID_SOCKET)
	{
		AfxMessageBox("Failed to initialize socket.", MB_OK|MB_ICONERROR);
		return 0;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(td.t_bindport);

	if (bind(sock, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR) 
	{
		AfxMessageBox("Failed to bind port.", MB_OK|MB_ICONERROR);
		return 0;
	}

	if (listen(sock, 1) == SOCKET_ERROR) 
	{
		AfxMessageBox("Failed to listen on port.", MB_OK|MB_ICONERROR);
		return 0;
	}

	while (1) 
	{
		clsock = SOCKET_ERROR;

		while (clsock == SOCKET_ERROR) 
		{
			clsock = accept(sock, NULL, NULL);
		}

		if (clsock != INVALID_SOCKET) 
		{
			Accept(td.t_h, clsock, td.t_fsize, p, td.t_transfer);
			break;
		}
	}

	closesocket(sock);

	return 0;
}
