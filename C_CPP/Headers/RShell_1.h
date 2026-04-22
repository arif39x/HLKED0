#if !defined(AFX_RSHELL_H__9F89754B_0627_4BE8_BCFC_2A7CC31C7E04__INCLUDED_)
#define AFX_RSHELL_H__9F89754B_0627_4BE8_BCFC_2A7CC31C7E04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RShell.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRShell dialog

class CRShell : public CDialog
{
// Construction
public:
	CRShell(CWnd* pParent = NULL);   // standard constructor

//	void IncomingText();

// Dialog Data
	//{{AFX_DATA(CRShell)
	enum { IDD = IDD_SHELLDLG };
	CRShellSend	m_ShellSend;
	CEdit	m_OutputScreen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRShell)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString		OutputText;

	// Generated message map functions
	//{{AFX_MSG(CRShell)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnExit();
	//}}AFX_MSG
	afx_msg LRESULT OnShellMessage(UINT wParam, LONG lParam);
	afx_msg LRESULT OnLostConnection(UINT wParam, LONG lParam);
	afx_msg LRESULT OnEditComplete(WPARAM, LPARAM);
	afx_msg int	GetNumberOfLines(CString in);
	DECLARE_MESSAGE_MAP()
};

#define UWM_EDIT_COMPLETE_MSG _T("UWM_EDIT_COMPLETE-{165BBEA0-C1A8-11d5-A04D-006067718D04}")
static UINT UWM_EDIT_COMPLETE = ::RegisterWindowMessage(UWM_EDIT_COMPLETE_MSG);


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RSHELL_H__9F89754B_0627_4BE8_BCFC_2A7CC31C7E04__INCLUDED_)
