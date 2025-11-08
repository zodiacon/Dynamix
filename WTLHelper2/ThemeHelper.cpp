#include "pch.h"
#include "ThemeHelper.h"
#include "DarkModeSubclass.h"
HHOOK g_hHook;

void HandleCreateWindow(CWPRETSTRUCT* cs) {
	auto hWnd = cs->hwnd;
	CString name;
	auto lpcs = (LPCREATESTRUCT)cs->lParam;
	if (!::GetClassName(hWnd, name.GetBufferSetLength(32), 32))
		return;

	DarkMode::setColorizeTitleBarConfig(true);
	DarkMode::setDarkWndNotifySafe(hWnd);
	DarkMode::setWindowEraseBgSubclass(hWnd);
	DarkMode::setWindowMenuBarSubclass(hWnd);
	DarkMode::setWindowExStyle(hWnd, true, WS_EX_COMPOSITED);
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
	//if (auto hook = ::SetWindowsHookEx(WH_CALLWNDPROCRET, CallWndProc, nullptr, ::GetCurrentThreadId()); !hook)
	//	return false;
	//else
	//	g_hHook = hook;

	return true;
}

void ThemeHelper::Term() noexcept {
	if (g_hHook)
		::UnhookWindowsHookEx(g_hHook);
}