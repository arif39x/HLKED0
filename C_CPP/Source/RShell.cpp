// RShell.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "RShellSend.h"
#include "RShell.h"
#include "net.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRShell dialog


CRShell::CRShell(CWnd* pParent /*=NULL*/)
	: CDialog(CRShell::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRShell)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRShell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRShell)
	DDX_Control(pDX, IDC_INPUT, m_ShellSend);
	DDX_Control(pDX, IDC_OUTPUT, m_OutputScreen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRShell, CDialog)
	//{{AFX_MSG_MAP(CRShell)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_SHELL_MESSAGE, OnShellMessage)
	ON_MESSAGE(WM_LOST_CONNECTION, OnLostConnection)
	ON_REGISTERED_MESSAGE(UWM_EDIT_COMPLETE, OnEditComplete)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRShell message handlers

BOOL CRShell::OnInitDialog() 
{
	CDialog::OnInitDialog();

	cndata[CurrentSelection].hWnd = GetSafeHwnd();
	
	NET_Send(CurrentSelection, SHELL, "on");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int	CRShell::GetNumberOfLines(CString in)
{
	int		i = 0, k = 0;

	i = in.Find("\n", 0);
	while (i != -1)
	{
		k++;
		i = in.Find("\n", i + 1);
	}

	return k;

}

LRESULT CRShell::OnLostConnection(UINT wParam, LONG lParam)
{
	if (CurrentSelection == wParam)
		OnOK();

	return 0;
}

LRESULT CRShell::OnShellMessage(UINT wParam, LONG lParam)
{
	OutputText += NewText[wParam];

	m_OutputScreen.SetWindowText(OutputText);

	m_OutputScreen.LineScroll(GetNumberOfLines(OutputText));

	UpdateData(TRUE);
	return 0;
}

LRESULT CRShell::OnEditComplete(WPARAM, LPARAM lParam)
{
	char	buffer[MAX_RECEIVE_BUFFER - 8] = "msg ";

	m_ShellSend.GetWindowText(buffer + 4, sizeof(buffer) - 5);

	if (!strcmp(buffer, "msg cls"))
	{
		m_OutputScreen.SetWindowText("");
		OutputText = "";
		NET_Send(CurrentSelection, SHELL, "msg ");
	}
	else if (!strcmp(buffer, "msg exit"))
	{
		NET_Send(CurrentSelection, SHELL, buffer);
		OnExit();
	}
	else
	{
		if (strcmp(buffer, "msg "))
		{
			OutputText.ReleaseBuffer(OutputText.GetLength() - 2);
		}

		if (NET_Send(CurrentSelection, SHELL, buffer) <= 0)
			AfxMessageBox("Socket error - the connection might be closed.", MB_OK|MB_ICONERROR);
	}

	m_ShellSend.SetWindowText("");

    return 0;
}

void CRShell::OnExit() 
{
	NET_Send(CurrentSelection, SHELL, "off");
	
	OnOK();	
}

void CRShell::OnOK() 
{
	cndata[CurrentSelection].hWnd = NULL;

	CDialog::OnOK();	
}
