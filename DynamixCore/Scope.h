#pragma once

#include <unordered_map>
#include "Value.h"
#include "NoCopyMove.h"

namespace Dynamix {
	enum class ElementFlags : uint16_t {
		None =					0x0000,
		Function =				0x0000,
		Const =					0x0002,
		Static =				0x0004,
		NativeFunction =		0x0008,
		Class =					0x0010,
		Enum =					0x0020,
		DefaultClass =			0x0040,
		Alias =					0x0080,
		AnnonymousFunction =	0x0100,
	};

	struct Element {
		Value VarValue;
		ElementFlags Flags{ ElementFlags::None };
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
		Scope Clone() const;

	private:
		std::unordered_map<std::string, std::vector<Element>> m_Elements;
		std::vector<UseElement> m_Uses;
		Scope* m_Parent;
	};
}