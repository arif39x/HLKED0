/* 
	DataSpyNetworkX - DataSpy2k@Hotmail.com  Copyright (C) 2001 DataSpy
	Release Source 0.4Beta.

	In no event will the author be held liable for any damages arising from the use of this code. 

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

*/
#if !defined(AFX_EDITSERVERDLG_H__1BE2426B_FA80_4433_AC6D_D55433436273__INCLUDED_)
#define AFX_EDITSERVERDLG_H__1BE2426B_FA80_4433_AC6D_D55433436273__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CEditServerDlg dialog

class CEditServerDlg : public CDialog
{
// Construction
public:
	CEditServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CEditServerDlg)
	enum { IDD = IDD_EDITSERVER_DIALOG };
	CString	m_Server;
	CString	m_Port;
	CString	m_Channel;
	CString	m_Password;
	CString	m_Prefix;
	BOOL	m_RTC;
	BOOL	m_PServer;
	CString	m_Hostmask;
	CString	m_Waittime;
	CString	m_Status;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEditServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnChannel();
	afx_msg void OnQuery();
	afx_msg void OnNOTICE();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITSERVERDLG_H__1BE2426B_FA80_4433_AC6D_D55433436273__INCLUDED_)
