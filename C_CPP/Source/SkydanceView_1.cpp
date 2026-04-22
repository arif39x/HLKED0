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
// SkydanceView.cpp : Implementierung der Klasse CSkydanceView
//

#include "stdafx.h"
#include "Skydance.h"

#include "SkydanceDoc.h"
#include "SkydanceView.h"
#include "SkyFiles.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkydanceView

IMPLEMENT_DYNCREATE(CSkydanceView, CListView)

BEGIN_MESSAGE_MAP(CSkydanceView, CListView)
	//{{AFX_MSG_MAP(CSkydanceView)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CON_COPY, OnConCopy)
	ON_COMMAND(ID_CON_COPYPATH, OnConCopypath)
	ON_COMMAND(ID_CON_DELETE, OnConDelete)
	ON_COMMAND(ID_CON_DOWNLOAD, OnConDownload)
	ON_COMMAND(ID_CON_EXECUTE_WITHOUTPARAMETER, OnConExecuteWithoutparameter)
	ON_COMMAND(ID_CON_EXECUTE_WITHPARAMETERS, OnConExecuteWithparameters)
	ON_UPDATE_COMMAND_UI(ID_CON_COPY, OnUpdateConCopy)
	ON_UPDATE_COMMAND_UI(ID_CON_COPYPATH, OnUpdateConCopypath)
	ON_UPDATE_COMMAND_UI(ID_CON_DELETE, OnUpdateConDelete)
	ON_UPDATE_COMMAND_UI(ID_CON_DOWNLOAD, OnUpdateConDownload)
	ON_UPDATE_COMMAND_UI(ID_CON_EXECUTE_WITHOUTPARAMETER, OnUpdateConExecuteWithoutparameter)
	ON_UPDATE_COMMAND_UI(ID_CON_EXECUTE_WITHPARAMETERS, OnUpdateConExecuteWithparameters)
	ON_COMMAND(ID_CON_EXECUTE_WITHCOPY, OnConExecuteWithcopy)
	ON_UPDATE_COMMAND_UI(ID_CON_EXECUTE_WITHCOPY, OnUpdateConExecuteWithcopy)
	ON_COMMAND(ID_CON_MOVE, OnConMove)
	ON_UPDATE_COMMAND_UI(ID_CON_MOVE, OnUpdateConMove)
	ON_COMMAND(ID_CON_COPYTOBUFFER, OnConCopytobuffer)
	ON_UPDATE_COMMAND_UI(ID_CON_COPYTOBUFFER, OnUpdateConCopytobuffer)
	ON_COMMAND(ID_CON_MOVETOBUFF, OnConMovetobuff)
	ON_UPDATE_COMMAND_UI(ID_CON_MOVETOBUFF, OnUpdateConMovetobuff)
	ON_COMMAND(ID_CON_PAST, OnConPast)
	ON_UPDATE_COMMAND_UI(ID_CON_PAST, OnUpdateConPast)
	ON_COMMAND(ID_CON_UPLOAD, OnConUpload)
	ON_UPDATE_COMMAND_UI(ID_CON_UPLOAD, OnUpdateConUpload)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkydanceView Konstruktion/Destruktion

CSkydanceView::CSkydanceView()
{
	// ZU ERLEDIGEN: Hier Code zur Konstruktion einfügen,
	m_mFileTb = new CMapWordToOb;
	m_iFilesSelected = 0;
	//m_sFirstSelectedFullName = "";
	m_sFirstSelectedName = "";
	m_iFirstSelectedFile = 0;		//wirklich wichtig? und auch richtig?
	m_iFirstSelectedTypeNr = 0;
	m_pSkyFiles = NULL;
	m_bCopyPath = FALSE;
	m_saSelectedFiles.Add("");
	m_saSelectedFileNames.Add("");
	m_bMoveBuff = FALSE;
	m_bCopyBuff = FALSE;
}

CSkydanceView::~CSkydanceView()
{
}

CSkydanceView * CSkydanceView::GetView()
{
      CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

      CView * pView = pFrame->GetActiveView();

      if ( !pView )
         return NULL;

      // Fail if view is of wrong kind
      // (this could occur with splitter windows, or additional
      // views on a single document
      if ( ! pView->IsKindOf( RUNTIME_CLASS(CSkydanceView) ) )
         return NULL;

      return (CSkydanceView *) pView;
} 

BOOL CSkydanceView::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSkydanceView Zeichnen

void CSkydanceView::OnDraw(CDC* pDC)
{
	CSkydanceDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
	// ZU ERLEDIGEN: Hier Code zum Zeichnen der ursprünglichen Daten hinzufügen
}

void CSkydanceView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();
	
	CListCtrl &listCtrl = GetListCtrl();
	listCtrl.InsertColumn(0, "Name", LVCFMT_LEFT, 220);
	listCtrl.InsertColumn(1, "Type", LVCFMT_CENTER, 150);
	//listCtrl.InsertColumn(2, "Size", LVCFMT_CENTER, 250);
	
	m_bmpSmallIcons.Create(IDB_BMPSMFILES, 16, 0, RGB(246, 232, 255));
	listCtrl.SetImageList(&m_bmpSmallIcons, LVSIL_SMALL);//IDB_BMPSMFILES
	
	// ZU ERLEDIGEN: Sie können Elemente in Ihre ListView eintragen, indem Sie direkt 
	//  über einen Aufruf von GetListCtrl() auf ihre Listensteuerung zugreifen.
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_pSkydanceView = this;
	ModifyStyle(LVS_TYPEMASK, LVS_LIST);
}

/////////////////////////////////////////////////////////////////////////////
// CSkydanceView Diagnose

#ifdef _DEBUG
void CSkydanceView::AssertValid() const
{
	CListView::AssertValid();
}

void CSkydanceView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSkydanceDoc* CSkydanceView::GetDocument() // Die endgültige (nicht zur Fehlersuche kompilierte) Version ist Inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSkydanceDoc)));
	return (CSkydanceDoc*)m_pDocument;
}
#endif //_DEBUG

CLeftView* CSkydanceView::GetLeftView()
{
	return (CLeftView*)(GetDocument()->m_pLeftView);
}
/////////////////////////////////////////////////////////////////////////////
// CSkydanceView Nachrichten-Handler
void CSkydanceView::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	//ZU ERLEDIGEN: Code hinzufügen, der auf die Änderung des Benutzers des Ansichtsformats Ihres Fensters reagiert
}

void CSkydanceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	m_iSelectedFolderNr = GetLeftView()->GetSelectedItemData();
	int iFileTypeImg = 0;
	CListCtrl &listCtrl = GetListCtrl();
	CSkydanceDoc *pDocument = GetDocument();
	CSkyFiles *pSkyFiles = NULL;
	m_pSkyFiles = NULL;
	if (m_iSelectedFolderNr < 0)
	{
		//AfxMessageBox("Error, no Item Selected");
		return;
	}
	listCtrl.DeleteAllItems();//View frei machen...
////////////////Folders
	for( int nLoopCount=0; nLoopCount<= GetDocument() -> m_iNumberOfFolders; nLoopCount++)
	{
		//GetListCtrl().InsertItem(0, GetDocument()->m_aData[nLoopCount], nLoopCount);
	}
////////////////Files(übertragene, neue Files)
	if(GetDocument() -> m_iNumberOfFiles > -1)
	{
		m_mFileTb->SetAt(m_iSelectedFolderNr, new CSkyFiles());//DWORD darf nicht < 0 sein!
		if (m_mFileTb->Lookup(m_iSelectedFolderNr, (CObject*&)pSkyFiles))
		{
			m_pSkyFiles = pSkyFiles;		//pointer auf die active map!
			for(nLoopCount=0; nLoopCount<= pDocument -> m_iNumberOfFiles; nLoopCount++)
			{
				//die FileTypeName Funktion wird leider zwei mal ausgeführt; in der CSkFolder und hier!
				iFileTypeImg = FileTypeNr(pDocument->m_aData[nLoopCount]);
				pSkyFiles->AddFile(pDocument->m_aData[nLoopCount]);
				listCtrl.InsertItem(nLoopCount, pDocument->m_aData[nLoopCount], iFileTypeImg);			
				listCtrl.SetItemText(nLoopCount, 1, FileTypeName(pDocument->m_aData[nLoopCount]));
				listCtrl.SetItemData(nLoopCount, (LPARAM) nLoopCount);
			}
		}
	}
	else//bereits geladene Items einfügen	
	{
		if (m_mFileTb->Lookup(m_iSelectedFolderNr, (CObject*&)pSkyFiles))
		{
			m_pSkyFiles = pSkyFiles;		//pointer auf die active map!
			for( int iFindFFcounter = 0; iFindFFcounter <= pSkyFiles->m_iFileCounter; iFindFFcounter++)
			{	
				iFileTypeImg = FileTypeNr(pSkyFiles->m_sFileName[iFindFFcounter]);
				listCtrl.InsertItem(iFindFFcounter, pSkyFiles->m_sFileName[iFindFFcounter], iFileTypeImg);
				listCtrl.SetItemText(iFindFFcounter, 1, FileTypeName(pSkyFiles->m_sFileName[iFindFFcounter]));
				listCtrl.SetItemData(iFindFFcounter, (LPARAM) iFindFFcounter);
			}
		}
	}
	//SortFileNames();
}

//////////////////////////////////////////////////////////////SortFunction
/////////////////////////////////////////////////////////////////////////
void CSkydanceView::SortFileNames()
{
	CListCtrl &listCtrl = GetListCtrl();
	//m_pSkyFiles->m_iActiveColumn = 0;
	//listCtrl.SortItems( (PFNLVCOMPARE)CompareName, (LPARAM)m_pSkyFiles);
}

void CSkydanceView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Code für die Behandlungsroutine der Steuerelement-Benachrichtigung hier einfügen
	CListCtrl &listCtrl = GetListCtrl();
	m_pSkyFiles->m_iActiveColumn = pNMListView->iSubItem;
	listCtrl.SortItems( (PFNLVCOMPARE)CompareName, (LPARAM)m_pSkyFiles);
	
	*pResult = 0;
}
/////////////////////////Sortieren
/////////////////////////////////////////////////////////
int CSkydanceView::FileTypeNr(CString fileName)
{
	CString sType;
	int typeBreak = fileName.Find(".");
	if (typeBreak != -1)
	{
		int strLeng = fileName.GetLength();
		sType = fileName.Right(strLeng - typeBreak - 1);
		sType.MakeUpper();
		//AfxMessageBox(sType);
		if(sType == "VBS") return 1;
		if(sType == "INI") return 2;
		if(sType == "TXT") return 3;
		if(sType == "EXE") return 4;
		if(sType == "COM") return 4;
		if(sType == "BAT") return 5;
		if(sType == "JS")  return 6;
		if(sType == "BMP" || sType == "GIF" || sType == "JPG" || sType == "PCX") return 7;
		if(sType == "MOV" || sType == "AVI" || sType == "ANI") return 8;
		if(sType == "WAV" || sType == "MP3" || sType == "MID" || sType == "RMI" || sType == "AU") return 9;

		//if(sType == "BMP") return 5;//jpg//js
		else return 0;
	}
	return 0;
}

CString CSkydanceView::FileTypeName(CString fileName)
{
	CString sType;
	int typeBreak = fileName.Find(".");
	if (typeBreak != -1)
	{
		int strLeng = fileName.GetLength();
		sType = fileName.Right(strLeng - typeBreak - 1);
		sType.MakeUpper();
		return sType;
	}
	else return "";
}


void CSkydanceView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Code für die Behandlungsroutine für Nachrichten hier einfügen
	m_saSelectedFiles.RemoveAll();
	m_saSelectedFileNames.RemoveAll();
	//m_saSelectedFiles = new CStringArray;
	CListCtrl &listCtrl = GetListCtrl();
	CSkydanceDoc *pDocument = GetDocument();
	CMenu menu;
	int iSelectedFile;
	CString sSelectedName;
	POSITION pos = listCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL || pDocument->m_iConnected == 0) m_iFilesSelected = 0;	//case: no file selected... 0
	else
	{
		m_iFilesSelected = listCtrl.GetSelectedCount();	//get number of selected files...
		m_iFirstSelectedFile = listCtrl.GetNextSelectedItem(pos);
		m_sFirstSelectedName = m_pSkyFiles->m_sFileName[listCtrl.GetItemData(m_iFirstSelectedFile)];	//first selected in array...
		m_iFirstSelectedTypeNr = FileTypeNr(m_sFirstSelectedName);	//what kind of file is the first selected
		m_saSelectedFiles.Add(GetLeftView()->m_sActivePath + m_sFirstSelectedName);
		m_saSelectedFileNames.Add(m_sFirstSelectedName);
		//m_sFirstSelectedFullName= GetLeftView()->m_sActivePath + m_sFirstSelectedName;
		while (pos)
		{
			iSelectedFile = listCtrl.GetNextSelectedItem(pos);
			sSelectedName = m_pSkyFiles->m_sFileName[listCtrl.GetItemData(iSelectedFile)];
			m_saSelectedFiles.Add(GetLeftView()->m_sActivePath + sSelectedName);
			m_saSelectedFileNames.Add(sSelectedName);
		}
			//m_iFilesSelected
		//m_sFileName
		//das ItemData enthält die Position in der FileMap
	}
	menu.LoadMenu(IDR_MENFILES);		//und das Menü laden!
	CMenu *pContextMenu = menu.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y,
												AfxGetMainWnd());
}

////////////////////////////////////////////////
////////////////////ContextMenu-Bearbeiten

void CSkydanceView::OnConCopy() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgCopy.m_sCurrent = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->m_cDlgCopy.m_sSource = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->m_cDlgCopy.m_sTarget = GetLeftView()->m_sActivePath;//"";
	pDocument->OnBtcopy();
}

void CSkydanceView::OnConMove() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgMove.m_sCurrent = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->m_cDlgMove.m_sSource = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->m_cDlgMove.m_sTarget = GetLeftView()->m_sActivePath;//"";
	pDocument->OnBtmove();
}

void CSkydanceView::OnConCopypath() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_sCopyPathOfFile = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	m_bCopyPath = TRUE;
}

void CSkydanceView::OnConDelete() 
{
	CSkydanceDoc *pDocument = GetDocument();
	if(m_iFilesSelected > 1)	//case: more then one file are selected... send delete-command several times
	{
		if(MessageBox("Are you sure you want to delete all selected Files?\n", "Skydance - Warning", MB_OKCANCEL + MB_ICONQUESTION) == IDOK)
		{
			int iLength = m_saSelectedFiles.GetSize();
			for(int i = 0; i < iLength; i++)
			{
				pDocument->SendPack("DELETE<" + m_saSelectedFiles[i] + "</");
				pDocument->m_pLeftRecView->AddEvent("Delete: " + m_saSelectedFiles[i], 3);
			}
		}
	}
	else
	{
		if(MessageBox("Are you sure you want to delete this File?\n\n" + m_saSelectedFiles[0], "Skydance - Warning", MB_OKCANCEL + MB_ICONQUESTION) == IDOK)
		{
			pDocument->SendPack("DELETE<" + m_saSelectedFiles[0] + "</");
			pDocument->m_pLeftRecView->AddEvent("Delete: " + m_saSelectedFiles[0] , 3);
		}
	}
	

//	CSkydanceDoc *pDocument = GetDocument();
//	pDocument->m_cDlgDelete.m_sDeleteName = m_sFirstSelectedFullName;
//	pDocument->OnBtdelete();	
}

void CSkydanceView::OnConDownload() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgFtp.m_sFileNameS = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->m_cDlgFtp.m_sFileNameC = "C:\\Skd2\\" + m_sFirstSelectedName;
	pDocument->m_cDlgFtp.m_bUpDown = FALSE;
	pDocument->m_cDlgFtp.m_iDirection = FALSE;
	pDocument->OnBtftp();
}

void CSkydanceView::OnConExecuteWithoutparameter() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->SendPack("run<" + m_saSelectedFiles[0] + "</");
	pDocument->m_sStat2 = "Sending Command...";
}

void CSkydanceView::OnConExecuteWithparameters() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgRun.m_sRunCommand = m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->OnBtrun();
}

void CSkydanceView::OnConExecuteWithcopy() 
{
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgRun.m_sRunCommand = pDocument->m_sCopyPathOfFile + " " + m_saSelectedFiles[0];//m_sFirstSelectedFullName;
	pDocument->OnBtrun();
}

void CSkydanceView::OnConCopytobuffer() 
{
	m_saSelectedFilesBuff.RemoveAll();
	m_saSelectedFileNamesBuff.RemoveAll();
	m_saSelectedFilesBuff.Append(m_saSelectedFiles);
	m_saSelectedFileNamesBuff.Append(m_saSelectedFileNames);
	m_bCopyBuff = TRUE;
	m_bMoveBuff = FALSE;
}

void CSkydanceView::OnConMovetobuff() 
{
	m_saSelectedFilesBuff.RemoveAll();
	m_saSelectedFileNamesBuff.RemoveAll();
	m_saSelectedFilesBuff.Append(m_saSelectedFiles);
	m_saSelectedFileNamesBuff.Append(m_saSelectedFileNames);
	m_bCopyBuff = FALSE;
	m_bMoveBuff = TRUE;
}	

void CSkydanceView::OnConPast() 
{
	CSkydanceDoc *pDocument = GetDocument();
	if(m_bCopyBuff)
	{
		int iLength = m_saSelectedFilesBuff.GetSize();
		for(int i = 0; i < iLength; i++)
		{
			pDocument->SendPack("copy<" + m_saSelectedFilesBuff[i] + "<" + GetLeftView()->m_sActivePath +
									m_saSelectedFileNamesBuff[i] +"</");
			pDocument->m_pLeftRecView->AddEvent("copy: " + m_saSelectedFilesBuff[i] + " -> " + 
									GetLeftView()->m_sActivePath + m_saSelectedFileNamesBuff[i], 3);
		}
		m_bCopyBuff = FALSE;
	}
	if(m_bMoveBuff)
	{
		int iLength = m_saSelectedFilesBuff.GetSize();
		for(int i = 0; i < iLength; i++)
		{
			pDocument->SendPack("move<" + m_saSelectedFilesBuff[i] + "<" + GetLeftView()->m_sActivePath +
									m_saSelectedFileNamesBuff[i] +"</");
			pDocument->m_pLeftRecView->AddEvent("move: " + m_saSelectedFilesBuff[i] + " -> " + 
									GetLeftView()->m_sActivePath + m_saSelectedFileNamesBuff[i], 3);
		}
		m_bMoveBuff = FALSE;
	}
}

void CSkydanceView::OnConUpload() 
{	//	neue Datei hochladen...
	CSkydanceDoc *pDocument = GetDocument();
	pDocument->m_cDlgFtp.m_sFileNameS = GetLeftView()->m_sActivePath;//m_sFirstSelectedFullName;
	pDocument->m_cDlgFtp.m_bUpDown = TRUE;	//UPLOAD
	pDocument->m_cDlgFtp.m_iDirection = FALSE;
	pDocument->OnBtftp();
}

////////////////////////////////////////////////////////////////////////////
/////updateUI
void CSkydanceView::OnUpdateConMove(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CSkydanceView::OnUpdateConCopy(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConCopypath(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConDelete(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConDownload(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConExecuteWithoutparameter(CCmdUI* pCmdUI) 
{			//Achtung, entgegen dem voherigen wird hier logischerweise geprüft, ob der Menüpunt aktiv sein soll!
	if(m_iFilesSelected > 0 && (m_iFirstSelectedTypeNr == 4 || m_iFirstSelectedTypeNr == 5))
				pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CSkydanceView::OnUpdateConExecuteWithparameters(CCmdUI* pCmdUI) 
{			//Achtung, entgegen dem voherigen wird hier logischerweise geprüft, ob der Menüpunt aktiv sein soll!
	if(m_iFilesSelected > 0 && m_iFirstSelectedTypeNr == 4) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);	
}

void CSkydanceView::OnUpdateConExecuteWithcopy(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected > 0 && m_bCopyPath == TRUE)pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);	
}

void CSkydanceView::OnUpdateConCopytobuffer(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConMovetobuff(CCmdUI* pCmdUI) 
{
	if(m_iFilesSelected == 0) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CSkydanceView::OnUpdateConPast(CCmdUI* pCmdUI) 
{
	if(m_iSelectedFolderNr == 1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	if( m_bCopyBuff == TRUE || m_bMoveBuff == TRUE) pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}

void CSkydanceView::OnUpdateConUpload(CCmdUI* pCmdUI) 
{
	if(m_iSelectedFolderNr == 1) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}




void CSkydanceView::CleanView()
{
	CListCtrl &listCtrl = GetListCtrl();
	m_saSelectedFiles.RemoveAll();
	m_saSelectedFileNames.RemoveAll();
	listCtrl.DeleteAllItems();
}
