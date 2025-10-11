#pragma once

class ThemeHelper {
public:
	static bool Init() noexcept;
	static bool SetDarkMode(bool dark) noexcept;
	static bool IsDarkMode() noexcept;
	static void Term() noexcept;
};

