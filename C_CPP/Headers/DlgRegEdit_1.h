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
#if !defined(AFX_DLGREGEDIT_H__436D8901_559F_11D4_A059_00C026A21151__INCLUDED_)
#define AFX_DLGREGEDIT_H__436D8901_559F_11D4_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgRegEdit.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgRegEdit 

class CDlgRegEdit : public CDialog
{
// Konstruktion
public:
	BOOL m_bLock;
	BOOL CheckEdit();
	CDlgRegEdit(CWnd* pParent = NULL);   // Standardkonstruktor

// Dialogfelddaten
	//{{AFX_DATA(CDlgRegEdit)
	enum { IDD = IDD_DLGREG };
	CListBox	m_lstReg;
	CButton	m_BtLock;
	CString	m_sKey;
	CString	m_sValue;
	int		m_iValueType;
	int		m_iComboKey;
	CString	m_sValName;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgRegEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgRegEdit)
	afx_msg void OnBtrequest();
	afx_msg void OnBtdelkey();
	afx_msg void OnBtset();
	afx_msg void OnBtreglock();
	afx_msg void OnBtcompinfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGREGEDIT_H__436D8901_559F_11D4_A059_00C026A21151__INCLUDED_
