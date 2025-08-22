#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Interfaces.h"
#include "Runtime.h"

using namespace Dynamix;

ObjectType::ObjectType(std::string name) : m_Name(std::move(name)) {
}

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
