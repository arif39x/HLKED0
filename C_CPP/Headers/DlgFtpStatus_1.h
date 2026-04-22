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
#if !defined(AFX_DLGFTPSTATUS_H__81B70780_F2B3_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_DLGFTPSTATUS_H__81B70780_F2B3_11D3_A059_00C026A21151__INCLUDED_

#include "SkySocket.h"	// Hinzugefügt von der Klassenansicht
#include "SkyThread.h"	//von mir
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgFtpStatus.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld CDlgFtpStatus 

class CDlgFtpStatus : public CDialog
{
// Konstruktion
public:
	CString m_sFileNameServer;
	BOOL m_bServerUpdate;
	CMutex m_mutex;
	BOOL m_bTransferDone;
	CString m_sCancel;
	//	CString m_sFileNameServer;
//	int m_iInitFile; //noch nötig?
	int m_iNextFtpStatNr;
	CSkyThread * m_cSkyThread;
	CDlgFtpStatus(int socketNr, bool scWay, int ftpPort, CString sActiveIP, 
				bool UpDown, CString sFileName, CString sFileNameServer, CWnd* pParent = NULL);   // Standardkonstruktor
	void UpdateStat();
// Dialogfelddaten
	//{{AFX_DATA(CDlgFtpStatus)
	enum { IDD = IDD_DLGFTPSTATUS };
	CButton	m_btCancel;
	CString	m_sFileName;
	CString	m_sGoal;
	CString	m_sDirection;
	CString	m_sCurrentStat;
	//}}AFX_DATA


// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CDlgFtpStatus)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CDlgFtpStatus)
	virtual void OnCancel();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_DLGFTPSTATUS_H__81B70780_F2B3_11D3_A059_00C026A21151__INCLUDED_
