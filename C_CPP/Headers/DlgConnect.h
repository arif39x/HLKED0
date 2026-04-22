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
#if !defined(AFX_DLGCONNECT_H__D74A4BC0_F129_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_DLGCONNECT_H__D74A4BC0_F129_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgConnect.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgConnect 

class CDlgConnect : public CDialog
{
// Konstruktion
public:
	CDlgConnect(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgConnect)
	enum { IDD = IDD_DLGCONNECT };
	CString	m_sIP;
	int		m_iPort;
	CString	m_sPass;
	int		m_iPassword;
	BOOL	m_bUpEnvir;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgConnect)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgConnect)
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGCONNECT_H__D74A4BC0_F129_11D3_A059_00C026A21151__INCLUDED_
