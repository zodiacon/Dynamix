#include "VectorEnumerator.h"

using namespace Dynamix;

VectorEnumerator::VectorEnumerator(std::vector<Value> const& v) : m_Vec(v), m_Iter(v.begin()) {
}

Value Dynamix::VectorEnumerator::GetNextValue() {
	return m_Iter == m_Vec.end() ? Value::Error(ValueErrorType::CollectionEnd) : *m_Iter++;
}
