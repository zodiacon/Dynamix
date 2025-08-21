#pragma once

#include <string>
#include <unordered_map>
#include "Value.h"

namespace Dynamix {
	class AstNode;

	enum class SymbolType {
		Variable,
		Function,
		NativeFunction,
		Argument,
		Enum,
		Class,
		Struct,
	};

	enum class SymbolFlags {
		None = 0,
		Const = 1,
	};

	class Interpreter;

	using NativeFunction = Value(*)(Interpreter&, std::vector<Value>&);

	struct Symbol {
		std::string Name;
		SymbolType Type;
		SymbolFlags Flags{ SymbolFlags::None };
		union {
			AstNode* Ast;
			NativeFunction Code;
		};
		int Arity() const;
		std::string_view SimpleName() const;
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

	protected:
		void SetParent(SymbolTable* parent) {
			m_Parent = parent;
		}

	private:
		std::unordered_map<std::string, Symbol> m_Symbols;
		SymbolTable* m_Parent;
	};
}
