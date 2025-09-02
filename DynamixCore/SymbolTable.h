#pragma once

#include <string>
#include <unordered_map>
#include "Value.h"

namespace Dynamix {
	class AstNode;

	enum class SymbolType : uint8_t {
		Element,
		Function,
		NativeFunction,
		Argument,
		Enum,
		Class,
		Struct,
	};

	enum class SymbolFlags : uint16_t {
		None = 0,
		Const = 1,
		VarArg = 2,
	};

	class Interpreter;

	struct Symbol {
		std::string Name;
		SymbolType Type;
		SymbolFlags Flags{ SymbolFlags::None };
	};

	class SymbolTable {
		friend class AstNode;
	public:
		explicit SymbolTable(SymbolTable* parent = nullptr);
		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const;
		SymbolTable const* Parent() const {
			return m_Parent;
		}
		std::vector<Symbol const*> EnumSymbols() const;

	protected:
		void SetParent(SymbolTable* parent) {
			m_Parent = parent;
		}

	private:
		std::unordered_map<std::string, Symbol> m_Symbols;
		SymbolTable* m_Parent;
	};
}
