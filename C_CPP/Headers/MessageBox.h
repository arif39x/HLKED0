#if !defined(AFX_MESSAGEBOX_H__575BDCE1_15AF_4410_A3D7_11870931FCEF__INCLUDED_)
#define AFX_MESSAGEBOX_H__575BDCE1_15AF_4410_A3D7_11870931FCEF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MessageBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMessageBox dialog

class CMessageBox : public CDialog
{
// Construction
public:
	CMessageBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMessageBox)
	enum { IDD = IDD_MSGBOX };
	CEdit	m_MsgBoxText;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMessageBox)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MESSAGEBOX_H__575BDCE1_15AF_4410_A3D7_11870931FCEF__INCLUDED_)
