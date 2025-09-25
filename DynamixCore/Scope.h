#pragma once

#include <unordered_map>
#include "Value.h"
#include "NoCopyMove.h"

namespace Dynamix {
	enum class ElementFlags : uint16_t {
		None =				0x00,
		Function =			0x01,
		Const =				0x02,
		Static =			0x04,
		NativeFunction =	0x08,
		Class =				0x10,
		Enum =				0x20,
		DefaultClass =		0x40,
		Alias =				0x80,
	};

	struct Element {
		Value VarValue;
		ElementFlags Flags{ ElementFlags::Function };
		int8_t Arity{ -1 };
	};

	struct UseElement {
		std::string Name;
		ElementFlags Type;
	};

	class Scope : public NoCopy {
	public:
		explicit Scope(Scope* parent = nullptr);
		bool AddElement(std::string name, Element var);
		Element* FindElement(std::string const& name, int arity = -1, bool localOnly = false);
		std::vector<Element*> FindElements(std::string const& name, bool localOnly = false);
		Element* FindElementWithUse(std::string const& name);
		bool AddUse(std::string name, ElementFlags type = ElementFlags::DefaultClass);

	private:
		std::unordered_map<std::string, std::vector<Element>> m_Elements;
		std::vector<UseElement> m_Uses;
		Scope* m_Parent;
	};
}