/**********************************************************************************/
// Read this First!
// Skydance 2.291 Beta Release source code for MFC-VC++ 6.0
// by Edrin@gmx.net
//
// http://skd.box.sk
// - USE SKYDANCE/SKYDANCE SOURCE-CODE AND/OR IT`S COMPONENTS ON YOUR OWN RISK -
//
// Introduction:
//	This Source is FREEWARE!
//	SkyServer is a component of Skydance!
//	Skydance is designed to control other Win95/98/NT systems in tcp/ip-nets like Internet and Intranets.
//	Skydance 2.291 Beta Release is a program that will remain "under construction" and i will NEVER be
//	responsible for any HARM or harmfull things that happend because SKYDANCE(or skydance-source) was USED! 
//	That means I do NOT	guarantee for any features and security-thing
//	and i do not guarantee that this program works proper!!!
//	Indeed i guarantee for nothing ;)
//good luck, Edrin
/**********************************************************************************/
#if !defined(AFX_DLGSPECIAL_H__470D0DC8_0328_11D4_A059_00C026A21151__INCLUDED_)
#define AFX_DLGSPECIAL_H__470D0DC8_0328_11D4_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSpecial.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgSpecial 

class CDlgSpecial : public CDialog
{
// Konstruktion
public:
	//CString m_sServerName;
	CString m_sComputerName;
	CDlgSpecial(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgSpecial)
	enum { IDD = IDD_DLGSPECIAL };
	CString	m_sPass;
	CString	m_sFileName;
	BOOL	m_bSettings;
	CString	m_sActionToPerform;
	int		m_iPort;
	BOOL	m_bDlgVisible;
	CString	m_sPassReenter;
	CString	m_sDLLFile;
	CString	m_sLogFile;
	BOOL	m_bKeyLogAlways;
	CString	m_sUIN;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgSpecial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgSpecial)
	afx_msg void OnBtactionsend();
	afx_msg void OnCheck1();
	afx_msg void OnBtchange();
	afx_msg void OnBtsserver();
	afx_msg void OnBtklstart();
	afx_msg void OnBtklstop();
	afx_msg void OnBtremo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGSPECIAL_H__470D0DC8_0328_11D4_A059_00C026A21151__INCLUDED_
