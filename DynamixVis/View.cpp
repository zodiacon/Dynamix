// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"
#include "View.h"
#include "TreeViewVisitor.h"
#include <wil/resource.h>

using namespace Dynamix;

CView::CView(IMainFrame* frame) : CFrameView(frame), m_Parser(m_Tokenizer) {
}

BOOL CView::PreTranslateMessage(MSG* pMsg) {
	pMsg;
	return FALSE;
}

bool CView::Parse(PCWSTR file) {
	m_FileName = file;
	
	if (!m_FileName.IsEmpty() && !LoadFile())
		return false;

	return true;
}

void CView::BuildTree() {
	m_Tree.DeleteAllItems();
	m_Tree.SetRedraw(FALSE);
	TreeViewVisitor visitor(m_Tree, m_Tokenizer);
	visitor.Visit(m_Code.get(), TVI_ROOT);
	m_Tree.SetRedraw();
}

bool CView::LoadFile() {
	wil::unique_hfile hFile(::CreateFile(m_FileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr));
	if (!hFile)
		return false;

	std::string text;
	text.resize(::GetFileSize(hFile.get(), nullptr));
	DWORD read;
	if (!::ReadFile(hFile.get(), text.data(), (DWORD)text.length(), &read, nullptr))
		return false;

	m_Edit.SetWindowTextW(CString(text.c_str()));

	m_Code = m_Parser.Parse(text);
	if (!m_Code) {
		DisplayErrors();
	}
	else {
		BuildTree();
	}
	m_Edit.SetFocus();

	return true;
}

void CView::DisplayErrors() {
	m_Tree.DeleteAllItems();
	for (auto& err : m_Parser.Errors()) {
		m_Tree.InsertItem(CString(std::format("({:2}:{:2}): {}", err.Location().Line, err.Location().Col, err.Description()).c_str()), TVI_ROOT, TVI_LAST);
	}
}

LRESULT CView::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	m_hWndClient = m_Splitter.Create(m_hWnd, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	m_Edit.Create(m_Splitter, rcDefault, L"", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_WANTRETURN | WS_HSCROLL | WS_VSCROLL);
	CFont font;
	font.CreatePointFont(110, L"Consolas");
	m_Edit.SetFont(font.Detach());
	m_Edit.SetTabStops(10);
	m_Tree.Create(m_Splitter, rcDefault, nullptr, WS_CHILD | WS_VISIBLE | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_LINESATROOT | 0*TVS_HASLINES);
	m_Tree.SetExtendedStyle(TVS_EX_DOUBLEBUFFER | TVS_EX_RICHTOOLTIP, 0);

	m_Splitter.SetSplitterPanes(m_Edit, m_Tree);
	m_Splitter.SetSplitterPosPct(50);

	return 0;
}
