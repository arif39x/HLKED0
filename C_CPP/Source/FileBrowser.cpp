// FileBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "FileList.h"
#include "FileBrowser.h"
#include "net.h"
#include "TransferDlg.h"
#include "MkdirDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileBrowser dialog


CFileBrowser::CFileBrowser(CWnd* pParent /*=NULL*/)
	: CDialog(CFileBrowser::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFileBrowser)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFileBrowser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFileBrowser)
	DDX_Control(pDX, IDC_MKDIRBUT, m_MkdirButton);
	DDX_Control(pDX, IDC_DOWNBUT, m_DownButton);
	DDX_Control(pDX, IDC_DELBUT, m_DelButton);
	DDX_Control(pDX, IDC_UPBUT, m_UpButton);
	DDX_Control(pDX, IDC_FILELIST, m_FileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFileBrowser, CDialog)
	//{{AFX_MSG_MAP(CFileBrowser)
	ON_BN_CLICKED(IDC_REFRESHB, OnRefreshb)
	ON_NOTIFY(NM_DBLCLK, IDC_FILELIST, OnDblclkFilelist)
	ON_BN_CLICKED(IDC_DELBUT, OnDelbut)
	ON_BN_CLICKED(IDC_DOWNBUT, OnDownbut)
	ON_BN_CLICKED(IDC_UPBUT, OnUpbut)
	ON_BN_CLICKED(IDC_MKDIRBUT, OnMkdirbut)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_FILE_MESSAGE, OnFileMessage)
	ON_MESSAGE(WM_LOST_CONNECTION, OnLostConnection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileBrowser message handlers

BOOL CFileBrowser::OnInitDialog() 
{
	CDialog::OnInitDialog();

	LVCOLUMN	lvColumn;
	int			nCol;

	m_FileList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 140;
	lvColumn.pszText = "File";
	nCol = m_FileList.InsertColumn(0, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_RIGHT;
	lvColumn.cx = 80;
	lvColumn.pszText = "Size (bytes)";
	m_FileList.InsertColumn(1, &lvColumn);

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 50;
	lvColumn.pszText = "Type";
	m_FileList.InsertColumn(2, &lvColumn);

	cndata[CurrentSelection].hWnd = GetSafeHwnd();

	_snprintf(CurrentDir, sizeof(CurrentDir) - 1, "drives");

	OnRefreshb();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFileBrowser::OnOK() 
{
	// TODO: Add extra validation here

	cndata[CurrentSelection].hWnd = NULL;
	
	CDialog::OnOK();
}

LRESULT CFileBrowser::OnLostConnection(UINT wParam, LONG lParam)
{
	if (CurrentSelection == wParam)
		OnOK();

	return 0;
}

LRESULT CFileBrowser::OnFileMessage(UINT wParam, LONG lParam)
{
	LVITEM		lvItem;
	int			nItem;
	char		buff[MAX_RECEIVE_BUFFER], *b;

	if (lParam == 3)
	{
		// todo: finished getting file list
		return 0;
	}
	// file action successful
	else if (lParam == 4)
	{
		OnRefreshb();
		return 0;
	}
	// file to perform action
	else if (lParam == 5)
	{
		AfxMessageBox("Failed to perform file action.", MB_OK);
		return 0;
	}
	else
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = m_FileList.GetItemCount();
		lvItem.iSubItem = 0;

		_snprintf(buff, sizeof(buff) - 1, "%s", NewText[wParam]);

		// add file, dir or drive name
		b = strtok(buff, "\"");
		lvItem.pszText = b;
		nItem = m_FileList.InsertItem(&lvItem);

		// add file size
		if (lParam == 0)
		{
			b = strtok(NULL, "\"");
			if (b != NULL)
				m_FileList.SetItemText(nItem, 1, b);
		}

		// add type
		if (lParam == 0)
		{
			_snprintf(buff, sizeof(buff) - 1, "File");
			m_FileList.SetItemText(nItem, 2, buff);
		}
		else if (lParam == 1)
		{
			_snprintf(buff, sizeof(buff) - 1, "Dir");
			m_FileList.SetItemText(nItem, 2, buff);
		}
		else if (lParam == 2)
		{
			_snprintf(buff, sizeof(buff) - 1, "Drive");
			m_FileList.SetItemText(nItem, 2, buff);
		}
		else
		{
			_snprintf(buff, sizeof(buff) - 1, "Unknw");
			m_FileList.SetItemText(nItem, 2, buff);
		}

		UpdateData(TRUE);
		return 0;
	}
}

void CFileBrowser::OnRefreshb() 
{
	m_FileList.DeleteAllItems();

	if (strlen(CurrentDir) == 2)
	{
		LVITEM		lvItem;
		int			nItem;

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = 0;
		lvItem.iSubItem = 0;
		lvItem.pszText = ".";
		nItem = m_FileList.InsertItem(&lvItem);
		m_FileList.SetItemText(nItem, 2, "Dir");

		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = 1;
		lvItem.iSubItem = 0;
		lvItem.pszText = "..";
		nItem = m_FileList.InsertItem(&lvItem);
		m_FileList.SetItemText(nItem, 2, "Dir");
	}

	if (!strcmp(CurrentDir, "drives"))
	{
		m_MkdirButton.EnableWindow(false);
		m_DelButton.EnableWindow(false);
		m_DownButton.EnableWindow(false);
		m_UpButton.EnableWindow(false);
	}
	else
	{
		m_MkdirButton.EnableWindow(true);
		m_DelButton.EnableWindow(true);
		m_DownButton.EnableWindow(true);
		m_UpButton.EnableWindow(true);
	}

	UpdateData(TRUE);
	NET_Send(CurrentSelection, FILES, CurrentDir);	
}

void CFileBrowser::OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int		i;

	i = m_FileList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (i < 0)
		return;
	else
	{
		char	buff[MAX_PATH];

		m_FileList.GetItemText(i, 2, buff, sizeof(buff) - 1);

		// we only go on if dbl clicked on driver or dir
		if (!strcmp(buff, "Dir"))
		{
			m_FileList.GetItemText(i, 0, buff, sizeof(buff) - 1);
			if (!strcmp(buff, ".."))
			{
				char	*p;
				p = strrchr(CurrentDir, '\\');
				if (p != NULL)
					*p = 0;
				else
				{
					_snprintf(CurrentDir, sizeof(CurrentDir) - 1, "drives");
				}
			}
			else if (!strcmp(buff, "."))
			{
				// simply refresh
			}
			else
				_snprintf(CurrentDir, sizeof(CurrentDir) - 1, "%s\\%s", CurrentDir, buff);

			OnRefreshb();
		}
		else if (!strcmp(buff, "Drive"))
		{
			m_FileList.GetItemText(i, 0, buff, sizeof(buff) - 1);
			_snprintf(CurrentDir, sizeof(CurrentDir) - 1, "%s", buff);
			OnRefreshb();
		}
	}
	
	*pResult = 0;
}

void CFileBrowser::FileAction(f_action action)
{
	int		i;
	char	name[MAX_PATH],
			type[MAX_PATH],
			size[MAX_PATH],
			buff[MAX_PATH];
	int		transfer = 0; // 1 == incoming, 2 == outgoing
	CTransferDlg trans;

	if (action == UPLOAD)
	{
		CFileDialog FileDlg(TRUE, "*", NULL, 0, NULL);

		if (FileDlg.DoModal() == IDOK)
		{
			WIN32_FIND_DATA	FindFileData;
			HANDLE fh;
			fh = FindFirstFile(FileDlg.GetPathName(), &FindFileData);
			FindClose(fh);

			trans.bindport = TransferPort;
			trans.transfer = 2;
			trans.fsize = atoi(size);
			trans.fsize = FindFileData.nFileSizeLow;
			_snprintf(trans.remotefile, sizeof(trans.remotefile) - 1, "%s\\%s", CurrentDir, FileDlg.GetFileName());
			_snprintf(trans.localfile, sizeof(trans.localfile) - 1, "%s", FileDlg.GetPathName());

			_snprintf(buff, sizeof(buff) - 1, "%d %s\\%s", TransferPort, CurrentDir, FileDlg.GetFileName());
			NET_Send(CurrentSelection, FILEIN, buff);

			trans.DoModal();

			cndata[CurrentSelection].hWnd = GetSafeHwnd();
		}

		return;
	}
	else if (action == MAKEDIR)
	{
		CMkdirDlg dirdlg;
		if (dirdlg.DoModal() == IDOK && strlen(dirdlg.dir) > 0)
		{
			_snprintf(buff, sizeof(buff) - 1, "%s\\%s", CurrentDir, dirdlg.dir);
			NET_Send(CurrentSelection, FILEMKDIR, buff);
		}
		return;
	}
	
	i = m_FileList.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		
	if (i < 0)
	{
		AfxMessageBox("No file selected.", MB_OK|MB_ICONERROR);
		return;
	}

	m_FileList.GetItemText(i, 0, name, sizeof(name));
	
	if (!strcmp(name, "..") || !strcmp(name, "."))
		return;

	m_FileList.GetItemText(i, 1, size, sizeof(size));
	m_FileList.GetItemText(i, 2, type, sizeof(type));

	if (!strcmp(type, "Dir") && action == DEL)
	{
		_snprintf(buff, sizeof(buff) - 1, "%s\\%s", CurrentDir, name);
		NET_Send(CurrentSelection, FILEDELDIR, buff);
		return;
	}

	if (!strcmp(type, "File"))
	{
		switch (action)
		{
		case DEL:
			_snprintf(buff, sizeof(buff) - 1, "%s\\%s", CurrentDir, name);
			NET_Send(CurrentSelection, FILEDEL, buff);
			break;
		case DOWNLOAD:
			_snprintf(buff, sizeof(buff) - 1, "%d %s\\%s", TransferPort, CurrentDir, name);
			transfer = 1;
			NET_Send(CurrentSelection, FILEOUT, buff);
			break;
		default:
			break;
		}
	}
	else
		return;

	if (transfer > 0)
	{
		trans.bindport = TransferPort;
		trans.transfer = transfer;
		trans.fsize = atoi(size);
		_snprintf(trans.remotefile, sizeof(trans.remotefile) - 1, "%s\\%s", CurrentDir, name);
		trans.DoModal();

		cndata[CurrentSelection].hWnd = GetSafeHwnd();
	}
}

void CFileBrowser::OnDelbut() 
{
	FileAction(DEL);
}

void CFileBrowser::OnDownbut() 
{
	FileAction(DOWNLOAD);	
}

void CFileBrowser::OnUpbut() 
{
	FileAction(UPLOAD);	
}

void CFileBrowser::OnMkdirbut() 
{
	FileAction(MAKEDIR);	
}
