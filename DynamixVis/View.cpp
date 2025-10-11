// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"
#include "View.h"
#include "TreeViewVisitor.h"

using namespace Dynamix;

CView::CView(IMainFrame* frame) : CFrameView(frame), m_Parser(m_Tokenizer) {
}

BOOL CView::PreTranslateMessage(MSG* pMsg) {
	pMsg;
	return FALSE;
}

bool CView::Parse(PCWSTR file) {
	m_Code = m_Parser.ParseFile((PCSTR)CStringA(file));
	if(!m_Code)
		return false;

	return true;
}

void CView::OnFinalMessage(HWND /*hWnd*/) {
	delete this;
}

void CView::BuildTree() {
	TreeViewVisitor visitor(m_Tree, m_Tokenizer);
	visitor.Visit(m_Code.get(), TVI_ROOT);
}

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_hWndClient = m_Tree.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES);
	m_Tree.SetExtendedStyle(TVS_EX_DOUBLEBUFFER | TVS_EX_RICHTOOLTIP, 0);

	BuildTree();

	return 0;
}
