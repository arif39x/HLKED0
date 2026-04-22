// MySocket.cpp : implementation file
//

#include "stdafx.h"
#include "inetspy.h"
#include "MySocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMySocket

CMySocket::CMySocket()
{
	m_pAcceptedSocket=NULL;
}

CMySocket::~CMySocket()
{
	if(m_pAcceptedSocket!=NULL)
		delete m_pAcceptedSocket;
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CMySocket, CAsyncSocket)
	//{{AFX_MSG_MAP(CMySocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CMySocket member functions

void CMySocket::OnAccept(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	//AfxMessageBox("Accept this connect?",MB_OKCANCEL);
	
	CAsyncSocket::OnAccept(nErrorCode);

	if(m_pAcceptedSocket==NULL)
	{
		m_pAcceptedSocket=new CMySocket;
		Accept(*m_pAcceptedSocket);
		m_pAcceptedSocket->m_hParentWnd=m_hParentWnd;
		AfxMessageBox("Connected!");
	}

	::SendMessage(m_hParentWnd,WM_SOCKET_ACTIVE,1,0);
	
}

void CMySocket::OnConnect(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	if(!nErrorCode)
		AfxMessageBox("Connected!");
	else
		AfxMessageBox("Connect Error!");

	::SendMessage(m_hParentWnd,WM_SOCKET_ACTIVE,1,0);

	CAsyncSocket::OnConnect(nErrorCode);
}

void CMySocket::OnReceive(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class

	static char buffer[32000];
	int count;

	count=Receive(buffer,32000);

	//if(count==32000) count--;
	//buffer[count]=0;

	::SendMessage(m_hParentWnd,WM_RECEIVE,count,(LONG)buffer);

	CAsyncSocket::OnReceive(nErrorCode);
}

void CMySocket::OnClose(int nErrorCode) 
{
	// TODO: Add your specialized code here and/or call the base class
	AfxMessageBox("Socket closed!");
	::SendMessage(m_hParentWnd,WM_SOCKET_ACTIVE,0,0);
	CAsyncSocket::OnClose(nErrorCode);
}
