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

int Symbol::Arity() const {
	auto n = Name.find('/');
	return n == std::string::npos ? -1 : std::strtol(Name.substr(n + 1).c_str(), nullptr, 10);
}
