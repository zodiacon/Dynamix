#include "ArrayType.h"
#include "RuntimeObject.h"
#include "Runtime.h"
#include "VectorEnumerator.h"

using namespace Dynamix;

ArrayType& ArrayType::Get() {
	static ArrayType type;
	return type;
}

ArrayObject* ArrayType::CreateArray(std::vector<Value>& args) {
	return new ArrayObject(args);
}

Value ArrayObject::InvokeIndexer(Value const& index) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = ValidateIndex(index.ToInteger());
	return m_Items[i];
}

void ArrayObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = ValidateIndex(index.ToInteger());
	m_Items[i].Assign(value, assign);
}

Int ArrayObject::ValidateIndex(Int i) const {
	if (i < 0 || i >= (Int)m_Items.size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range (array size: {})", i, m_Items.size()));
	return i;
}

ArrayType::ArrayType() : ObjectType("Array") {
	struct {
		const char* Name;
		int Arity;
		NativeFunction Code;
		SymbolFlags Flags{ SymbolFlags::Native };
	} methods[] = {
		{ "Count", 0, 
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				return GetInstance<ArrayObject>(args[0])->Count();
				} },
		{ "IsEmpty", 0,
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				return GetInstance<ArrayObject>(args[0])->IsEmpty();
				} },
		{ "Clear", 0,
			[](auto, auto& args) -> Value {
				assert(args.size() == 1);
				GetInstance<ArrayObject>(args[0])->Clear();
				return Value();
				} },
		{ "Add", 1,
			[](auto, auto& args) -> Value {
				assert(args.size() == 2);
				return GetInstance<ArrayObject>(args[0])->Add(args[1]);
			} },

		{ "Append", 1,
			[](auto, auto& args) -> Value {
				assert(args.size() == 2);
				return GetInstance<ArrayObject>(args[0])->Append(args[1]);
			} },

		{ "RemoveAt", 1,
			[](auto, auto& args) -> Value {
			assert(args.size() == 2);
			return GetInstance<ArrayObject>(args[0])->RemoveAt(args[1].ToInteger());
			} },
	};

	for (auto& m : methods) {
		auto mi = std::make_unique<MethodInfo>(m.Name);
		mi->Arity = m.Arity;
		mi->Code.Native = m.Code;
		mi->Flags = m.Flags;
		AddMethod(move(mi));
	}
}

ArrayObject::ArrayObject(std::vector<Value> init) : 
	RuntimeObject(ArrayType::Get()), m_Items(std::move(init)) {
}

std::unique_ptr<IEnumerator> ArrayObject::GetEnumerator() const {
	return std::make_unique<VectorEnumerator>(m_Items);
}

void* ArrayObject::QueryService(ServiceId id) {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
	}
	return nullptr;
}

Int ArrayObject::Add(Value item) {
	m_Items.push_back(std::move(item));
	return Int(m_Items.size());
}

Int ArrayObject::RemoveAt(Int index) {
	ValidateIndex(index);
	m_Items.erase(m_Items.begin() + index);
	return Int(m_Items.size());
}

Int ArrayObject::Insert(Int index, Value item) {
	ValidateIndex(index);
	m_Items.insert(m_Items.begin() + index, std::move(item));
	return Int(m_Items.size());
}

Int ArrayObject::Append(Value list) {
	auto obj = list.ToObject();
	if (obj->Type().Name() != Type().Name())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Cannot append non-array to array");
	auto other = ObjectType::GetInstance<ArrayObject>(obj);
	m_Items.insert(m_Items.end(), other->m_Items.begin(), other->m_Items.end());
	return Int(m_Items.size());
}

std::string ArrayObject::ToString() const {
	std::string text("[ ");
	for (auto& item : m_Items)
		text += item.ToString() + ", ";
	if (!m_Items.empty())
		text = text.substr(0, text.length() - 2);
	return text + " ]";
}
