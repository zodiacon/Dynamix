#pragma once

#include <vector>
#include "CoreInterfaces.h"

namespace Dynamix {
	struct VectorEnumerator : IEnumerator {
		explicit VectorEnumerator(std::vector<Value> const& v);

		Value GetNextValue() override;

	private:
		std::vector<Value> const& m_Vec;
		std::vector<Value>::const_iterator m_Iter;
	};
}
