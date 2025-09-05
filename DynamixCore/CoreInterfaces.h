#pragma once

#include <memory>

#include "Value.h"

namespace Dynamix {
	struct IEnumerator {
		virtual Value GetNextValue() = 0;
	};

	struct IEnumerable {
		virtual std::unique_ptr<IEnumerator> GetEnumerator() const = 0;
	};
}