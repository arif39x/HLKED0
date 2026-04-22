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
// LeftView.h : Schnittstelle der Klasse CLeftView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEFTVIEW_H__C6B5798C_F078_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_LEFTVIEW_H__C6B5798C_F078_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SkyFolders.h"
#include "DlgNewFolder.h"	// Hinzugefügt von der Klassenansicht
#include "DlgRenameFolder.h"	// Hinzugefügt von der Klassenansicht
class CSkydanceDoc;

class CLeftView : public CTreeView
{
protected: // Nur aus Serialisierung erstellen
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

// Attribute
public:
	CSkydanceDoc* GetDocument();
	CString m_aFolderPath[30000];
	int m_iFolderCounter;
	CString m_aFolderName[30000];
	CString m_sActivePath;
	
// Operationen
public:
	void AddEnvironmentData(CString HDname, CString HDtype, int HDcounter );
// Überladungen
	// Überladungen der vom Klassen-Assistenten erstellten virtuellen Funktion
	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  // Überladen, um diese Ansicht darzustellen
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // Erster Aufruf nach Erstellung
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	CDlgRenameFolder m_cDlgRenameFolder;
	CDlgNewFolder m_cDlgNewFolder;
	int m_iEnvironmentMax;
	BOOL m_bFolderHit;
	CString m_sActiveCPath;
	HTREEITEM m_hSelectedItem;
	CMapWordToOb *m_mFolderTb;
	int GetSelectedItemData();
	void CleanView();
	bool m_bGetFolder[30000];
	static CLeftView * GetView();
	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Erstellte Funktionen für die Nachrichtenumsetzung
protected:
	CImageList *m_HDImages;
	HTREEITEM *m_rghItem;
	//{{AFX_MSG(CLeftView)
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateLconCreate(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLconDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLconRename(CCmdUI* pCmdUI);
	afx_msg void OnLconCreate();
	afx_msg void OnLconDelete();
	afx_msg void OnLconRename();
	afx_msg void OnLconCopypath();
	afx_msg void OnUpdateLconCopypath(CCmdUI* pCmdUI);
	afx_msg void OnLconPast();
	afx_msg void OnUpdateLconPast(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // Debug-Version in LeftView.cpp
inline CSkydanceDoc* CLeftView::GetDocument()
   { return (CSkydanceDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_LEFTVIEW_H__C6B5798C_F078_11D3_A059_00C026A21151__INCLUDED_)
