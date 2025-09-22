#include "RuntimeObject.h"
#include "ObjectType.h"
#include "Value.h"
#include <format>
#include "Interpreter.h"

using namespace Dynamix;

RuntimeObject::RuntimeObject(ObjectType* type) : m_Type(type) {
	m_Type->ObjectCreated(this);
}

Dynamix::RuntimeObject::~RuntimeObject() noexcept {
	m_Type->ObjectDestroyed(this);
}

void RuntimeObject::Construct(std::vector<Value> const& args) {
}

void RuntimeObject::Destruct() {
}

void RuntimeObject::AssignField(std::string const& name, Value value, TokenType assignType) {
	m_FieldValues[name] = m_FieldValues[name].Assign(std::move(value), assignType);
}

Value RuntimeObject::GetField(std::string const& name) const {
	return m_FieldValues.at(name);
}

Value RuntimeObject::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) {
	return m_Type->Invoke(intr, this, name, args, flags);
}

Value RuntimeObject::InvokeIndexer(Value const& index) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, format("Indexer not supported on {}", Type()->Name()));
}

void RuntimeObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, std::format("indexer not supported on type {}", Type()->Name()));
}

int RuntimeObject::AddRef() const noexcept {
	return ++m_RefCount;
}

int RuntimeObject::Release() const noexcept {
	auto count = --m_RefCount;
	if (count == 0) {
		m_Type->DestroyObject(this);
		delete this;
	}
	return count;
}

std::string RuntimeObject::ToString() const {
	return std::format("Object ({})", Type()->Name());
}
