#include "Runtime.h"
#include "Interfaces.h"
#include <cstdlib>

namespace Dynamix {
	struct DefaultAllocator : Allocator {
		void* Allocate(size_t size) {
			return std::malloc(size);
		}

		void Free(void* p) {
			std::free(p);
		}
	};

	DefaultAllocator g_DefaultAllocator;

	Runtime::Runtime() {
		m_Allocator = &g_DefaultAllocator;
	}
	Allocator* Runtime::GetAllocator() const {
		return m_Allocator;
	}
}
