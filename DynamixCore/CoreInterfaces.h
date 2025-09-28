#pragma once

#include <memory>

#include "Value.h"

namespace Dynamix {
	class RuntimeObject;

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

	struct Enumerable : public IEnumerable {
		virtual Value Filter(Interpreter& intr, Value predciate);
		virtual Value Any(Interpreter& intr, Value predciate);
	};

	struct IClonable {
		virtual RuntimeObject* Clone() const = 0;
	};

	struct ISliceable {
		virtual SliceObject const* Slice(Int start, Int count = -1) const = 0;
	};
}

