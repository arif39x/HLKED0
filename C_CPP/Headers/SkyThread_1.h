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
#if !defined(AFX_SKYTHREAD_H__1CF024E0_F690_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYTHREAD_H__1CF024E0_F690_11D3_A059_00C026A21151__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SkyThread.h : Header-Datei
//

////////////////////////////////////////////////////////////////////////////
// Thread CSkyThread 

class CSkyThread : public CWinThread
{
	DECLARE_DYNCREATE(CSkyThread)
protected:
	CSkyThread();           // Dynamische Erstellung verwendet geschützten Konstruktor

// Attribute
public:

// Operationen
public:
	int m_iNextFtpStatNr;
	CMutex m_mutex;
	int m_iCurrentBytes;
	void SetStatusValues(int iLastPacket);
	void ReceiveError();
	BOOL m_bTransferBreak;
	CDialog *m_pDialog;
	void SendError();
	CString m_sFileNameServer;
	CString m_sFileName;
	DWORD m_dwBytesWritten;
	DWORD m_dwPos;
	HANDLE m_hAppend;
	HANDLE m_hFile;
	bool m_bUpDown;
	bool m_bSCway;
	CString m_sActiveIP;
	int m_iFtpPort;
	void SocketThreadInit();
	void OnSend();
	void OnClose();
	void OnAccept();
	void OnReceive();
	int ReadFileNow(CString fullName);
	int WriteFileNow(CString fullName, char buff[4096], DWORD nRead);
// Überschreibungen
	// Vom Klassen-Assistenten generierte virtuelle Funktionsüberschreibungen
	//{{AFX_VIRTUAL(CSkyThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementierung
protected:
	virtual ~CSkyThread();
	// Generierte Nachrichtenzuordnungsfunktionen
	//{{AFX_MSG(CSkyThread)
		// HINWEIS - Der Klassen-Assistent fügt hier Member-Funktionen ein und entfernt diese.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // AFX_SKYTHREAD_H__1CF024E0_F690_11D3_A059_00C026A21151__INCLUDED_
