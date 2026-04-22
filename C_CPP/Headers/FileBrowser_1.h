#if !defined(AFX_FILEBROWSER_H__0F453B70_50EC_4D61_9511_54D007516557__INCLUDED_)
#define AFX_FILEBROWSER_H__0F453B70_50EC_4D61_9511_54D007516557__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileBrowser.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileBrowser dialog

class CFileBrowser : public CDialog
{
// Construction
public:
	CFileBrowser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFileBrowser)
	enum { IDD = IDD_FBDLG };
	CButton	m_MkdirButton;
	CButton	m_DownButton;
	CButton	m_DelButton;
	CButton	m_UpButton;
	CFileList	m_FileList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileBrowser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	char	CurrentDir[MAX_PATH];
	typedef enum
	{
		DEL,
		DOWNLOAD,
		UPLOAD,
		MAKEDIR,
		DELDIR
	} f_action;


	// Generated message map functions
	//{{AFX_MSG(CFileBrowser)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRefreshb();
	afx_msg void OnDblclkFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelbut();
	afx_msg void OnDownbut();
	afx_msg void OnUpbut();
	afx_msg void OnMkdirbut();
	//}}AFX_MSG
	afx_msg LRESULT CFileBrowser::OnFileMessage(UINT wParam, LONG lParam);
	afx_msg LRESULT CFileBrowser::OnLostConnection(UINT wParam, LONG lParam);
	afx_msg void CFileBrowser::FileAction(f_action action);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILEBROWSER_H__0F453B70_50EC_4D61_9511_54D007516557__INCLUDED_)
