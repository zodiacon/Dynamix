#pragma once

#include <unordered_map>
#include <string>
#include <memory>
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
		UnknownOperator,
		ObjectExpected,
		UnknownMember,
		IndexerNotSupported,
		IndexerTypeMismatch,
		IndexOutOfRange,
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
		ObjectType* GetObjectType(AstNode const* classNode);
		ObjectType* GetObjectType(std::string const& classNode) const;

		void AddNativeFunctions();
		bool Init();

	private:
		Parser& m_Parser;
		std::unordered_map<std::string, std::unique_ptr<ObjectType>> m_Types;
	};
}

