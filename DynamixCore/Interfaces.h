#pragma once

namespace Dynamix {
	struct Allocator {
		virtual ~Allocator() noexcept = default;
		virtual void* Allocate(size_t bytes) = 0;
		virtual void Free(void* ptr) = 0;
	};

	struct Library {
		virtual ~Library() noexcept = default;
		virtual bool Init(Runtime& rt) = 0;
	};
}
