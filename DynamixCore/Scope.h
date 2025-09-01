#pragma once

#include "Value.h"
#include <unordered_map>

namespace Dynamix {
	enum class VariableFlags {
		None = 0,
		Const = 1,
		Static = 2,
		NativeFunction = 4,
		Class = 8,
	};

	struct Variable {
		Value VarValue;
		VariableFlags Flags{ VariableFlags::None };
	};

	class Scope {
	public:
		explicit Scope(Scope* parent = nullptr);
		bool AddVariable(std::string name, Variable var);
		Variable* FindVariable(std::string const& name, bool localOnly = false);

	private:
		std::unordered_map<std::string, Variable> m_Variables;
		Scope* m_Parent;
	};
}