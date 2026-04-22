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
#include "Skydance.h"
#include "SkySocket.h"
#include "SkydanceDoc.h"//von mir

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
	//////////////nur  zur info...
	//BOOL SetSockOpt( int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET );
	//BOOL IOCtl( long lCommand, DWORD* lpArgument );
	//if ( IOCtl( FIONBIO, &disableB) == 0) AfxMessageBox("Blocking disable Error");
	//if((this->AsyncSelect()) == 0) AfxMessageBox("Error block....");
	//DWORD disableB = 100;
	///////////////und weiter
	if( i != 0)
	{
	}


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
void CSkySocket::SetParent(CSkyThread *pSkyThread)
{
	m_cSkyThread = pSkyThread;
}


void CSkySocket::OnAccept(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		
		if(m_iSocketType != 0) m_cSkyThread->OnAccept();
		//else CSkydanceDoc::GetDoc()->OnAccept;
	}
}

void CSkySocket::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		if(m_iSocketType != 0) m_cSkyThread->OnClose();
		else CSkydanceDoc::GetDoc()->OnClose();
	}
}

void CSkySocket::OnReceive(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		if (m_iSocketType != 0) m_cSkyThread->OnReceive();
		else CSkydanceDoc::GetDoc()->OnReceive();
	}
	else
	{
		AfxMessageBox("Receive Error");
		if(m_iSocketType != 0) m_cSkyThread->ReceiveError();
	}
}

void CSkySocket::OnSend(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		if(m_iSocketType != 0) m_cSkyThread->OnSend();
		else CSkydanceDoc::GetDoc()->OnSend();
	}
	else 
	{
		AfxMessageBox("Send Error");
		if(m_iSocketType != 0) m_cSkyThread->SendError();
	}
}
