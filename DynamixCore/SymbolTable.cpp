#include "SymbolTable.h"

using namespace Dynamix;
using namespace std;

SymbolTable::SymbolTable(SymbolTable* parent) : m_Parent(parent) {
}

bool SymbolTable::AddSymbol(Symbol sym) {
	return m_Symbols.insert({ sym.Name, move(sym) }).second;
}

Symbol const* SymbolTable::FindSymbol(string const& name, bool localOnly) const {
	if (auto it = m_Symbols.find(name); it != m_Symbols.end())
		return &(it->second);

	if (!localOnly && m_Parent)
		return m_Parent->FindSymbol(name, localOnly);
	return nullptr;
}

std::vector<Symbol const*> SymbolTable::EnumSymbols() const {
	std::vector<Symbol const*> symbols;
	symbols.reserve(m_Symbols.size());
	for (auto& [name, sym] : m_Symbols)
		symbols.push_back(&sym);
	return symbols;
}

