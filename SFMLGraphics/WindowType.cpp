#include "WindowType.h"
#include <TypeHelper.h>
#include <StringType.h>

using namespace Dynamix;

WindowType::WindowType() : StaticObjectType("Window") {
	BEGIN_METHODS(WindowObject)
		CTOR(3),
		METHOD(Title, 0, return inst->Title();),
		METHOD(Title, 1, inst->Window().setTitle(args[1].ToString()); return inst;),
		END_METHODS()
}

WindowType* WindowType::Get() {
	static WindowType type;
	return &type;
}

RuntimeObject* WindowType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	return new WindowObject(static_cast<int>(args[0].ToInteger()), static_cast<int>(args[1].ToInteger()), args[2].ToString().c_str(), WindowCreateStyle::Default);
}

WindowObject::WindowObject(int width, int height, const char* title, WindowCreateStyle style) : RuntimeObject(WindowType::Get()) {
	sf::VideoMode mode(sf::Vector2u(width, height));
	m_Window.create(mode, sf::String(title), static_cast<int>(style));
	m_Window.setFramerateLimit(60);
}

Value WindowObject::Title() const {
	auto hWnd = static_cast<HWND>(m_Window.getNativeHandle());
	auto len = ::GetWindowTextLength(hWnd);
	if (len == 0)
		return Value();

	std::string title;
	title.resize(len);
	::GetWindowTextA(hWnd, title.data(), len);
	return new StringObjectA(std::move(title));
}
