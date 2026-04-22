#if !defined(AFX_INFODLG_H__50317A7C_BF14_4D2D_8451_DE45C2674291__INCLUDED_)
#define AFX_INFODLG_H__50317A7C_BF14_4D2D_8451_DE45C2674291__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InfoDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog

class CInfoDlg : public CDialog
{
// Construction
public:
	CInfoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInfoDlg)
	enum { IDD = IDD_INFODLG };
	CStatic	m_Status;
	CEdit	m_OutputScreen;
	//}}AFX_DATA

	infotype_s infotype;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString		OutputText;

	// Generated message map functions
	//{{AFX_MSG(CInfoDlg)
	afx_msg void OnExit();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT CInfoDlg::OnInfoMessage(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INFODLG_H__50317A7C_BF14_4D2D_8451_DE45C2674291__INCLUDED_)
