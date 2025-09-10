#include "RuntimeObject.h"
#include "ObjectType.h"
#include "Value.h"
#include <format>
#include "Interpreter.h"

using namespace Dynamix;

RuntimeObject::RuntimeObject(ObjectType& type) : m_Type(type) {
}

void RuntimeObject::Construct(std::vector<Value> const& args) {
}

void RuntimeObject::Destruct() {
}

void RuntimeObject::AssignField(std::string const& name, Value value, TokenType assignType) {
	m_Fields[name] = m_Fields[name].Assign(std::move(value), assignType);
}

Value RuntimeObject::GetField(std::string const& name) const {
	return m_Fields.at(name);
}

Value RuntimeObject::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) {
	auto method = m_Type.GetMethod(name, (int8_t)args.size());
	if (!method)
		throw RuntimeError(RuntimeErrorType::MethodNotFound,
			std::format("Method {} with {} args not found in type {}", name, args.size(), Type().Name()));

	bool instance = (method->Flags & SymbolFlags::Static) == SymbolFlags::None;
	if ((method->Flags & SymbolFlags::Native) == SymbolFlags::Native) {
		args.insert(args.begin(), this);
		return (*method->Code.Native)(intr, args);
	}
	if (instance)
		intr.CurrentScope()->AddElement("this", { this });
	for(size_t i = 0; i < method->Parameters.size(); i++)
		intr.CurrentScope()->AddElement(method->Parameters[i].Name, { args[i] });
	return intr.Eval(method->Code.Node);
}

Value RuntimeObject::InvokeIndexer(Value const& index) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, format("Indexer not supported on {}", Type().Name()));
}

void RuntimeObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, std::format("indexer not supported on type {}", Type().Name()));
}

int RuntimeObject::AddRef() {
	return ++m_RefCount;
}

int RuntimeObject::Release() {
	auto count = --m_RefCount;
	if (count == 0)
		m_Type.DestroyObject(this);
	return count;
}

std::string RuntimeObject::ToString() const {
	return std::format("Object ({})", Type().Name());
}
