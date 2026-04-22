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
#if !defined(AFX_SKYSOCKET_H__1BC3334C_DE4A_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYSOCKET_H__1BC3334C_DE4A_11D3_A059_00C026A21151__INCLUDED_

#include "SkyThread.h"	// Hinzugefügt von der Klassenansicht
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkySocket.h : Header-Datei
//

/////////////////////////////////////////////////////////////////////////////
// Befehlsziel CSkySocket 

class CSkySocket : public CSocket
{
// Attribute
public:

// Operationen
public:
	void SetParent(CDialog* pDlg);
	CSkySocket(int i);
	CSkySocket();
	virtual ~CSkySocket();
	CSkyThread *m_cSkyThread;
// Überschreibungen
public:
	void SetParentThread(CSkyThread *pSkyThread);
	int m_iSocketType;
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSkySocket)
	//}}AFX_VIRTUAL

	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSkySocket)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG

// Implementierung
protected:
	virtual void OnAccept(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
private:
	CDialog* m_pDlg;
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SKYSOCKET_H__1BC3334C_DE4A_11D3_A059_00C026A21151__INCLUDED_
