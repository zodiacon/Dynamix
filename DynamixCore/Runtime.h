#pragma once

#include <stacktrace>
#include <vector>
#include "AstNode.h"

namespace Dynamix {
	struct Allocator;
	class Parser;
	class RuntimeObject;
	class ObjectType;
	class Value;
	
	enum class RuntimeErrorType {
		CannotConvertToBoolean,
		CannotConvertToReal,
		CannotConvertToInteger,
		UnknownIdentifier,
		TypeMismatch,
		DivisionByZero,
		NonCallable,
	};

	struct ReturnStatementException {
		Value ReturnValue;
	};

	struct BreakStatementException {
	};

	struct ContinueStatementException {
	};

	struct BreakAllStatementException {
	};

	struct RuntimeError {
		RuntimeError(RuntimeErrorType type, std::string msg, CodeLocation location = CodeLocation());
		RuntimeErrorType Type() const {
			return m_Type;
		}
		std::string const& Message() const {
			return m_Message;
		}
		CodeLocation const& Location() const {
			return m_Location;
		}

	private:
		RuntimeErrorType m_Type;
		std::string m_Message;
		CodeLocation m_Location;
	};

	class Runtime {
	public:
		Runtime(Parser& parser);

		void AddNativeFunctions();
		bool Init();

	private:
		Allocator* m_Allocator;
		Parser& m_Parser;
	};
}

