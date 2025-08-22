#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Interfaces.h"
#include "Runtime.h"

using namespace Dynamix;

ObjectType::ObjectType(Runtime& rt, std::string name) : m_Runtime(rt), m_Name(std::move(name)) {
}

RuntimeObject* ObjectType::CreateObject(std::vector<Value>& args) {
	m_ObjectCount++;
	auto obj = (RuntimeObject*)m_Runtime.GetAllocator()->Allocate(sizeof(RuntimeObject));
	new (obj) RuntimeObject(*this);
	obj->Construct(args);
	return obj;
}

Value ObjectType::Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) {
	return Value();
}

Value ObjectType::Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) {
	return Invoke(intr, nullptr, name, args, flags);
}

void ObjectType::DestroyObject(RuntimeObject* instance) {
	instance->Destruct();
	instance->~RuntimeObject();
	m_Runtime.GetAllocator()->Free(instance);
	--m_ObjectCount;
}

unsigned Dynamix::ObjectType::GetObjectCount() const {
	return m_ObjectCount.load();
}
