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
// SkydanceView.h : Schnittstelle der Klasse CSkydanceView
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SKYDANCEVIEW_H__C6B5798A_F078_11D3_A059_00C026A21151__INCLUDED_)
#define AFX_SKYDANCEVIEW_H__C6B5798A_F078_11D3_A059_00C026A21151__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "LeftView.h"	//von mir!
#include "SkyFiles.h"	// Hinzugefügt von der Klassenansicht

class CSkydanceView : public CListView
{
protected: // Nur aus Serialisierung erzeugen
	CSkydanceView();
	DECLARE_DYNCREATE(CSkydanceView)

// Attribute
public:
	CSkydanceDoc* GetDocument();

// Operationen
public:

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSkydanceView)
	public:
	virtual void OnDraw(CDC* pDC);  // überladen zum Zeichnen dieser Ansicht
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // das erste mal nach der Konstruktion aufgerufen
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementierung
public:
	void CleanView();
	afx_msg void OnConPast();
	afx_msg void OnUpdateConPast(CCmdUI* pCmdUI);
	CStringArray m_saSelectedFileNamesBuff;
	CStringArray m_saSelectedFilesBuff;
	BOOL m_bMoveBuff;
	BOOL m_bCopyBuff;
	CStringArray m_saSelectedFileNames;
	int m_iSelectedFolderNr;
	CStringArray m_saSelectedFiles;
	BOOL m_bCopyPath;
//	CString m_sFirstSelectedFullName;
	int m_iFirstSelectedTypeNr;
	CString m_sFirstSelectedName;
	int m_iFirstSelectedFile;
	int m_iFilesSelected;
static int CALLBACK CompareName(LPARAM lParam1, LPARAM lParam2, LPARAM lFiles)//lParamSort)
	{
		CSkyFiles* pSkyFiles = (CSkyFiles*) lFiles;
		int iReturn;
		CString strItem1;
		CString strItem2;
		switch (pSkyFiles->m_iActiveColumn)
		{
		case 0:		strItem1 = pSkyFiles->m_sFileName[lParam1];
					strItem2 = pSkyFiles->m_sFileName[lParam2];
					strItem1.MakeUpper();
					strItem2.MakeUpper();
//					if(m_bOrder)
//					{
						iReturn = strcmp(strItem1, strItem2);
//						m_bOrder = FALSE;
//					}
//					else
//					{
//						iReturn = strcmp(strItem2, strItem1);
//						m_bOrder = TRUE;
//					}
					break;
		case 1:		strItem1 = pSkyFiles->m_sTypeName[lParam1];
					strItem2 = pSkyFiles->m_sTypeName[lParam2];
					strItem1.MakeUpper();
					strItem2.MakeUpper();
//					m_bOrder = TRUE;
//					if(m_bOrder)
//					{
						iReturn = strcmp(strItem1, strItem2);
//						m_bOrder = FALSE;
//					}
//					else
//					{
//						iReturn = strcmp(strItem2, strItem1);
//						m_bOrder = TRUE;
//					}
					break;
		default:	iReturn = 1;
		}
		return iReturn;
	}
	CSkyFiles *m_pSkyFiles;
	CString FileTypeName(CString fileName);
	int FileTypeNr(CString fileName);
	CImageList m_bmpSmallIcons;
	void SortFileNames();
	CMapWordToOb *m_mFileTb;
	static CSkydanceView * GetView();
	virtual ~CSkydanceView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CSkydanceView)
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnConCopy();
	afx_msg void OnConCopypath();
	afx_msg void OnConDelete();
	afx_msg void OnConDownload();
	afx_msg void OnConExecuteWithoutparameter();
	afx_msg void OnConExecuteWithparameters();
	afx_msg void OnUpdateConCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConCopypath(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConDownload(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConExecuteWithoutparameter(CCmdUI* pCmdUI);
	afx_msg void OnUpdateConExecuteWithparameters(CCmdUI* pCmdUI);
	afx_msg void OnConExecuteWithcopy();
	afx_msg void OnUpdateConExecuteWithcopy(CCmdUI* pCmdUI);
	afx_msg void OnConMove();
	afx_msg void OnUpdateConMove(CCmdUI* pCmdUI);
	afx_msg void OnConCopytobuffer();
	afx_msg void OnUpdateConCopytobuffer(CCmdUI* pCmdUI);
	afx_msg void OnConMovetobuff();
	afx_msg void OnUpdateConMovetobuff(CCmdUI* pCmdUI);
//	afx_msg void OnConPast();
//	afx_msg void OnUpdateConPast(CCmdUI* pCmdUI);
	afx_msg void OnConUpload();
	afx_msg void OnUpdateConUpload(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
private:
	CLeftView* GetLeftView();
};

#ifndef _DEBUG  // Testversion in SkydanceView.cpp
inline CSkydanceDoc* CSkydanceView::GetDocument()
   { return (CSkydanceDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_SKYDANCEVIEW_H__C6B5798A_F078_11D3_A059_00C026A21151__INCLUDED_)
