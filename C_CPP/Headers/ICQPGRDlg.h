// ICQPGRDlg.h : header file
//

#if !defined(AFX_ICQPGRDLG_H__09EC3B42_0347_4A17_BE26_796BA34ADE3E__INCLUDED_)
#define AFX_ICQPGRDLG_H__09EC3B42_0347_4A17_BE26_796BA34ADE3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CICQPGRDlg dialog

class CICQPGRDlg : public CDialog
{
// Construction
public:
	CICQPGRDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CICQPGRDlg)
	enum { IDD = IDD_ICQPGR_DIALOG };
	CString	m_Message;
	CString	m_Uin;
	BOOL	m_Compress;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CICQPGRDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CICQPGRDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBuild();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ICQPGRDLG_H__09EC3B42_0347_4A17_BE26_796BA34ADE3E__INCLUDED_)
