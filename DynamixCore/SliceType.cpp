#include "SliceType.h"
#include "RangeType.h"
#include "Runtime.h"

using namespace Dynamix;

SliceType* SliceType::Get() {
	static SliceType type;
	return &type;
}

SliceObject* SliceType::CreateSlice(RuntimeObject* target, Value const& range) const {
	assert(range.AsObject()->Type() == RangeType::Get());
	auto r = reinterpret_cast<RangeObject const*>(range.ToObject());
	return new SliceObject(target, r->Start(), r->End() < 0 ? -1 : r->End() - r->Start());
}

SliceType::SliceType() : StaticObjectType("Slice") {
}

std::unique_ptr<IEnumerator> SliceObject::GetEnumerator() const {
	return std::make_unique<Enumerator>(this);
}

RuntimeObject* SliceObject::Clone() const {
	return new SliceObject(m_Target, Start(), Size());
}

void* SliceObject::QueryService(ServiceId id) noexcept {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
		case ServiceId::Clonable: return static_cast<IClonable*>(this);
	}
	return nullptr;
}

SliceObject::SliceObject(RuntimeObject* target, Int start, Int size) noexcept
	: RuntimeObject(SliceType::Get()), m_Target(target), m_Start(start), m_Size(size) {
	target->AddRef();
}

SliceObject::~SliceObject() noexcept {
	m_Target->Release();
}

std::string SliceObject::ToString() const {
	std::string text("[ ");
	for (Int i = 0; Size() < 0 ? true : (i < Size()); i++) {
		if (!m_Target->HasValue(i + Start()))
			break;
		text += m_Target->InvokeIndexer(Value(i + Start())).ToString() + ", ";
	}
	return text.substr(0, text.length() - 2) + " ]";
}

Value SliceObject::InvokeIndexer(Value const& index) {
	if (index.ToInteger() < 0 || index.ToInteger() >= Size())
		throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} out of range in slice", index.ToInteger()));

	return m_Target->InvokeIndexer(Value(index.ToInteger() + Start()));
}

void SliceObject::AssignIndexer(Value const& index, Value const& value, TokenType assign) {
	m_Target->AssignIndexer(Value(index.ToInteger() + Start()), value, assign);
}

Value SliceObject::GetByIndex(Int index) const {
	return m_Target->InvokeIndexer(Value(index + Start()));
}

SliceObject::Enumerator::Enumerator(SliceObject const* slice) : m_Slice(slice), m_Current(0), m_End(slice->Size()) {
	slice->AddRef();
}

Value SliceObject::Enumerator::GetNextValue() {
	if (m_Current == m_End)
		return Value::Error(ValueErrorType::CollectionEnd);

	return m_Slice->GetByIndex(m_Current++);
}
