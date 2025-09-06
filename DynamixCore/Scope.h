#pragma once

#include "Value.h"
#include <unordered_map>

namespace Dynamix {
	enum class ElementFlags : uint8_t {
		None = 0,
		Const = 1,
		Static = 2,
		NativeFunction = 4,
		Class = 8,
		Enum = 0x10,
	};

	struct Element {
		Value VarValue;
		ElementFlags Flags{ ElementFlags::None };
		int8_t Arity{ -1 };
	};

	class Scope {
	public:
		explicit Scope(Scope* parent = nullptr);
		bool AddElement(std::string name, Element var);
		Element* FindElement(std::string const& name, int arity = -1, bool localOnly = false);
		std::vector<Element*> FindElements(std::string const& name, bool localOnly = false);

	private:
		std::unordered_map<std::string, std::vector<Element>> m_Elements;
		Scope* m_Parent;
	};
}