// inetspy.h : main header file for the INETSPY application
//

#if !defined(AFX_INETSPY_H__23955FC6_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
#define AFX_INETSPY_H__23955FC6_38D0_11D1_BE29_00A024A0D351__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInetspyApp:
// See inetspy.cpp for the implementation of this class
//

class CInetspyApp : public CWinApp
{
public:
	CInetspyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInetspyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInetspyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INETSPY_H__23955FC6_38D0_11D1_BE29_00A024A0D351__INCLUDED_)
