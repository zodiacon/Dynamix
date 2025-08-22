#pragma once

#include <stacktrace>

namespace Dynamix {
	struct Allocator;
	class Parser;

	enum class RuntimeErrorType {
		CannotConvertToBoolean,
		CannotConvertToReal,
		UnknownIdentifier,
	};

	struct RuntimeError {
		RuntimeError(RuntimeErrorType type, std::stacktrace trace = std::stacktrace::current());
	};

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

