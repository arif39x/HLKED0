#if !defined(AFX_MASSINFO_H__E182290B_1CDD_4490_9E95_A215DEACE3BB__INCLUDED_)
#define AFX_MASSINFO_H__E182290B_1CDD_4490_9E95_A215DEACE3BB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MassInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMassInfo dialog

class CMassInfo : public CDialog
{
// Construction
public:
	CMassInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMassInfo)
	enum { IDD = IDD_MASSINFO };
	CStatic	m_File;
	CStatic	m_Recived;
	CStatic	m_From;
	//}}AFX_DATA

	int		victims;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMassInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMassInfo)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
	afx_msg LRESULT OnInfoMessage(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MASSINFO_H__E182290B_1CDD_4490_9E95_A215DEACE3BB__INCLUDED_)
