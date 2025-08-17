#pragma once

namespace Dynamix {
	struct Allocator;

	class Runtime {
	public:
		Runtime();

		bool SetAllocator(Allocator* allocator);
		Allocator* GetAllocator() const;

	private:
		Allocator* m_Allocator;
	};
}

