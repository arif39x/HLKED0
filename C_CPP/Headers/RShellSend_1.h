#if !defined(AFX_RSHELLSEND_H__508D0896_0EFC_42D3_A7FE_6EEF10890FCF__INCLUDED_)
#define AFX_RSHELLSEND_H__508D0896_0EFC_42D3_A7FE_6EEF10890FCF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RShellSend.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRShellSend window

class CRShellSend : public CEdit
{
// Construction
public:
	CRShellSend();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRShellSend)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRShellSend();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRShellSend)
	afx_msg void OnKillfocus();
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RSHELLSEND_H__508D0896_0EFC_42D3_A7FE_6EEF10890FCF__INCLUDED_)
