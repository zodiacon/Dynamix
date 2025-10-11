// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"
#include "View.h"
#include "TreeViewVisitor.h"

using namespace Dynamix;

CView::CView(IMainFrame* frame, std::unique_ptr<Statements> code) : CFrameView(frame), m_Code(std::move(code)) {
}

BOOL CView::PreTranslateMessage(MSG* pMsg) {
	pMsg;
	return FALSE;
}

void CView::OnFinalMessage(HWND /*hWnd*/) {
	delete this;
}

void CView::BuildTree() {
	auto hRoot = m_Tree.InsertItem(L"(Root)", TVI_ROOT, TVI_LAST);
	TreeViewVisitor visitor(m_Tree);
	visitor.Visit(m_Code.get(), hRoot);
	m_Tree.Expand(hRoot, TVE_EXPAND);
}

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_hWndClient = m_Tree.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES);
	m_Tree.SetExtendedStyle(TVS_EX_DOUBLEBUFFER | TVS_EX_RICHTOOLTIP, 0);

	BuildTree();

	return 0;
}
