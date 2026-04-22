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
// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "Skydance.h"

#include "MainFrm.h"
#include "LeftView.h"
#include "SkydanceView.h"
#include "LeftRecView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_STAT1,
	ID_INDICATOR_STAT2,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
	// ZU ERLEDIGEN: Hier Code zur Member-Initialisierung einfügen
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	// ZU ERLEDIGEN: Löschen Sie diese drei Zeilen, wenn Sie nicht wollen, dass die Symbolleiste
	//  andockbar ist.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
	CCreateContext* pContext)
{
	// Unterteiltes Fenster erstellen
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(200, 100), pContext))// ||
		//  !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CSkydanceView), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}


	// add the second splitter pane - which is a nested splitter with 2 rows
	if (!m_wndSplitter2.CreateStatic(
		&m_wndSplitter,     // our parent window is the first splitter
		2, 1,               // the new splitter is 2 rows, 1 column
		WS_CHILD | WS_VISIBLE | WS_BORDER,  // style, WS_BORDER is needed
		m_wndSplitter.IdFromRowCol(0, 1)
			// new splitter is in the first row, 2nd column of first splitter
	   ))
	{
		//TRACE0("Failed to create nested splitter\n");
		return FALSE;
	}


	//int cyRview = lpcs->cy - 100;    // height of RView pane

	if(!m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CSkydanceView), CSize(0, 500), pContext) ||
		!m_wndSplitter2.CreateView(1, 0, RUNTIME_CLASS(CLeftRecView), CSize(0, 50), pContext))
	{
		//TRACE0("Failed to create second pane\n");
		m_wndSplitter2.DestroyWindow();
		return FALSE;
	}
	return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler

CSkydanceView* CMainFrame::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter2.GetPane(0, 0);
	CSkydanceView* pView = DYNAMIC_DOWNCAST(CSkydanceView, pWnd);
	return pView;
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
 	// ZU ERLEDIGEN: Passen Sie diesen Code an, damit er Auswahlen im Menü
 	// Ansicht behandeln kann.
 
 	CSkydanceView* pView = GetRightPane(); 

// Wurde der rechte Ausschnitt nicht erstellt oder ist dieser keine Ansicht,
// dann deaktivieren Sie die Befehle in unserem Bereich

	if (pView == NULL)
	pCmdUI->Enable(FALSE);
	else
	{
	DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

// Ist der Befehl ID_VIEW_LINEUP, dann aktivieren Sie den Befehl nur, wenn
 		// wir im Modus LVS_ICON oder LVS_SMALLICON sind
 
 		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
 		{
 			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
 				pCmdUI->Enable();
 			else
 				pCmdUI->Enable(FALSE);
 		}
 		else
 		{
 			// andernfalls verwenden Sie Punkte, um das Format der Ansicht widerzuspiegeln
 			pCmdUI->Enable();
			BOOL bChecked = FALSE;
 
		switch (pCmdUI->m_nID)
 			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
 				break;

			case ID_VIEW_SMALLICON:
 				bChecked = (dwStyle == LVS_SMALLICON);
 				break;
 
 			case ID_VIEW_LARGEICON:
 				bChecked = (dwStyle == LVS_ICON);
 				break;
 
 			case ID_VIEW_LIST:
 				bChecked = (dwStyle == LVS_LIST);
 				break;
 
 			default:
 				bChecked = FALSE;
				break;
			}
 
 			pCmdUI->SetRadio(bChecked ? 1 : 0);
 		}
	}
}


void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// ZU ERLEDIGEN: Passen Sie diesen Code an, damit er Auswahlen im Menü
	// Ansicht behandeln kann.
	CSkydanceView* pView = GetRightPane();

	// Wenn der rechte Ausschnitt erstellt wurde und CSkydanceView ist,
	// dann verarbeiten Sie die Menübefehle...
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
				// Das Listen-Steuerelement soll sich am Raster ausrichten
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

		// Weitere Befehle ändern das Format im Listen-Steuerelement
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

		// Format ändern; das Fenster baut sich automatisch neu auf
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}

//DEL void CMainFrame::OnBtregedit() 
//DEL {
//DEL 	// TODO: Code für Befehlsbehandlungsroutine hier einfügen
//DEL 	
//DEL }

//DEL void CMainFrame::OnUpdateBtregedit(CCmdUI* pCmdUI) 
//DEL {
//DEL 	// TODO: Code für die Befehlsbehandlungsroutine zum Aktualisieren der Benutzeroberfläche hier einfügen
//DEL 	
//DEL }
