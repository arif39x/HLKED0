#if !defined(AFX_SETTINGS_H__AF841284_0438_4AA7_A9C4_B5C82169784E__INCLUDED_)
#define AFX_SETTINGS_H__AF841284_0438_4AA7_A9C4_B5C82169784E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Settings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Settings dialog

class Settings : public CDialog
{
// Construction
public:
	Settings(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Settings)
	enum { IDD = IDD_SETTINGS };
	CEdit	m_LoginCMD;
	int		m_ListenPort;
	int		m_MaxVictims;
	int		m_PingInterval;
	int		m_TransferPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Settings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Settings)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGS_H__AF841284_0438_4AA7_A9C4_B5C82169784E__INCLUDED_)
