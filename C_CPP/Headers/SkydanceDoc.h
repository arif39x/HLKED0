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
// SkydanceDoc.h : Schnittstelle der Klasse CSkydanceDoc
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKYDANCEDOC_H__C6B57988_F078_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYDANCEDOC_H__C6B57988_F078_11D3_A059_00C026A21151__INCLUDED_

#include "DlgNewFolder.h"	// Hinzugefügt von der Klassenansicht
#include "DlgMove.h"	// Hinzugefügt von der Klassenansicht
#include "DlgCopy.h"	// Hinzugefügt von der Klassenansicht
#include "SkySocket.h"	// Hinzugefügt von der Klassenansicht
#include "DlgConnect.h"	// Hinzugefügt von der Klassenansicht
#include "DlgMessageBox.h"	//von mir
#include "DlgFtp.h"		//von mir
#include "DlgFtpStatus.h"	//auch von mir...
#include "DlgRun.h"	// Hinzugefügt von der Klassenansicht
#include "DlgDelete.h"	// Hinzugefügt von der Klassenansicht
#include "DlgSpecial.h"	//von mir
#include "LeftView.h"	// Hinzugefügt von der Klassenansicht
#include "SkydanceView.h"	// Hinzugefügt von der Klassenansicht
#include "MainFrm.h"	// Hinzugefügt von der Klassenansicht
#include "DlgTasks.h"	// Hinzugefügt von der Klassenansicht
#include "LeftRecView.h"	// von mir
#include "DlgRegEdit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSkydanceDoc : public CDocument
{
protected: // Nur aus Serialisierung erzeugen
	CSkydanceDoc();
	DECLARE_DYNCREATE(CSkydanceDoc)

// Attribute
public:
	bool m_bFreeSocket[80];
	CString m_sStat1;
	CString m_sStat2;
	int m_iNextFtpStatNr;
	CString m_sActiveIP;
	CDlgFtpStatus *m_cDlgFtpStatus[40];
	int m_iConnected;
	int m_iNavigateMode;
	bool m_bFolderFile;
// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSkydanceDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementierung
public:
	BOOL m_bServerUpdate;
	BOOL m_bDlgRegExists;
	CLeftRecView *m_pLeftRecView;
	CDlgNewFolder m_cDlgNewFolder;
	CDlgMove m_cDlgMove;
	int m_iNumberOfTasks;
	CDlgTasks m_cDlgTasks;
	BOOL m_bDlgSpecialExists;
	BOOL m_bDlgTasksExists;
	CDlgSpecial m_cDlgSpecial;
	CString m_sCopyPathOfFile;
	CSkydanceView *m_pSkydanceView;
	CLeftView *m_pLeftView;
	void UpdateOtherViews();
	void CleanAllViews();
	int m_iFolderFileTask;
	int m_iHDcounter;
	CString m_aData[10000];
	int m_iNumberOfFolders;
	int m_iNumberOfFiles;
	void AnalyseString(CString strToAnalyse);
	void OnReceive();
	CDlgRun m_cDlgRun;
	CDlgRegEdit m_cDlgReg;
	CDlgCopy m_cDlgCopy;
	CDlgFtp m_cDlgFtp;
	CDlgDelete m_cDlgDelete;
	void CreateFtpSession(int socketNr, int ftpPort,bool scWay, 
							bool UpDown, CString sFileNameC, CString sFileNameS, 
							BOOL bServerUpdate = FALSE);
	void SendPack(CString Pack);
	void OnSend();
	void OnClose();
	static CSkydanceDoc * GetDoc();
	virtual ~CSkydanceDoc();
		//
	afx_msg void OnBtcopy();
	afx_msg void OnBtdelete();
	afx_msg void OnBtftp();
	afx_msg void OnBtrun();
	afx_msg void OnBtnewfolder();
	afx_msg void OnBtmove();
	//
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	afx_msg void OnUpdateIndicatorStat1(CCmdUI * pCmdUI);
	afx_msg void OnUpdateIndicatorStat2(CCmdUI * pCmdUI);
	//{{AFX_MSG(CSkydanceDoc)
	afx_msg void OnBtconnect();
	afx_msg void OnUpdateStealthMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUpdateMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDiscoveryMode(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFolder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFolderfile(CCmdUI* pCmdUI);
	afx_msg void OnDiscoveryMode();
	afx_msg void OnFolder();
	afx_msg void OnFolderfile();
	afx_msg void OnUpdateMode();
	afx_msg void OnStealthMode();
	afx_msg void OnUpdateBtconnect(CCmdUI* pCmdUI);
	afx_msg void OnBtregedit();
	afx_msg void OnUpdateBtregedit(CCmdUI* pCmdUI);
	afx_msg void OnBtmsg();
	afx_msg void OnBtspecial();
	afx_msg void OnUpdateBtcopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtdelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtftp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtmsg(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtrun(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtspecial(CCmdUI* pCmdUI);
	afx_msg void OnBtdisconnect();
	afx_msg void OnUpdateBtdisconnect(CCmdUI* pCmdUI);
	afx_msg void OnBttasks();
	afx_msg void OnUpdateBttasks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtmove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnewfolder(CCmdUI* pCmdUI);
	afx_msg void OnBtupdate();
	afx_msg void OnUpdateBtupdate(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSkySocket *m_cSkySocket;
	CDlgMessageBox m_cDlgMessageBox;
	CDlgConnect m_cDlgConnect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SKYDANCEDOC_H__C6B57988_F078_11D3_A059_00C026A21151__INCLUDED_)
