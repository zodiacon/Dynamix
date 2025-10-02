#include "RuntimeObject.h"
#include "ArrayType.h"
#include "Runtime.h"
#include "VectorEnumerator.h"
#include "TypeHelper.h"
#include "SliceType.h"
#include "RangeType.h"

using namespace Dynamix;

ArrayType* ArrayType::Get() {
	static ArrayType type;
	return &type;
}

ArrayObject* ArrayType::CreateArray(std::vector<Value> args) {
	return new ArrayObject(std::move(args));
}

Value ArrayObject::InvokeIndexer(Value const& index) {
	if (index.IsObject() && index.ToObject()->Type() == RangeType::Get()) {
		// slicing
		return SliceType::Get()->CreateSlice(this, index);
	}
	auto i = ValidateIndex(index.ToInteger());
	return m_Items[i];
}

void ArrayObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	if (!index.IsInteger())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Array index must be an integer");
	auto i = ValidateIndex(index.ToInteger());
	m_Items[i].Assign(value, assign);
}

void ArrayObject::Reverse() {
	std::reverse(m_Items.begin(), m_Items.end());
}

Int ArrayObject::ValidateIndex(Int i) const {
	if (i < 0 || i >= (Int)m_Items.size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range (array size: {})", i, m_Items.size()));
	return i;
}

SliceObject* ArrayObject::Slice(Int start, Int count) {
	return new SliceObject(this, start, count);
}

ArrayType::ArrayType() : StaticObjectType("Array") {
	BEGIN_METHODS(ArrayObject)
		METHOD(Count, 0, return inst->Count();),
		METHOD(IsEmpty, 0, return inst->IsEmpty();),
		METHOD(Clear, 0, inst->Clear();	return inst;),
		METHOD(Clone, 0, return GetInstance<ArrayObject>(args[0])->Clone();),
		METHOD(Reverse, 0, inst->Reverse();	return inst;),
		METHOD(Add, 1, inst->Add(args[1]); return inst;),
		METHOD(Append, 1, inst->Append(args[1]); return inst;),
		METHOD(RemoveAt, 1, return inst->RemoveAt(args[1].ToInteger());),
		METHOD(Resize, 1, inst->Resize(args[1].ToInteger()); return inst;),
		METHOD(Slice, 2, return inst->Slice(args[1].ToInteger(), args[2].ToInteger());),
		ENUMERABLE_METHODS
	END_METHODS()
}

ArrayObject::ArrayObject(std::vector<Value> init) :
	RuntimeObject(ArrayType::Get()), m_Items(std::move(init)) {
}

std::unique_ptr<IEnumerator> ArrayObject::GetEnumerator() const {
	return std::make_unique<VectorEnumerator<decltype(m_Items)::const_iterator>>(m_Items.begin(), m_Items.end());
}

RuntimeObject* ArrayObject::Clone() const {
	return new ArrayObject(m_Items);
}

void* ArrayObject::QueryService(ServiceId id) {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
		case ServiceId::Clonable: return static_cast<IClonable*>(this);
		case ServiceId::Sliceable: return static_cast<ISliceable*>(this);
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
	if (obj->Type()->Name() != Type()->Name())
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
