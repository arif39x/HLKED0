// RShellSend.cpp : implementation file
//

#include "stdafx.h"
#include "client.h"
#include "RShellSend.h"
#include "RShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CRShellSend

CRShellSend::CRShellSend()
{
}

CRShellSend::~CRShellSend()
{
}


BEGIN_MESSAGE_MAP(CRShellSend, CEdit)
	//{{AFX_MSG_MAP(CRShellSend)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRShellSend message handlers

void CRShellSend::OnKillfocus() 
{
	// TODO: Add your control notification handler code here
	
}

void CRShellSend::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{ /* nChar */
		case VK_RETURN:
			GetParent()->SendMessage(UWM_EDIT_COMPLETE, GetDlgCtrlID(), (LPARAM)this);
			return;
	} /* nChar */	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

UINT CRShellSend::OnGetDlgCode() 
{
	// TODO: Add your message handler code here and/or call default
	
	return CEdit::OnGetDlgCode() | DLGC_WANTALLKEYS;
}
