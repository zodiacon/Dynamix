#include "SymbolTable.h"

using namespace Dynamix;
using namespace std;

SymbolTable::SymbolTable(SymbolTable* parent) : m_Parent(parent) {
}

bool SymbolTable::AddSymbol(Symbol sym) {
	return m_Symbols.insert({ sym.Name, move(sym) }).second;
}

Symbol const* SymbolTable::FindSymbol(string const& name, int8_t arity, bool localOnly) const {
	if (auto it = m_Symbols.find(name); it != m_Symbols.end() && it->second.Arity == arity)
		return &(it->second);

	if (!localOnly && m_Parent)
		return m_Parent->FindSymbol(name, arity, localOnly);
	return nullptr;
}

