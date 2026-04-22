// TaskMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "TaskMgrDlg.h"
#include "net.h"
#include "NewTaskDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskMgrDlg dialog


CTaskMgrDlg::CTaskMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTaskMgrDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskMgrDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTaskMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskMgrDlg)
	DDX_Control(pDX, IDC_PROCLIST, m_ProcessList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskMgrDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskMgrDlg)
	ON_BN_CLICKED(IDC_REFRESHBUT, OnRefreshbut)
	ON_BN_CLICKED(IDC_KILLBUT, OnKillbut)
	ON_BN_CLICKED(IDC_NEWTASKBUT, OnNewtaskbut)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_PROCESS_MESSAGE, OnProcessMessage)
	ON_MESSAGE(WM_LOST_CONNECTION, OnLostConnection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskMgrDlg message handlers

BOOL CTaskMgrDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LVCOLUMN	lvColumn;
	int			nCol;

	m_ProcessList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 130;
	lvColumn.pszText = "Image Name";
	nCol = m_ProcessList.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 50;
	lvColumn.pszText = "ID";
	m_ProcessList.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 60;
	lvColumn.pszText = "Threads";
	m_ProcessList.InsertColumn(2, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 100;
	lvColumn.pszText = "Priority";
	m_ProcessList.InsertColumn(3, &lvColumn);

	cndata[CurrentSelection].hWnd = GetSafeHwnd();

	OnRefreshbut();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTaskMgrDlg::OnRefreshbut() 
{
	m_ProcessList.DeleteAllItems();
	NET_Send(CurrentSelection, PROCESS_REFRESH, NULL);	
}

LRESULT CTaskMgrDlg::OnLostConnection(UINT wParam, LONG lParam)
{
	if (CurrentSelection == wParam)
		OnOK();

	return 0;
}

LRESULT CTaskMgrDlg::OnProcessMessage(UINT wParam, LONG lParam)
{
	LVITEM		lvItem;
	int			nItem;
	char		buff[MAX_RECEIVE_BUFFER], *b;

	if (lParam == 1)
	{
		OnRefreshbut();
		return 0;
	}
	else if (lParam == 2)
	{
		AfxMessageBox("Failed to perform process action.", MB_OK|MB_ICONERROR);
		return 0;
	}
	else
	{
		_snprintf(buff, sizeof(buff) - 1, "%s", NewText[wParam]);

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = m_ProcessList.GetItemCount();
		lvItem.iSubItem = 0;

		// add image name
		b = strtok(buff, "\"");
		lvItem.pszText = b;
		nItem = m_ProcessList.InsertItem(&lvItem);	

		// add pid
		b = strtok(NULL, "\"");
		m_ProcessList.SetItemText(nItem, 1, b);

		// add threads
		b = strtok(NULL, "\"");
		m_ProcessList.SetItemText(nItem, 2, b);

		// add priority
		// 4 = low
		// 6 = below normal
		// 8 = normal
		// 10 = above normal
		// 13 = high
		// 24 = real time
		b = strtok(NULL, "\"");
		int		p = atoi(b);

		switch (p)
		{
		case 4:
			m_ProcessList.SetItemText(nItem, 3, "Low");
			break;
		case 6:
			m_ProcessList.SetItemText(nItem, 3, "Below Normal");
			break;
		case 10:
			m_ProcessList.SetItemText(nItem, 3, "Above Normal");
			break;
		case 13:
			m_ProcessList.SetItemText(nItem, 3, "High");
			break;
		case 24:
			m_ProcessList.SetItemText(nItem, 3, "Realtime");
			break;
		default:
			m_ProcessList.SetItemText(nItem, 3, "Normal");
			break;
		}

		UpdateData(TRUE);
	}

	return 0;
}


void CTaskMgrDlg::OnKillbut() 
{
	char	pid[16];
	int		i;
	
	i = m_ProcessList.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		
	if (i < 0)
	{
		AfxMessageBox("No process selected.", MB_OK|MB_ICONERROR);
		return;
	}

	m_ProcessList.GetItemText(i, 1, pid, sizeof(pid));

	NET_Send(CurrentSelection, PROCESS_KILL, pid);
}

void CTaskMgrDlg::OnNewtaskbut() 
{
	CNewTaskDlg ntdlg;

	if (ntdlg.DoModal() == IDOK)
		NET_Send(CurrentSelection, PROCESS_RUN, ntdlg.command);
}

void CTaskMgrDlg::OnOK() 
{
	cndata[CurrentSelection].hWnd = NULL;
	CDialog::OnOK();
}
