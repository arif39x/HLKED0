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
// SkySocket.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "SkySocket.h"
#include "SkyServerDlg.h"
#include "SkyThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkySocket

	

CSkySocket::CSkySocket(int i)
{
	m_iSocketType = i;
}

CSkySocket::CSkySocket()
{
	m_iSocketType = 0;
}

CSkySocket::~CSkySocket()
{
}


//Die folgenden Zeilen nicht bearbeiten. Sie werden vom Klassen-Assistenten benötigt.
#if 0
BEGIN_MESSAGE_MAP(CSkySocket, CSocket)
	//{{AFX_MSG_MAP(CSkySocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// Member-Funktion CSkySocket 

void CSkySocket::SetParent(CDialog *pDlg)
{
	m_pDlg = pDlg;
}

void CSkySocket::SetParentThread(CSkyThread *pSkyThread)
{
	m_cSkyThread = pSkyThread;
}

void CSkySocket::OnAccept(int nErrorCode)
{
	if(m_iSocketType == 5) return;
	if(nErrorCode == 0)
	{
		if(m_iSocketType != 0) m_cSkyThread->OnAccept();
		else ((CSkyServerDlg*)m_pDlg)->OnAccept();
	}
}

void CSkySocket::OnClose(int nErrorCode)
{
	if(m_iSocketType == 5) return;
	if(nErrorCode == 0)
	{
		if(m_iSocketType != 0) m_cSkyThread->OnClose();
		else ((CSkyServerDlg*)m_pDlg)->OnClose();
	}
}

void CSkySocket::OnReceive(int nErrorCode)
{
	if(m_iSocketType == 5) return;
	if(nErrorCode == 0)
	{
		if (m_iSocketType != 0) m_cSkyThread->OnReceive();
		else ((CSkyServerDlg*)m_pDlg)->OnReceive();
	}
	else if(m_iSocketType != 0) m_cSkyThread->ReceiveError();
}

void CSkySocket::OnSend(int nErrorCode)
{
	if(m_iSocketType == 5) return;
	if(nErrorCode == 0)
	{
		if(m_iSocketType != 0) m_cSkyThread->OnSend();
		else ((CSkyServerDlg*)m_pDlg)->OnSend();
	}
	else if(m_iSocketType != 0) m_cSkyThread->SendError();
}
