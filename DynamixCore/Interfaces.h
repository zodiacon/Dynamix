#pragma once

namespace Dynamix {
	struct Allocator {
		virtual ~Allocator() noexcept = default;
		virtual void* Allocate(size_t bytes) = 0;
		virtual void Free(void* ptr) = 0;
	};
}
