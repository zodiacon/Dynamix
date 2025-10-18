// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Interfaces.h"
#include <AstNode.h>
#include <FrameView.h>
#include <Parser.h>
#include <Tokenizer.h>

class CView : public CFrameView<CView, IMainFrame> {
public:
	explicit CView(IMainFrame* frame);

	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);
	bool Parse(PCWSTR file);

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_DYNAMIX_PARSE, OnParse)
		COMMAND_ID_HANDLER(ID_DYNAMIX_SYNC, OnSync)
		CHAIN_MSG_MAP(BaseFrame)
	END_MSG_MAP()

private:
	void BuildTree();
	bool LoadFile();
	void DisplayErrors();

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSetFocus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnParse(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSync(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
	CTreeViewCtrl m_Tree;
	CSplitterWindow m_Splitter;
	CEdit m_Edit;
	std::unique_ptr<Dynamix::Statements> m_Code;
	Dynamix::Tokenizer m_Tokenizer;
	Dynamix::Parser m_Parser;
	CString m_FileName;
};
