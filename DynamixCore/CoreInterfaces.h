#pragma once

#include <memory>

#include "Value.h"

namespace Dynamix {
	enum class ServiceId {
		Invalid,
		Enumerable,
	};

	struct IServices {
		virtual void* QueryService(ServiceId id) {
			return nullptr;
		}
	};

	struct IEnumerator {
		virtual Value GetNextValue() = 0;
	};

	struct IEnumerable {
		virtual std::unique_ptr<IEnumerator> GetEnumerator() const = 0;
	};
}