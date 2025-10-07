#include <format>

#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Runtime.h"
#include "Interpreter.h"
#include "Scope.h"
#include "TypeHelper.h"

using namespace Dynamix;

Value ObjectType::Invoke(Interpreter& intr, RuntimeObject* instance, std::string const& name, std::vector<Value>& args, InvokeFlags flags) const {
	auto count = (int8_t)args.size();// -((flags & InvokeFlags::Static) == InvokeFlags::Static ? 0 : 1);
	assert(count >= 0);
	auto method = GetMethod(name, count);
	if (!method && m_Base)
		return m_Base->Invoke(intr, instance, name, args, flags);
	if (!method) {
		method = GetMethod(name, -1);
		if (method && method->Arity != -1 && method->Arity != args.size())
			method = nullptr;
	}
	if (!method)
		throw RuntimeError(RuntimeErrorType::MethodNotFound,
			std::format("Method {} with {} args not found in type {}", name, args.size(), Name()));

	Scoper scope(&intr);
	if ((method->Flags & SymbolFlags::Native) == SymbolFlags::Native) {
		if (instance) {
			args.insert(args.begin(), instance);
		}
		return (*method->Code.Native)(intr, args);
	}
	if (instance && !instance->IsObjectType())
		intr.CurrentScope().AddElement("this", { instance });
	else {
		for (auto& [name, v] : m_FieldValues)
			intr.CurrentScope().AddElement(name, { v });
	}
	for (size_t i = 0; i < method->Parameters.size(); i++)
		intr.CurrentScope().AddElement(method->Parameters[i].Name, { args[i] });
	return intr.Eval(method->Code.Node);
}

Value ObjectType::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) const {
	return Invoke(intr, nullptr, name, args, flags);
}

void ObjectType::RunClassConstructor(Interpreter& intr) {
	if (!m_ClassCtorRun) {
		m_ClassCtorRun = true;
		// init static fields
		for (auto& [name, f] : m_Fields)
			if ((f->Flags & SymbolFlags::Static) == SymbolFlags::Static && f->Init)
				m_FieldValues[name] = intr.Eval(f->Init);

		if (auto it = m_Constructors.find(std::format("class/new")); it != m_Constructors.end()) {
			auto m = it->second.get();
			intr.Eval(m->Code.Node);
		}
	}
}

unsigned ObjectType::GetObjectCount() const noexcept {
	return m_ObjectCount;
}

bool ObjectType::AddField(std::unique_ptr<FieldInfo> field, Value value) {
	if(field->IsStatic()) {
		m_FieldValues.insert({ field->Name(), std::move(value) });
	}
	m_Members.insert({ field->Name(), field.get() });
	return m_Fields.insert({ field->Name(), std::move(field) }).second;
}

bool ObjectType::AddMethod(std::unique_ptr<MethodInfo> method) {
	if ((method->Flags & SymbolFlags::Ctor) == SymbolFlags::Ctor) {
		return m_Constructors.insert({ method->IsStatic() ? "class/new" : std::format("new/{}", method->Arity), move(method) }).second;
	}
	auto clone = std::make_unique<MethodInfo>(method->Name());
	clone->Flags = method->Flags;
	clone->Code = method->Code;
	clone->Arity = method->Arity;

	m_Members.insert({ method->Name(), clone.get() });
	m_Methods.insert({ method->Name(), std::move(clone) });

	if (method->Arity >= 0)
		return m_Methods.insert({ std::format("{}/{}", method->Name(), method->Arity), std::move(method) }).second;
	return false;
}

bool ObjectType::AddType(ObjectPtr<ObjectType> type) {
	m_Members.insert({ type->Name(), type });
	return m_Types.insert({ type->Name(), std::move(type) }).second;
}

FieldInfo const* ObjectType::GetField(std::string const& name) const noexcept {
	if(auto it = m_Fields.find(name); it != m_Fields.end())
		return it->second.get();
	return m_Base ? m_Base->GetField(name) : nullptr;
}

MethodInfo const* ObjectType::GetMethod(std::string const& name, int8_t arity) const noexcept {
	if (arity < 0)
		if (auto it = m_Methods.find(name); it != m_Methods.end() && it->second->Arity == -1)
			return it->second.get();

	if (auto it = m_Methods.find(std::format("{}/{}", name, arity)); it != m_Methods.end())
		return it->second.get();
	return m_Base ? m_Base->GetMethod(name, arity) : nullptr;
}

MethodInfo const* ObjectType::GetClassConstructor() const {
	if (auto it = m_Constructors.find("class/new"); it != m_Constructors.end())
		return it->second.get();
	return nullptr;
}

MemberInfo const* ObjectType::GetMember(std::string const& name) const {
	if (auto it = m_Members.find(name); it != m_Members.end())
		return it->second;
	return m_Base ? m_Base->GetMember(name) : nullptr;
}

void ObjectType::AddTypesToScope(Scope& scope) {
	for (auto& [name, type] : m_Types) {
		Element t{ static_cast<RuntimeObject*>(type.Get()), ElementFlags::Class };
		scope.AddElement(name, std::move(t));
	}
}

Value& ObjectType::GetStaticField(std::string const& name) {
	assert(m_FieldValues.contains(name));
	return m_FieldValues.at(name);
}

void ObjectType::SetStaticField(std::string const& name, Value value) {
	assert(m_FieldValues.contains(name));
	m_FieldValues[name] = std::move(value);
}

void ObjectType::ObjectCreated(RuntimeObject* obj) {
	m_ObjectCount++;
}

void ObjectType::ObjectDestroyed(RuntimeObject* obj) {
	m_ObjectCount--;
}

ObjectType::ObjectType(std::string name, ObjectType* base)
	: RuntimeObject(this), MemberInfo(move(name), MemberType::Class), m_Base(base) {

	BEGIN_METHODS(ObjectType)
		METHOD(ObjectCount, 0, return Value((Int)inst->GetObjectCount());),
	END_METHODS()
}

ObjectType::~ObjectType() noexcept {
	if(RefCount() == 0)
		Runtime::Get()->RevokeType(this);
}

RuntimeObject* ObjectType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	RunClassConstructor(intr);
	auto obj = new RuntimeObject(this);

	// init fields
	MethodInfo* ctor = nullptr;
	for (auto& [name, fi] : m_Fields) {
		obj->AssignField(name, fi->Init ? intr.Eval(fi->Init) : Value());
	}

	if (auto it = m_Constructors.find(std::format("new/{}", args.size())); it != m_Constructors.end())
		ctor = it->second.get();
	else if (!args.empty())
		throw RuntimeError(RuntimeErrorType::NoMatchingConstructor, "No matching constructor");

	// run ctor
	if (ctor) {
		intr.RunConstructor(obj, ctor, args);
	}

	return obj;
}

bool MemberInfo::IsStatic() const {
	return (Flags & SymbolFlags::Static) == SymbolFlags::Static;
}

StaticObjectType::StaticObjectType(std::string name, ObjectType* baseType) : ObjectType(move(name), baseType) {
}
