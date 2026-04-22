#if !defined(AFX_MESSAGEDLG_H__23955FD1_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
#define AFX_MESSAGEDLG_H__23955FD1_38D0_11D1_BE29_00A024A0D351__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// MessageDlg.h : header file
//

#include "MySocket.h"


/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

class CMessageDlg : public CDialog
{
// Construction
public:
	LONG OnSocketActive(UINT,LONG);
	CString m_sOutData;
	CString m_sInData;
	LONG OnReceiveData(UINT,LONG);
	int m_iPort;
	char m_sAddress[128];
	BOOL m_bIsListen;
	CMySocket socket;
	CMessageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessageDlg)
	enum { IDD = IDD_MESSAGE };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEDLG_H__23955FD1_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
