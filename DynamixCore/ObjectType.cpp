#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Runtime.h"
#include "Interpreter.h"
#include <format>

using namespace Dynamix;

Value ObjectType::Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Value();
}

Value ObjectType::Invoke(Interpreter& intr, Value& instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Value();
}

Value ObjectType::Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Invoke(intr, nullptr, name, args, flags);
}

unsigned ObjectType::GetObjectCount() const {
	return m_ObjectCount;
}

bool ObjectType::AddField(std::unique_ptr<FieldInfo> field) {
	return m_Fields.insert({ field->Name(), std::move(field) }).second;
}

bool ObjectType::AddMethod(std::unique_ptr<MethodInfo> method) {
	m_Methods.insert({ method->Name(), std::make_unique<MethodInfo>(method->Name()) });

	if(method->Arity >= 0)
		return m_Methods.insert({method->Arity < 0 ? method->Name() : std::format("{}/{}", method->Name(), method->Arity), std::move(method) }).second;
	return false;
}

FieldInfo const* ObjectType::GetField(std::string const& name) const {
	auto it = m_Fields.find(name);
	return it == m_Fields.end() ? nullptr : it->second.get();
}

MethodInfo const* ObjectType::GetMethod(std::string const& name, int8_t arity) const {
	if (arity < 0)
		if (auto it = m_Methods.find(name); it != m_Methods.end())
			return it->second.get();

	if (auto it = m_Methods.find(std::format("{}/{}", name, arity)); it != m_Methods.end())
		return it->second.get();
	return nullptr;
}

void ObjectType::DestroyObject(RuntimeObject* instance) {
	instance->Destruct();
	delete instance;
}

RuntimeObject* ObjectType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	auto obj = new RuntimeObject(*this);

	// init fields
	MethodInfo* ctor = nullptr;
	bool defCtor = true;
	for (auto& [name, fi] : m_Fields) {
		obj->AssignField(name, fi->Init ? intr.Eval(fi->Init) : Value());
	}

	for (auto& [pair, m] : m_Methods) {
		if (!ctor && (m->Flags & MemberFlags::Ctor) == MemberFlags::Ctor) {
			if (m.get()->Parameters.size() == args.size())
				ctor = m.get();
			else
				defCtor = false;

		}
	}

	// run ctor
	if (ctor) {
		intr.RunConstructor(obj, ctor, args);
	}
	else if (!defCtor)
		throw RuntimeError(RuntimeErrorType::NoMatchingConstructor, "No matching constructor");

	return obj;
}
