#pragma once

#include <vector>
#include "CoreInterfaces.h"

namespace Dynamix {
	template<typename It>
	struct VectorEnumerator : IEnumerator {
		VectorEnumerator(It begin, It end) : m_Iter(begin), m_End(end) {}

		Value GetNextValue() {
			return m_Iter == m_End ? Value::Error(ValueErrorType::CollectionEnd) : *m_Iter++;
		}

	private:
		It m_Iter, m_End;
	};
}
