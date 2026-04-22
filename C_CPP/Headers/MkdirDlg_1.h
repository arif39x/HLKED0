#if !defined(AFX_MKDIRDLG_H__C67B9C4D_7310_41FB_B199_D45BA8E28FB5__INCLUDED_)
#define AFX_MKDIRDLG_H__C67B9C4D_7310_41FB_B199_D45BA8E28FB5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MkdirDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMkdirDlg dialog

class CMkdirDlg : public CDialog
{
// Construction
public:
	CMkdirDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMkdirDlg)
	enum { IDD = IDD_MKDIRDLG };
	CEdit	m_DirNameBox;
	//}}AFX_DATA

	char	dir[MAX_PATH];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMkdirDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMkdirDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MKDIRDLG_H__C67B9C4D_7310_41FB_B199_D45BA8E28FB5__INCLUDED_)
