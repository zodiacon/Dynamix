#pragma once

#include <stacktrace>
#include <vector>

namespace Dynamix {
	struct Allocator;
	class Parser;
	class RuntimeObject;
	class ObjectType;
	class Value;

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

		RuntimeObject* CreateObject(ObjectType* type, std::vector<Value>& args);
		void DestroyObject(RuntimeObject* object);

		void AddNativeFunctions();
		bool Init();

	private:
		Allocator* m_Allocator;
		Parser& m_Parser;
	};
}

