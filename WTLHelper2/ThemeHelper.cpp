#include "pch.h"
#include "ThemeHelper.h"

HHOOK g_hHook;

void HandleCreateWindow(CWPRETSTRUCT* cs) {
	CString name;
	CWindow win(cs->hwnd);
	auto lpcs = (LPCREATESTRUCT)cs->lParam;
	if (!::GetClassName(cs->hwnd, name.GetBufferSetLength(32), 32))
		return;

	if (name.CompareNoCase(WC_COMBOBOX) != 0) {
		if ((lpcs->style & (WS_THICKFRAME | WS_CAPTION | WS_POPUP | WS_DLGFRAME)) == 0)
			::SetWindowTheme(cs->hwnd, L" ", L"");
	}
}

LRESULT CALLBACK CallWndProc(int action, WPARAM wp, LPARAM lp) {
	if (action == HC_ACTION) {
		auto cs = reinterpret_cast<CWPRETSTRUCT*>(lp);

		switch (cs->message) {
			case WM_CREATE:
				HandleCreateWindow(cs);
				break;

		}
	}

	return ::CallNextHookEx(nullptr, action, wp, lp);
}

bool ThemeHelper::Init() noexcept {
	if (auto hook = ::SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndProc, nullptr, ::GetCurrentThreadId()); !hook)
		return false;
	else
		g_hHook = hook;

	return true;
}

void ThemeHelper::Term() noexcept {
	if (g_hHook)
		::UnhookWindowsHookEx(g_hHook);
}