#include "Scope.h"

using namespace Dynamix;

Scope::Scope(Scope* parent) : m_Parent(parent) {
}

bool Scope::AddElement(std::string name, Element var) {
	if (auto it = m_Elements.find(name); it != m_Elements.end())
		it->second.push_back(std::move(var));
	else
		return m_Elements.insert({ std::move(name), std::vector { std::move(var)} }).second;
	return true;
}

Element* Scope::FindElement(std::string const& name, int arity, bool localOnly) {
	Element* e = nullptr;
	if (auto it = m_Elements.find(name); it != m_Elements.end()) {
		if (it->second.empty())
			return nullptr;
		if (arity < 0 || it->second[0].Arity < 0)
			return &it->second[0];
		for (auto& v : it->second)
			if (v.Arity == arity)
				return &v;
		return nullptr;
	}

	return m_Parent && !localOnly ? m_Parent->FindElement(name, arity) : nullptr;
}

std::vector<Element*> Scope::FindElements(std::string const& name, bool localOnly) {
	std::vector<Element*> v;
	if (auto it = m_Elements.find(name); it != m_Elements.end()) {
		for (auto& e : it->second)
			v.push_back(&e);
		return v;
	}

	return m_Parent && !localOnly ? m_Parent->FindElements(name) : v;
}
