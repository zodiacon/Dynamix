#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include "AstNode.h"
#include "Scope.h"

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
		WrongNumberArguments,
		MultipleSymbols,
		NoMatchingConstructor,
		Syntax,
		MethodNotFound,
		InvalidMemberAccess,
	};

	struct ReturnStatementException {
		Value ReturnValue;
	};

	struct BreakStatementException {
	};

	struct BreakoutStatementException {
	};

	struct ContinueStatementException {
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
		[[maybe_unused]] ObjectType* GetObjectType(AstNode const* classNode, Interpreter* intr = nullptr);
		ObjectType* GetObjectType(std::string const& name);

		Scope* GetGlobalScope() {
			return &m_GlobalScope;
		}

		void InitStdLibrary();
	private:
		Parser& m_Parser;
		Scope m_GlobalScope;
		std::unordered_map<std::string, std::unique_ptr<ObjectType>> m_Types;
	};
}

