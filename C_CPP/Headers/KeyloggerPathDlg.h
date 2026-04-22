#if !defined(AFX_KEYLOGGERPATHDLG_H__63B996A3_4FFF_44CD_9B3B_5C5D95C6E96E__INCLUDED_)
#define AFX_KEYLOGGERPATHDLG_H__63B996A3_4FFF_44CD_9B3B_5C5D95C6E96E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KeyloggerPathDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKeyloggerPathDlg dialog

class CKeyloggerPathDlg : public CDialog
{
// Construction
public:
	CKeyloggerPathDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKeyloggerPathDlg)
	enum { IDD = IDD_KEYLOGGERPATH };
	CEdit	m_InputPath;
	//}}AFX_DATA

	char	path[MAX_PATH];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKeyloggerPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CKeyloggerPathDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEYLOGGERPATHDLG_H__63B996A3_4FFF_44CD_9B3B_5C5D95C6E96E__INCLUDED_)
