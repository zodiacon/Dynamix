#pragma once

#include <string>
#include <unordered_map>
#include <tuple>

namespace Dynamix {
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

		int Arity() const;
		std::string_view SimpleName() const;
	};

	class SymbolTable {
	public:
		explicit SymbolTable(SymbolTable* parent = nullptr);
		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const;

	private:
		std::unordered_map<std::string, Symbol> m_Symbols;
		SymbolTable* m_Parent;
	};
}
