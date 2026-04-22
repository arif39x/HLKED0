#if !defined(AFX_TASKMGRDLG_H__44A85EEC_A0A1_45AB_AA7E_D24F55A942C9__INCLUDED_)
#define AFX_TASKMGRDLG_H__44A85EEC_A0A1_45AB_AA7E_D24F55A942C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskMgrDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskMgrDlg dialog

class CTaskMgrDlg : public CDialog
{
// Construction
public:
	CTaskMgrDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskMgrDlg)
	enum { IDD = IDD_TASKMGRDLG };
	CListCtrl	m_ProcessList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskMgrDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskMgrDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRefreshbut();
	afx_msg void OnKillbut();
	afx_msg void OnNewtaskbut();
	virtual void OnOK();
	//}}AFX_MSG
	afx_msg LRESULT CTaskMgrDlg::OnProcessMessage(UINT wParam, LONG lParam);
	afx_msg LRESULT CTaskMgrDlg::OnLostConnection(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKMGRDLG_H__44A85EEC_A0A1_45AB_AA7E_D24F55A942C9__INCLUDED_)
