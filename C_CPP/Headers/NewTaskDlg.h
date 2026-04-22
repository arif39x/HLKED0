#if !defined(AFX_NEWTASKDLG_H__C17A735F_745C_4807_8D74_E226C01C38FC__INCLUDED_)
#define AFX_NEWTASKDLG_H__C17A735F_745C_4807_8D74_E226C01C38FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg dialog

class CNewTaskDlg : public CDialog
{
// Construction
public:
	CNewTaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewTaskDlg)
	enum { IDD = IDD_NEWTASKDLG };
	CEdit	m_NTInput;
	//}}AFX_DATA

	char	command[MAX_PATH];


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewTaskDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTASKDLG_H__C17A735F_745C_4807_8D74_E226C01C38FC__INCLUDED_)
