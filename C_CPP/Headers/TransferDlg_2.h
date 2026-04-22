#if !defined(AFX_TRANSFERDLG_H__25C8C58A_1C85_4DE9_BAB5_EDB7393CE2AF__INCLUDED_)
#define AFX_TRANSFERDLG_H__25C8C58A_1C85_4DE9_BAB5_EDB7393CE2AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransferDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransferDlg dialog

class CTransferDlg : public CDialog
{
// Construction
public:
	CTransferDlg(CWnd* pParent = NULL);   // standard constructor

	int				bindport;
	unsigned long	fsize;
	char			remotefile[MAX_PATH];
	char			localfile[MAX_PATH];
	int				transfer;

// Dialog Data
	//{{AFX_DATA(CTransferDlg)
	enum { IDD = IDD_TRANSFERDLG };
	CStatic	m_RemoteStatus;
	CButton	m_Button;
	CStatic	m_InfoText;
	CProgressCtrl	m_ProgressBar;
	CStatic	m_ProgressText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransferDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransferDlg)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT CTransferDlg::OnNewStep(UINT wParam, LONG lParam);
	afx_msg LRESULT CTransferDlg::OnFinished(UINT wParam, LONG lParam);
	afx_msg LRESULT CTransferDlg::OnError(UINT wParam, LONG lParam);
	afx_msg void CTransferDlg::SetProgressBar();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSFERDLG_H__25C8C58A_1C85_4DE9_BAB5_EDB7393CE2AF__INCLUDED_)
