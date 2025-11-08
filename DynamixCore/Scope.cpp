#include "Scope.h"
#include "ObjectType.h"
#include <ranges>

using namespace Dynamix;
using namespace std::ranges;

Scope::Scope(Scope* parent) : m_Parent(parent) {
}

bool Scope::AddElement(std::string name, Element var) {
	if (auto it = find_if(m_Elements, [&](auto& e) { return e.first == name; }); it != m_Elements.end())
		it->second.emplace_back(std::move(var));
	else
		m_Elements.push_back({ std::move(name), std::vector { std::move(var) } });

	//if (auto it = m_Elements.find(name); it != m_Elements.end())
	//	it->second.emplace_back(std::move(var));
	//else
	//	return m_Elements.insert({ std::move(name), std::vector { std::move(var)} }).second;
	return true;
}

Element* Scope::FindElement(std::string const& name, int arity, bool localOnly) {
	if (auto it = find_if(m_Elements, [&](auto& e) { return e.first == name; }); it != m_Elements.end()) {
		//if (auto it = m_Elements.find(name); it != m_Elements.end()) {
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

std::vector<Element*> Scope::FindElements(std::string const& name, bool localOnly, bool withUse) {
	if (auto it = find_if(m_Elements, [&](auto& e) { return e.first == name; }); it != m_Elements.end()) {
//	if (auto it = m_Elements.find(name); it != m_Elements.end()) {
		std::vector<Element*> v;
		v.reserve(it->second.size());
		for (auto& e : it->second)
			v.push_back(&e);
		if (withUse) {
			auto element = FindElementWithUse(name);
			if (element)
				v.push_back(element);
		}

		return v;
	}

	return m_Parent && !localOnly ? m_Parent->FindElements(name, false, withUse) : std::vector<Element*>();
}

Element* Scope::FindElementWithUse(std::string const& name) {
	for (auto const& use : m_Uses) {
		auto cls = FindElement(use.Name);
		if (cls) {
			auto type = reinterpret_cast<ObjectType const*>(cls->VarValue.ToObject());
			if (type->GetMember(name))
				return cls;
		}
	}
	return m_Parent ? m_Parent->FindElementWithUse(name) : nullptr;
}

bool Scope::AddUse(std::string name, ElementFlags type) {
	m_Uses.push_back(UseElement{ std::move(name), type });
	return true;
}
