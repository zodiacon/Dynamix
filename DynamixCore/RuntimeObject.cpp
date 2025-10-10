#include "RuntimeObject.h"
#include "ObjectType.h"
#include "Value.h"
#include <format>
#include "Interpreter.h"

using namespace Dynamix;

#ifdef _WIN32
#include <Windows.h>

static HANDLE s_hHeap = ::HeapCreate(HEAP_NO_SERIALIZE, 1 << 18, 0);
void* RuntimeObject::operator new(size_t size) {
	return ::HeapAlloc(s_hHeap, 0, size);
}

void RuntimeObject::operator delete(void* p) {
	::HeapFree(s_hHeap, 0, p);
}

#endif

RuntimeObject::RuntimeObject(ObjectType* type) : m_Type(type) {
	if (type) {
		type->ObjectCreated(this);
	}
}

RuntimeObject::~RuntimeObject() noexcept {
	if (m_Type) {
		m_Type->ObjectDestroyed(this);
	}
}

void RuntimeObject::Construct(std::vector<Value> const& args) {
}

void RuntimeObject::Destruct() {
}

void RuntimeObject::AssignField(std::string const& name, Value value, TokenType assignType) {
	m_FieldValues[name] = m_FieldValues[name].Assign(std::move(value), assignType);
}

bool RuntimeObject::HasField(std::string const& name) const noexcept {
	return Type()->GetField(name) != nullptr;
}


Value RuntimeObject::GetFieldValue(std::string const& name) const {
	if (auto it = m_FieldValues.find(name); it != m_FieldValues.end())
		return it->second;

	if (Type()->GetField(name))
		return Value();

	throw RuntimeError(RuntimeErrorType::UnknownMember, std::format("Member '{}' not found on type '{}", name, Type()->Name()));
}

Value RuntimeObject::Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) {
	return (flags & InvokeFlags::Static) == InvokeFlags::Static ? m_Type->Invoke(intr, name, args, flags) : m_Type->Invoke(intr, this, name, args, flags);
}

Value RuntimeObject::InvokeOperator(Interpreter& intr, TokenType op, Value const& rhs) const {
	throw RuntimeError(RuntimeErrorType::OperatorNotImplemented,
		format("Operator {} not implemented on type '{}'", Token::TypeToString(op), Type()->Name()));
}

Value Dynamix::RuntimeObject::InvokeOperator(Interpreter& intr, TokenType op) const {
	throw RuntimeError(RuntimeErrorType::OperatorNotImplemented,
		format("Operator {} not implemented on type '{}'", Token::TypeToString(op), Type()->Name()));
}

Value RuntimeObject::InvokeGetIndexer(Value const& index) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, format("Indexer not supported on {}", Type()->Name()));
}

void RuntimeObject::InvokeSetIndexer(Value const& index, Value const& value, TokenType assign) {
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, std::format("indexer not supported on type {}", Type()->Name()));
}

int RuntimeObject::AddRef() const noexcept {
	return ++m_RefCount;
}

int RuntimeObject::Release() const noexcept {
	auto count = --m_RefCount;
	if (count == 0) {
		delete this;
	}
	return count;
}

std::string RuntimeObject::ToString() const {
	return std::format("Object ({})", Type()->Name());
}
