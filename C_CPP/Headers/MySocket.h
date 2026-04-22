#if !defined(AFX_MYSOCKET_H__23955FD0_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
#define AFX_MYSOCKET_H__23955FD0_38D0_11D1_BE29_00A024A0D351__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MySocket.h : header file
//

#define WM_RECEIVE WM_USER+1000
#define WM_SOCKET_ACTIVE WM_USER+1001

/////////////////////////////////////////////////////////////////////////////
// CMySocket command target

class CMySocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CMySocket();
	virtual ~CMySocket();

// Overrides
public:
	CMySocket * m_pAcceptedSocket;
	HWND m_hParentWnd;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMySocket)
	public:
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CMySocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSOCKET_H__23955FD0_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
