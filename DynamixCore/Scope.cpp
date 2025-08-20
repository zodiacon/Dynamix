#include "Scope.h"

using namespace Dynamix;

Scope::Scope(Scope* parent) : m_Parent(parent) {
}

bool Scope::AddVariable(std::string name, Variable var) {
	return m_Variables.insert({ std::move(name), std::move(var) }).second;
}

Variable* Scope::FindVariable(std::string const& name, bool localOnly) {
	if (auto it = m_Variables.find(name); it != m_Variables.end())
		return &it->second;

	return m_Parent && !localOnly ? m_Parent->FindVariable(name) : nullptr;
}
