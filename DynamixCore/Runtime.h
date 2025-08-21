#pragma once

namespace Dynamix {
	struct Allocator;
	class Parser;

	class Runtime {
	public:
		Runtime(Parser& parser);

		bool SetAllocator(Allocator* allocator);
		Allocator* GetAllocator() const;

		void AddNativeFunctions();
		bool Init();

	private:
		Allocator* m_Allocator;
		Parser& m_Parser;
	};
}

