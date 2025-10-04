#pragma once

#include <ObjectType.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <SFML/Graphics.hpp>

class WindowObject;

enum class WindowCreateStyle {
	None = 0,
	TitleBar = sf::Style::Titlebar,
	Close = sf::Style::Close,
	Resize = sf::Style::Resize,
	Default = TitleBar,
};
DEFINE_ENUM_FLAG_OPERATORS(WindowCreateStyle);

class WindowType : public Dynamix::StaticObjectType {
public:
	static WindowType* Get();


private:
	WindowType();
};

class WindowObject : public Dynamix::RuntimeObject {
public:
	WindowObject(int width, int height, const char* title, WindowCreateStyle flags);

	Dynamix::Value Title() const;

	sf::RenderWindow& Window() {
		return m_Window;
	}

private:
	sf::RenderWindow m_Window;
};
