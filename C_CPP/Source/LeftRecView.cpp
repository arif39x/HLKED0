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
// LeftRecView.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "Skydance.h"
#include "LeftRecView.h"
#include "SkydanceDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftRecView

IMPLEMENT_DYNCREATE(CLeftRecView, CListView)

CLeftRecView::CLeftRecView()
{
	m_iCounter = 0;
}

CLeftRecView::~CLeftRecView()
{
}


BEGIN_MESSAGE_MAP(CLeftRecView, CListView)
	//{{AFX_MSG_MAP(CLeftRecView)
		// HINWEIS - Der Klassen-Assistent fügt hier Zuordnungsmakros ein und entfernt diese.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Zeichnung CLeftRecView 

void CLeftRecView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
	CSkydanceDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// ZU ERLEDIGEN: Code zum Zeichnen hier einfügen
}

CSkydanceDoc* CLeftRecView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkydanceDoc)));
	return (CSkydanceDoc*)m_pDocument;
}
/////////////////////////////////////////////////////////////////////////////
// Diagnose CLeftRecView

#ifdef _DEBUG
void CLeftRecView::AssertValid() const
{
	CListView::AssertValid();
}

void CLeftRecView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten CLeftRecView 

void CLeftRecView::OnInitialUpdate() 
{
	CListView::OnInitialUpdate();
	
	CListCtrl &listCtrl = GetListCtrl();
	listCtrl.InsertColumn(0, "Action", LVCFMT_LEFT, 700);
	//listCtrl.InsertColumn(1, "Time", LVCFMT_CENTER, 150);
	//listCtrl.InsertColumn(2, "Size", LVCFMT_CENTER, 250);
	m_bmpSmallIcons.Create(IDB_BMPREC, 16, 0, RGB(246, 232, 255));
	listCtrl.SetImageList(&m_bmpSmallIcons, LVSIL_SMALL);//IDB_BMPSMFILES
	listCtrl.InsertItem(m_iCounter, "Ready to connect!", 0);
	m_iCounter++;
	// TODO: Speziellen Code hier einfügen und/oder Basisklasse aufrufen
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_pLeftRecView = this;
	ModifyStyle(LVS_TYPEMASK, LVS_REPORT);
}

void CLeftRecView::AddEvent(CString sLastEvent, int iPic)
{
	CListCtrl &listCtrl = GetListCtrl();
	listCtrl.InsertItem(m_iCounter, sLastEvent, iPic);
	listCtrl.EnsureVisible(m_iCounter, NULL);
	m_iCounter++;
}

