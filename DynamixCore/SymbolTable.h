#pragma once

#include <string>
#include <unordered_map>

namespace Dynamix {
	class AstNode;

	enum class SymbolType {
		Variable,
		Function,
		Argument,
		Enum,
		Class,
		Struct,
	};

	enum class SymbolFlags {
		None = 0,
		Const = 1,
	};

	struct Symbol {
		std::string Name;
		SymbolType Type;
		SymbolFlags Flags;
		AstNode* Ast;

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
