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
// DlgFtpStatus.cpp: Implementierungsdatei
//

#include "stdafx.h"
//#include <afxpriv.h>	//von mir
#include "Skydance.h"
#include "DlgFtpStatus.h"
#include "SkydanceDoc.h"//von mir

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgFtpStatus 

//wert noch ändern...(SocketNr)

CDlgFtpStatus::CDlgFtpStatus(int nextFtpStatNr, bool scWay, int ftpPort, CString sActiveIP, bool UpDown,
							 CString sFileName, CString sFileNameServer, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgFtpStatus::IDD, pParent)
{
	///m_sDirection löschen! nicht nötig... --->>>m_bUpDown
	//m_iInitFile = 0;
	m_iNextFtpStatNr = nextFtpStatNr;
	m_bTransferDone = FALSE;
	m_sCancel = "&Abort";
	m_sFileNameServer = sFileNameServer;
	///////
	//////Thread erstellen und Variablen anpassen
	m_cSkyThread =						//Thread laden
         (CSkyThread*)AfxBeginThread(
            RUNTIME_CLASS(CSkyThread),
            THREAD_PRIORITY_NORMAL,
            0,
            CREATE_SUSPENDED);
	
	m_cSkyThread->m_bUpDown = UpDown;
	m_cSkyThread->m_pDialog = (CDlgFtpStatus*)this;
	m_cSkyThread->m_bSCway = scWay;
	m_cSkyThread->m_sActiveIP = sActiveIP;
	m_cSkyThread->m_iFtpPort = ftpPort;
	m_cSkyThread->m_sFileName = sFileName;
	m_cSkyThread->m_sFileNameServer = sFileNameServer;
	m_cSkyThread->m_iNextFtpStatNr = nextFtpStatNr;
	
	//und ab gehts!
	m_cSkyThread->ResumeThread();

	
	//{{AFX_DATA_INIT(CDlgFtpStatus)
	m_sFileName = _T("None");
	m_sGoal = _T("0");
	m_sDirection = _T("None");
	m_sCurrentStat = _T("0");
	//}}AFX_DATA_INIT
}


void CDlgFtpStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgFtpStatus)
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Text(pDX, IDC_STATICFILEN, m_sFileName);
	DDX_Text(pDX, IDC_STATICGOAL, m_sGoal);
	DDX_Text(pDX, IDC_STATICKIND, m_sDirection);
	DDX_Text(pDX, IDC_STATICSTAT, m_sCurrentStat);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgFtpStatus, CDialog)
	//ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//ON_UPDATE_COMMAND_UI(IDC_STATICSTAT, OnUpdateStat)
	//{{AFX_MSG_MAP(CDlgFtpStatus)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CDlgFtpStatus 

void CDlgFtpStatus::UpdateStat()
{
 	this->SetDlgItemText(IDCANCEL, m_sCancel);
	UpdateData(FALSE);
}	

void CDlgFtpStatus::OnCancel() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	if (m_bTransferDone == FALSE)
	{
		CSingleLock sLock(&(m_cSkyThread->m_mutex));	
			m_cSkyThread->m_bTransferBreak = TRUE;
		sLock.Lock();
	}
	pSkydanceDoc->m_bFreeSocket[m_iNextFtpStatNr] = TRUE;
	DestroyWindow();
	//stufe 2
	if(m_bServerUpdate)
	{
		if(m_sCurrentStat == m_sGoal)
		{
					if(MessageBox("Registry Server-run-command will get updated now\nContinue? (You can abort always...)",
										"Update Server: Step 2", MB_YESNO) != IDYES) return;
					pSkydanceDoc->SendPack("SetRegKey<2<\\Software\\Microsoft\\Windows\\CurrentVersion\\Run<Skd<" + m_sFileNameServer + "<0</");
					pSkydanceDoc->SendPack("DeleteRegKey<2<\\Software\\Microsoft\\Windows\\CurrentVersion<SkdDlg</");
					if(MessageBox("That´s all for now. Please wait until Server-Computer reboots and then connect again and delete the temporary file " + m_sFileNameServer,
										"Update Server: Step 3", MB_OK) != IDYES);
		}
		else AfxMessageBox("SkyServer-Update aborted!");
	}
	//ende stufe 2 und 3 stufe
	
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	//CDialog::OnCancel();
}

void CDlgFtpStatus::OnClose() 
{
	CSkydanceDoc *pSkydanceDoc = CSkydanceDoc::GetDoc();
	if (m_bTransferDone == FALSE)
	{
		CSingleLock sLock(&(m_cSkyThread->m_mutex));	
			m_cSkyThread->m_bTransferBreak = TRUE;
		sLock.Lock();
	}
	pSkydanceDoc->m_bFreeSocket[m_iNextFtpStatNr] = TRUE;
	DestroyWindow();
	//stufe 2
	if(m_bServerUpdate)
	{
		AfxMessageBox("SkyServer-Update aborted!");
	}
	//ende stufe 2
	// TODO: Zusätzlichen Bereinigungscode hier einfügen
	//CDialog::OnClose();
}
