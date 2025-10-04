#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <unordered_set>

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
		DuplicateDefinition,
		StackOverflow,
		Unexpected,
		TooManyArguments,
		InvalidType,
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

	class Runtime : NoCopy {
	public:
		Runtime();
		void InitStdLibrary();

		ObjectPtr<ObjectType> BuildType(ClassDeclaration const* decl, Interpreter* intr) const;
		ObjectPtr<ObjectType> BuildEnum(EnumDeclaration const* decl) const;

		Scope* GetGlobalScope() {
			return &m_GlobalScope;
		}

		Runtime& AddCode(std::unique_ptr<Statements> code) noexcept;
		Runtime& AddCode(std::vector<std::unique_ptr<Statements>> code) noexcept;
		void ClearCode();

		std::vector<std::unique_ptr<Statements>> const& Code() const noexcept {
			return m_Code;
		}

		std::vector<ObjectType*> GetTypes();

		void RegisterType(ObjectType* type);
		void RevokeType(ObjectType* type);

		static Runtime* Get();

	private:
		std::vector<std::unique_ptr<Statements>> m_Code;
		inline static thread_local Runtime* s_Runtime;
		Scope m_GlobalScope;
		std::unordered_set<ObjectType*> m_Types;
	};
}

