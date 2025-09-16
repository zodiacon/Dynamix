#include "RangeType.h"

using namespace Dynamix;
using namespace std;

RangeType* RangeType::Get() {
	static RangeType type;
	return &type;
}

RangeObject* Dynamix::RangeType::CreateRange(Int start, Int end) {
	return new RangeObject(start, end);
}

RangeObject::RangeObject(Int start, Int end) : RuntimeObject(RangeType::Get()), m_Start(start), m_End(end) {
}

unique_ptr<IEnumerator> RangeObject::GetEnumerator() const {
	return make_unique<Enumerator>(m_Start, m_End);
}

RangeObject::Enumerator::Enumerator(Int start, Int end) : m_Current(start), m_End(end) {
}

Value RangeObject::Enumerator::GetNextValue() {
	if (m_Current == m_End)
		return Value::Error(ValueErrorType::CollectionEnd);

	return Value(m_Current++);
}

void* RangeObject::QueryService(ServiceId id) {
	switch (id) {
		case ServiceId::Enumerable: return static_cast<IEnumerable*>(this);
	}
	return nullptr;
}
