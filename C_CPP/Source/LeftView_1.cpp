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
// LeftView.cpp : Implementierung der Klasse CLeftView
//

#include "stdafx.h"
#include "Skydance.h"

#include "SkydanceDoc.h"
#include "LeftView.h"
#include "SkydanceView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CTreeView)
	//{{AFX_MSG_MAP(CLeftView)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_UPDATE_COMMAND_UI(ID_LCON_CREATE, OnUpdateLconCreate)
	ON_UPDATE_COMMAND_UI(ID_LCON_DELETE, OnUpdateLconDelete)
	ON_UPDATE_COMMAND_UI(ID_LCON_RENAME, OnUpdateLconRename)
	ON_COMMAND(ID_LCON_CREATE, OnLconCreate)
	ON_COMMAND(ID_LCON_DELETE, OnLconDelete)
	ON_COMMAND(ID_LCON_RENAME, OnLconRename)
	ON_COMMAND(ID_LCON_COPYPATH, OnLconCopypath)
	ON_UPDATE_COMMAND_UI(ID_LCON_COPYPATH, OnUpdateLconCopypath)
	ON_COMMAND(ID_LCON_PAST, OnLconPast)
	ON_UPDATE_COMMAND_UI(ID_LCON_PAST, OnUpdateLconPast)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeftView Erstellung/Zerstörung

CLeftView::CLeftView()
{
	// ZU ERLEDIGEN: Fügen Sie hier den Erstellungs-Code hinzu
	m_rghItem = (HTREEITEM* ) new HTREEITEM[10000];
	m_mFolderTb = new CMapWordToOb;
	m_iFolderCounter = 1;	//wieso geht das HTREEITEM-array erst mit 1 los und nicht mit 0???
	m_sActivePath = "";				//mit 1 vermutlich weil ein Array undefinierter Größe!
	m_hSelectedItem = NULL;	//UNBEDINGT auf NULL, weil am anfang noch kein item gewählt ist (OnUpdate ListView!)
	m_bFolderHit = FALSE;
	m_sActiveCPath = "Teste";
	m_iEnvironmentMax = 0;
}

CLeftView::~CLeftView()
{
	delete m_rghItem;
}

CLeftView * CLeftView::GetView()
{
      CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

      CView * pView = pFrame->GetActiveView();

      if ( !pView )
         return NULL;

      // Fail if view is of wrong kind
      // (this could occur with splitter windows, or additional
      // views on a single document
      if ( ! pView->IsKindOf( RUNTIME_CLASS(CLeftView) ) )
         return NULL;

      return (CLeftView *) pView;
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklassse oder -formate, indem
	//  CREATESTRUCT cs ändern
	cs.style |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_EDITLABELS; //Baum-Stiel desingen!

	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView Darstellung

void CLeftView::OnDraw(CDC* pDC)
{
	CSkydanceDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// ZU ERLEDIGEN: Fügen Sie hier den Zeichen-Code für systemeigene Daten hinzu
}



void CLeftView::OnInitialUpdate()
{
	m_HDImages = new CImageList;
	CTreeCtrl& folderTree = GetTreeCtrl();
	m_HDImages->Create( IDB_BMPHD, 16, 1, RGB(0, 255, 0) );
	folderTree.SetImageList(m_HDImages, TVSIL_NORMAL);
	CTreeView::OnInitialUpdate();
	// ZU ERLEDIGEN: Sie können Ihre Baumansicht mit Elementen füllen, indem Sie direkt
	//  auf dessen Baum-Steuerelement über einen Aufruf von GetTreeCtrl() zugreifen.
	if(m_iFolderCounter != 1) return;//AfxMessageBox("Error while initialising Treeview!");
	m_mFolderTb->SetAt(m_iFolderCounter, new CSkyFolders());//DWORD darf nicht < 0 sein!
	CSkyFolders *pSkyFolders = NULL;
	if (m_mFolderTb->Lookup(m_iFolderCounter, (CObject*&)pSkyFolders))
	{
		m_rghItem[m_iFolderCounter] = folderTree.InsertItem(_T("Host"), 0, 0, NULL, TVI_SORT);
		pSkyFolders->m_sFolderName = "Host";
		pSkyFolders->m_sFolderPath = "";
		pSkyFolders->m_bFolderStatus = FALSE;
		folderTree.SetItemData(m_rghItem[m_iFolderCounter], m_iFolderCounter);
		folderTree.SetItemImage( m_rghItem[m_iFolderCounter], 10, 10);
		m_iFolderCounter++;
	}
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_pLeftView = this;
}

/////////////////////////////////////////////////////////////////////////////
// CLeftView Diagnose

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
	CTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

CSkydanceDoc* CLeftView::GetDocument() // Nicht-Debug-Version ist inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkydanceDoc)));
	return (CSkydanceDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLeftView Nachrichten-Behandlungsroutinen

//DEL void CLeftView::OnLButtonUp(UINT nFlags, CPoint point) 
//DEL {
//DEL 	//GetDocument()->UpdateAllViews(this); //einfach updateAllViews ohne sich slebst zu updaten! Richtiger pointer wird gefunden!
//DEL 	CTreeView::OnLButtonUp(nFlags, point);	//vom Klassenassistenten!
//DEL }



void CLeftView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CTreeCtrl& folderTree = GetTreeCtrl();
	m_hSelectedItem = folderTree.GetSelectedItem();
	int iSelectedFolderNr = folderTree.GetItemData(m_hSelectedItem);
	CSkyFolders *pSkyFolders = NULL;
	if(m_hSelectedItem != m_rghItem[0])
	{
		if (m_mFolderTb->Lookup(iSelectedFolderNr, (CObject*&)pSkyFolders))
					m_sActivePath = pSkyFolders->m_sFolderPath + pSkyFolders->m_sFolderName;
	}
	GetDocument()->UpdateAllViews(this); //einfach updateAllViews ohne sich slebst zu updaten! Richtiger pointer wird gefunden!
	*pResult = 0;
}

void CLeftView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CTreeCtrl& folderTree = GetTreeCtrl();
	m_hSelectedItem = folderTree.GetSelectedItem();
	int iSelectedFolderNr = folderTree.GetItemData(m_hSelectedItem);
	CSkydanceDoc *pDocument = GetDocument();
	CSkyFolders *pSkyFolders = NULL;
	if(m_hSelectedItem != m_rghItem[0])//Prüfen, ob es sich nicht um den NULL(Host)-tree handelt...
	{
		if (m_mFolderTb->Lookup(iSelectedFolderNr, (CObject*&)pSkyFolders))
		{
			if (pDocument -> m_iNavigateMode == 1)//Stealth Mode?
			{
				m_sActivePath = pSkyFolders->m_sFolderPath + pSkyFolders->m_sFolderName;
				pDocument->UpdateAllViews(this); //einfach updateAllViews ohne sich slebst zu updaten! Richtiger pointer wird gefunden!
			}
			else
			{
				if (pDocument -> m_iNavigateMode == 3)//Discovery Mode?
				{
					if (pSkyFolders->m_bFolderStatus == TRUE)
					{
						m_sActivePath = pSkyFolders->m_sFolderPath + pSkyFolders->m_sFolderName;
						if (pDocument->m_bFolderFile == TRUE) 
								pDocument -> SendPack("dir<" + m_sActivePath + "<1</");
						else	pDocument -> SendPack("dir<" + m_sActivePath + "<0</");
						pSkyFolders->m_bFolderStatus = FALSE;	
					}
					else pDocument->UpdateAllViews(this); //einfach updateAllViews ohne sich slebst zu updaten! Richtiger pointer wird gefunden!
				}
				if (pDocument -> m_iNavigateMode == 2)//Update Mode?
				{
					while (folderTree.GetChildItem(m_hSelectedItem) != 0)
					{
						HTREEITEM subfolder = folderTree.GetChildItem(m_hSelectedItem);
						folderTree.DeleteItem(subfolder);
					}
					m_sActivePath = pSkyFolders->m_sFolderPath + pSkyFolders->m_sFolderName;
					if (pDocument->m_bFolderFile == TRUE) 
							pDocument -> SendPack("dir<" + m_sActivePath + "<1</");
					else	pDocument -> SendPack("dir<" + m_sActivePath + "<0</");
					pSkyFolders->m_bFolderStatus = FALSE;	
				}
			}
		}
	}
	*pResult = 0;
}

void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	CTreeCtrl& folderTree = GetTreeCtrl();
	CSkydanceDoc *pDocument = GetDocument();
	CSkyFolders *pSkyFolders = NULL;
	if(pDocument -> m_iNumberOfFolders > -1)
	{
		for( int nLoopCount=0; nLoopCount<= pDocument->m_iNumberOfFolders; nLoopCount++)
		{
			m_mFolderTb->SetAt(m_iFolderCounter, new CSkyFolders());//DWORD darf nicht < 0 sein!
			if (m_mFolderTb->Lookup(m_iFolderCounter, (CObject*&)pSkyFolders))
			{
				pSkyFolders->m_sFolderName = pDocument -> m_aData[nLoopCount] + "\\";
				pSkyFolders->m_sFolderPath = m_sActivePath;
				pSkyFolders->m_bFolderStatus = TRUE;
				m_rghItem[m_iFolderCounter] = folderTree.InsertItem(_T(pDocument -> m_aData[nLoopCount]), folderTree.GetSelectedItem(), TVI_SORT);
				folderTree.SetItemData(m_rghItem[m_iFolderCounter], m_iFolderCounter);
				m_iFolderCounter++;
			}
		}
		folderTree.Expand(folderTree.GetSelectedItem(), TVE_TOGGLE);	
	}
}

void CLeftView::AddEnvironmentData(CString HDname, CString HDtype, int HDcounter )//Festplatten CD-rom...
{
	//HDs in den Host-Tree füllen
	CTreeCtrl& folderTree = GetTreeCtrl();
	CSkyFolders *pSkyFolders = NULL;
	m_mFolderTb->SetAt(m_iFolderCounter, new CSkyFolders());
	if (m_mFolderTb->Lookup(m_iFolderCounter, (CObject*&)pSkyFolders))
	{
		m_rghItem[HDcounter] = folderTree.InsertItem(_T(HDname), 0, 0, m_rghItem[1], TVI_SORT);
		folderTree.SetItemData(m_rghItem[HDcounter], m_iFolderCounter);
		pSkyFolders->m_sFolderName = HDname;
		//AfxMessageBox(HDname);
		pSkyFolders->m_sFolderPath = "";
		pSkyFolders->m_bFolderStatus = TRUE;
		/////////////und jetzt die Icons einfügen;
		if( HDtype == "DRIVE_UNKNOWN") folderTree.SetItemImage( m_rghItem[HDcounter], 8, 8);
		if( HDtype == "DRIVE_NO_ROOT_DIR") folderTree.SetItemImage( m_rghItem[HDcounter], 8, 8);		
		if( HDtype == "DRIVE_REMOVABLE") folderTree.SetItemImage( m_rghItem[HDcounter], 4, 4);		
		if( HDtype == "DRIVE_FIXED") folderTree.SetItemImage( m_rghItem[HDcounter], 5, 5);		
		if( HDtype == "DRIVE_REMOTE") folderTree.SetItemImage( m_rghItem[HDcounter], 7, 7);		
		if( HDtype == "DRIVE_CDROM") folderTree.SetItemImage( m_rghItem[HDcounter], 6, 6);		
		if( HDtype == "DRIVE_RAMDISK") folderTree.SetItemImage( m_rghItem[HDcounter], 9, 9);		
		if (m_iFolderCounter == 2) folderTree.Expand(m_rghItem[1], TVE_EXPAND);//um zeit zu sparen, nur beim ersten mal!
		m_iEnvironmentMax = m_iFolderCounter;
		m_iFolderCounter++;
	}
}

void CLeftView::CleanView() ///alles neu!
{
	CTreeCtrl& folderTree = GetTreeCtrl();
	folderTree.DeleteAllItems();	//niemals lehren tree löschen!!!
	delete m_rghItem;	// alten baum löschen!
	delete m_mFolderTb;	//und die daten...
	m_mFolderTb = new CMapWordToOb;					//und alles neu
	m_rghItem = (HTREEITEM* ) new HTREEITEM[10000];
	m_iFolderCounter = 1;
	m_sActivePath = "";
	m_hSelectedItem = NULL;
	OnInitialUpdate();
}

int CLeftView::GetSelectedItemData()
{
	CTreeCtrl& folderTree = GetTreeCtrl();
	//folderTree.on
	//HTREEITEM selectedItem = folderTree.GetSelectedItem();		//deaktiviert, m_hSelectedItem von Anfang an NULL!
	if (m_hSelectedItem == NULL) return -1;	//bei Fehler...
	else return folderTree.GetItemData(m_hSelectedItem);
}

void CLeftView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	*pResult = 1;
//	*pResult = 0;
}

void CLeftView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
		if (point.x == -1 && point.y == -1)
		point = (CPoint) GetMessagePos();
	
	CTreeCtrl& folderTree = GetTreeCtrl();
	CSkydanceDoc *pDocument = GetDocument();
	CSkyFolders *pSkyFolders = NULL;
	CMenu menu;
	HTREEITEM htItem;
	UINT uFlags;// = TVHT_ONITEM | TVHT_ONITEMBUTTON | TVHT_ONITEMICON | TVHT_ONITEMSTATEICON;
//
	ScreenToClient(&point);
	htItem = folderTree.HitTest( point, &uFlags);
	if (htItem == NULL || htItem == m_rghItem[0] ||pDocument->m_iConnected == 0) 
	{
		m_bFolderHit = FALSE;
		return;
	}
	else
	{
		int iHtFolderNr = folderTree.GetItemData(htItem);
		if(iHtFolderNr == 1) return;
		if(iHtFolderNr > m_iEnvironmentMax) m_bFolderHit = TRUE;
		else m_bFolderHit = FALSE;
		if (m_mFolderTb->Lookup(iHtFolderNr, (CObject*&)pSkyFolders))
		{
			m_sActiveCPath = pSkyFolders->m_sFolderPath + pSkyFolders->m_sFolderName;;
			//menüpunkte aktvieren?
			//AfxMessageBox(m_sActiveCPath);
		}
	}
	menu.LoadMenu(IDR_MENFOLDERS);		//und das Menü laden!
	CMenu *pContextMenu = menu.GetSubMenu(0);
	ClientToScreen(&point);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y,
												AfxGetMainWnd());
}

void CLeftView::OnUpdateLconCreate(CCmdUI* pCmdUI) 
{
	//if(m_bFolderHit == FALSE) pCmdUI->Enable(FALSE);
	//else pCmdUI->Enable(TRUE);
}

void CLeftView::OnUpdateLconDelete(CCmdUI* pCmdUI) 
{
	if(m_bFolderHit == FALSE) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CLeftView::OnUpdateLconRename(CCmdUI* pCmdUI) 
{
	if(m_bFolderHit == FALSE) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CLeftView::OnUpdateLconCopypath(CCmdUI* pCmdUI) 
{
}										

void CLeftView::OnLconCreate()				////BUG, der name wird in der doc auf etwas enderes gesetzt
{
	CSkydanceDoc *pDocument = GetDocument();
	//pDocument->m_cDlgNewFolder.SetDlgItemText(IDOK, "&Create");
	//pDocument->m_cDlgNewFolder.m_sFolderName = m_sActiveCPath;
	//pDocument->OnBtnewfolder();	
	m_cDlgNewFolder.m_sFolderName = m_sActiveCPath;
	if (m_cDlgNewFolder.DoModal () == IDOK)
	{
		if(m_cDlgNewFolder.m_sFolderName != "")
		{
			pDocument->SendPack("CreateDirectory<" + m_cDlgNewFolder.m_sFolderName + "</");
			pDocument->m_sStat2 = "Trying to Create...";
			pDocument->m_pLeftRecView->AddEvent("Create: " + m_cDlgNewFolder.m_sFolderName, 3);
		}
		else AfxMessageBox("Enter Folder-Name!");
	}
}

void CLeftView::OnLconDelete() 
{
	int iLength = m_sActiveCPath.GetLength();
	CString sFolderName = m_sActiveCPath.Left(iLength-1);
	if(MessageBox("Are you sure you want to delete this Folder?\n\n" + sFolderName , "Skydance - Warning", MB_OKCANCEL + MB_ICONQUESTION) == IDOK)
	{
		CSkydanceDoc *pDocument = GetDocument();
		pDocument->SendPack("RemoveDirectory<" + sFolderName + "</");
		pDocument->m_pLeftRecView->AddEvent("Remove: " + sFolderName, 3);
	}
}

void CLeftView::OnLconRename() 
{
	//AfxMessageBox("Noch nicht reingebaut");
	CSkydanceDoc *pDocument = GetDocument();
	int iLength = m_sActiveCPath.GetLength();
	CString sFolderName = m_sActiveCPath.Left(iLength-1);
	m_cDlgRenameFolder.m_sFolderName = sFolderName;
	if (m_cDlgRenameFolder.DoModal () == IDOK)
	{
		if(m_cDlgRenameFolder.m_sFolderName != "")
		{
			pDocument->SendPack("move<" + m_sActiveCPath + "<"+ m_cDlgRenameFolder.m_sFolderName + "</");
			pDocument->m_sStat2 = "Trying to Rename...";
			pDocument->m_pLeftRecView->AddEvent("Rename: " + m_sActiveCPath + " -> " + m_cDlgRenameFolder.m_sFolderName, 3);
		}
		else AfxMessageBox("Enter Folder-Name!");
	}
}

void CLeftView::OnLconCopypath() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_sCopyPathOfFile = m_sActiveCPath;
}

void CLeftView::OnLconPast() 
{
	CSkydanceDoc *pDocument = GetDocument();
	CSkydanceView *pSkydanceView = pDocument->m_pSkydanceView;
	pSkydanceView->OnConPast();
}

void CLeftView::OnUpdateLconPast(CCmdUI* pCmdUI) 
{
	CSkydanceDoc *pDocument = GetDocument();
	CSkydanceView *pSkydanceView = pDocument->m_pSkydanceView;
	pSkydanceView->OnUpdateConPast(pCmdUI);
}
