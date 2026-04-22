#if !defined(AFX_BUILDERDLG_H__0DAFD7DC_09E0_466C_86B3_A89DABF42FCE__INCLUDED_)
#define AFX_BUILDERDLG_H__0DAFD7DC_09E0_466C_86B3_A89DABF42FCE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BuilderDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CBuilderDlg dialog

	typedef struct
	{
		int		block;
		int		pos;
	} pos_s;

class CBuilderDlg : public CDialog
{
// Construction
public:
	CBuilderDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBuilderDlg)
	enum { IDD = IDD_BUILDDLG };
	CEdit	m_InputError;
	CEdit	m_InputFileName;
	CEdit	m_InputName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuilderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	char	*servername;
	char	*errormessage;

	// Generated message map functions
	//{{AFX_MSG(CBuilderDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg pos_s FindDataPosition(char *data, int len);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDERDLG_H__0DAFD7DC_09E0_466C_86B3_A89DABF42FCE__INCLUDED_)
