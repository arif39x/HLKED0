#if !defined(AFX_FILELIST_H__EDE6D47F_6608_46CE_B68D_BF7F4BCE27AA__INCLUDED_)
#define AFX_FILELIST_H__EDE6D47F_6608_46CE_B68D_BF7F4BCE27AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileList window

class CFileList : public CListCtrl
{
// Construction
public:
	CFileList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileList)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFileList();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFileList)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILELIST_H__EDE6D47F_6608_46CE_B68D_BF7F4BCE27AA__INCLUDED_)
