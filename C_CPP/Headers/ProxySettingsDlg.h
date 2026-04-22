#if !defined(AFX_PROXYSETTINGSDLG_H__AF4C774E_3E03_43F2_B94A_A1D2470C139B__INCLUDED_)
#define AFX_PROXYSETTINGSDLG_H__AF4C774E_3E03_43F2_B94A_A1D2470C139B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProxySettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProxySettingsDlg dialog

class CProxySettingsDlg : public CDialog
{
// Construction
public:
	CProxySettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProxySettingsDlg)
	enum { IDD = IDD_PROXYDLG };
	CEdit	m_EditLogFile;
	int		m_Port;
	BOOL	m_KeepAlive;
	int		m_Timeout;
	BOOL	m_LoggingEnabled;
	//}}AFX_DATA

	char	path[MAX_PATH];
	int		port;
	int		timeout;
	BOOL	keepalive;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProxySettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProxySettingsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPle();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROXYSETTINGSDLG_H__AF4C774E_3E03_43F2_B94A_A1D2470C139B__INCLUDED_)
