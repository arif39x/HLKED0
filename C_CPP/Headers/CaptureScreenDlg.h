#if !defined(AFX_CAPTURESCREENDLG_H__CBB9051E_CCD0_46B0_8DB3_D56F5EE59AFA__INCLUDED_)
#define AFX_CAPTURESCREENDLG_H__CBB9051E_CCD0_46B0_8DB3_D56F5EE59AFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CaptureScreenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCaptureScreenDlg dialog

class CCaptureScreenDlg : public CDialog
{
// Construction
public:
	CCaptureScreenDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCaptureScreenDlg)
	enum { IDD = IDD_CAPTUREDLG };
	CEdit	m_InputFile;
	//}}AFX_DATA

	char	file[MAX_PATH];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCaptureScreenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCaptureScreenDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CAPTURESCREENDLG_H__CBB9051E_CCD0_46B0_8DB3_D56F5EE59AFA__INCLUDED_)
