// client_gui_consoleView.h : interface of the CClient_gui_consoleView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CClient_gui_consoleView : public CWindowImpl<CClient_gui_consoleView, CListBox>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListBox::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CClient_gui_consoleView)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
