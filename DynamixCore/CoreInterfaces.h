#pragma once

#include <memory>

#include "Value.h"

namespace Dynamix {
	enum class ServiceId {
		Invalid,
		Enumerable,
		Clonable,
		Sliceable,
	};

	class SliceObject;

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

	struct IClonable {
		virtual RuntimeObject* Clone() const = 0;
	};

	struct ISliceable {
		virtual SliceObject* Slice(Int start, Int count = -1) = 0;
	};
}