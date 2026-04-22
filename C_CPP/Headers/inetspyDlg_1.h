// inetspyDlg.h : header file
//

#if !defined(AFX_INETSPYDLG_H__23955FC8_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
#define AFX_INETSPYDLG_H__23955FC8_38D0_11D1_BE29_00A024A0D351__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CInetspyDlg dialog

class CInetspyDlg : public CDialog
{
// Construction
public:
	CInetspyDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CInetspyDlg)
	enum { IDD = IDD_INETSPY_DIALOG };
	CString	m_sAddress;
	UINT	m_iPort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInetspyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CInetspyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnChangeAddress();
	afx_msg void OnListen();
	afx_msg void OnConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INETSPYDLG_H__23955FC8_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
