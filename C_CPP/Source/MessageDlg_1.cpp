// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "inetspy.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog


CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	//}}AFX_DATA_INIT
}


void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	ON_MESSAGE(WM_RECEIVE,OnReceiveData)
	ON_MESSAGE(WM_SOCKET_ACTIVE,OnSocketActive)
	ON_BN_CLICKED(IDC_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

BOOL CMessageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int res;

	if(m_bIsListen)
		res=socket.Create(m_iPort);
	else
		res=socket.Create();

	if(res==0) 
	{
		AfxMessageBox("Create Socket Error!");
		return FALSE;
	}

	socket.m_hParentWnd=m_hWnd;

	if(m_bIsListen) res=socket.Listen();
	else res=socket.Connect(m_sAddress,m_iPort);

	if(m_bIsListen&&res==0) 
	{
		AfxMessageBox("Error!");
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LONG CMessageDlg::OnReceiveData(UINT wParam, LONG lParam)
{
	LPSTR p=(LPSTR)lParam;
	int count=wParam;

	if(count==32000)
		count--;

	p[count]=0;

	m_sInData+=(LPSTR)lParam;
	GetDlgItem(IDC_MESSAGE_IN)->SetWindowText((LPCSTR)m_sInData);
	return 0;
}


void CMessageDlg::OnSend() 
{
	// TODO: Add your control notification handler code here
	char buffer[32000];
	char empty[1];
	int count;

	empty[0]=0;

	count=(GetDlgItem(IDC_INPUT)->GetWindowText(buffer,32000));
	GetDlgItem(IDC_INPUT)->SetWindowText(empty);

	if(count==0) return;

	m_sOutData+=buffer;
	GetDlgItem(IDC_MESSAGE_OUT)->SetWindowText((LPCSTR)m_sOutData);

	if(m_bIsListen)
	{
		socket.m_pAcceptedSocket->Send(buffer,count,0);
	}
	else
	{
		socket.Send(buffer,count,0);
	}
}

LONG CMessageDlg::OnSocketActive(UINT wParam, LONG lParam)
{
	if(wParam==0)
		GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	else
		GetDlgItem(IDC_SEND)->EnableWindow(TRUE);

	return 0;
}
