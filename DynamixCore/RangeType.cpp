#include <format>
#include "RangeType.h"
#include "TypeHelper.h"

using namespace Dynamix;
using namespace std;

RangeType::RangeType() : StaticObjectType("Range") {
	BEGIN_METHODS(RangeObject)
		METHOD(Size, 0, return inst->Size();),
		METHOD(Shift, 1, inst->Shift(args[1].ToInteger()); return inst;),
		METHOD(Start, 1, inst->Start(args[1].ToInteger()); return inst;),
		METHOD(Start, 0, return inst->Start();),
		METHOD(End, 1, inst->End(args[1].ToInteger()); return inst;),
		METHOD(End, 0, return inst->End();),
		METHOD(IsInRange, 1, return inst->IsInRange(args[1].ToInteger());),
		CTOR(2),
		END_METHODS()
}

RangeType* RangeType::Get() {
	static RangeType type;
	return &type;
}

RangeObject* RangeType::CreateRange(Int start, Int end) {
	return new RangeObject(start, end);
}

RuntimeObject* RangeType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	return CreateRange(args[0].ToInteger(), args[1].ToInteger());
}

RangeObject::RangeObject(Int start, Int end) : RuntimeObject(RangeType::Get()), m_Start(start), m_End(end) {
}

unique_ptr<IEnumerator> RangeObject::GetEnumerator() const {
	return make_unique<Enumerator>(m_Start, m_End);
}

RuntimeObject* RangeObject::Clone() const {
	return new RangeObject(m_Start, m_End);
}

std::string RangeObject::ToString() const {
	return std::format("{}..{}", m_Start, m_End);
}

RangeObject::Enumerator::Enumerator(Int start, Int end) : m_Current(start), m_End(end) {
}

Value RangeObject::Enumerator::GetNextValue() {
	if (m_Current == m_End)
		return Value::Error(ValueErrorType::CollectionEnd);

	return Value(m_Current++);
}

void* RangeObject::QueryService(ServiceId id) noexcept {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
		case ServiceId::Clonable: return static_cast<IClonable*>(this);
	}
	return nullptr;
}
