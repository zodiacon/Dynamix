#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Runtime.h"
#include "Interpreter.h"

using namespace Dynamix;

Value ObjectType::Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Value();
}

Value Dynamix::ObjectType::Invoke(Interpreter& intr, Value& instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Value();
}

Value ObjectType::Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const {
	return Invoke(intr, nullptr, name, args, flags);
}

unsigned ObjectType::GetObjectCount() const {
	return m_ObjectCount.load();
}

bool ObjectType::AddMember(std::unique_ptr<MemberInfo> member) {
	return m_Members.insert({ member->Name(), std::move(member) }).second;
}

MemberInfo const* Dynamix::ObjectType::GetMember(std::string const& name) const {
	auto it = m_Members.find(name);
	return it == m_Members.end() ? nullptr : it->second.get();
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
	for (auto& [name, m] : m_Members) {
		if (m->Type() == MemberType::Field) {
			auto fi = reinterpret_cast<FieldInfo*>(m.get());
			obj->AssignField(name, fi->Init ? intr.Eval(fi->Init) : Value());
		}
		else if (!ctor && (m->Flags & MemberFlags::Ctor) == MemberFlags::Ctor) {
			if (((MethodInfo*)m.get())->Parameters.size() == args.size())
				ctor = (MethodInfo*)m.get();
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
