#pragma once

#include <string>
#include <format>
#include <sstream>
#include <vector>
#include <cassert>

#include "Token.h"
#include "EnumClassBitwise.h"
#include "ObjectPtr.h"

namespace Dynamix {
	class RuntimeObject;
	class AstNode;
	class NativeStruct;
	class ObjectType;
	class Interpreter;
	enum class InvokeFlags;

	using Real = double;
	using Int = long long;
	using Bool = bool;

	enum class ValueType : uint16_t {
		Error = 0,
		Null = 1,
		Integer = 2,
		Real = 4,
		Boolean = 8,
		Object = 0x10,
		AstNode = 0x20,
		Struct = 0x40,
		String = 0x80,
		NativeFunction = 0x100,
		Callable = 0x200,
	};

	enum class ValueErrorType : uint8_t {
		None = 0,
		Unspecfied = 1,
		OutOfMemory,
		DivideByZero,
		UnsupportedBinaryOperator,
		UnsupportedUnaryOperator,
		TypeMismatch,
		DuplicateName,
		UndefinedSymbol,
		Parse,
		CollectionEnd,
	};

	class Value;
	struct MethodInfo;

	using NativeFunction = Value(*)(Interpreter&, std::vector<Value>&);

	struct Callable {
		ObjectPtr<RuntimeObject> Instance;
		AstNode const* Node{ nullptr };
		NativeFunction Native{ nullptr };
		MethodInfo const* Method{ nullptr };
		std::string Name;
	};

	class Value final {
	public:
		constexpr Value() noexcept : m_Type(ValueType::Null) {}
		constexpr Value(Int v) noexcept : iValue(v), m_Type(ValueType::Integer) {}
		constexpr Value(int v) noexcept : iValue(v), m_Type(ValueType::Integer) {}
		constexpr Value(Real d) noexcept : dValue(d), m_Type(ValueType::Real) {}
		constexpr Value(Bool b) noexcept : bValue(b), m_Type(ValueType::Boolean) {}
		constexpr Value(ValueType t) noexcept : m_Type(t) {}
		constexpr Value(AstNode const* node) noexcept : nValue(node), m_Type(ValueType::AstNode) {}
		constexpr Value(NativeFunction f) noexcept : fValue(f), m_Type(ValueType::NativeFunction) {}

		Value(const char* s) noexcept;
		Value(RuntimeObject* o) noexcept;
		Value(Callable* c) noexcept;

		Value(Value const& other) noexcept;
		Value& operator=(Value const& other) noexcept;
		Value(Value&& other) noexcept;
		Value& operator=(Value&& other) noexcept;

		static Value FromToken(Token const& token);
		static Value Error(ValueErrorType type = ValueErrorType::Unspecfied, const char* desc = nullptr);
#ifdef _WIN32
		static Value HResult(int hr);
#endif

		~Value() noexcept;

		ObjectType const* GetObjectType() const;

		ValueType Type() const noexcept {
			return m_Type;
		}

		bool IsNull() const noexcept {
			return m_Type == ValueType::Null;
		}

		bool IsInteger() const noexcept {
			return m_Type == ValueType::Integer;
		}

		bool IsReal() const noexcept {
			return m_Type == ValueType::Real;
		}
		bool IsBoolean() const noexcept {
			return m_Type == ValueType::Boolean;
		}
		bool IsObject() const noexcept {
			return m_Type == ValueType::Object;
		}
		bool IsObjectType() const noexcept;

		bool IsAstNode() const noexcept {
			return m_Type == ValueType::AstNode;
		}
		bool IsStruct() const noexcept {
			return m_Type == ValueType::Struct;
		}
		bool IsString() const noexcept {
			return m_Type == ValueType::String;
		}
		bool IsError() const noexcept {
			return m_Type == ValueType::Error;
		}
		bool IsNativeFunction() const noexcept {
			return m_Type == ValueType::NativeFunction;
		}

		bool IsCallable() const noexcept {
			return m_Type == ValueType::Callable;
		}

		Int AsInteger() const noexcept {
			assert(IsInteger());
			return iValue;
		}

		Real AsReal() const noexcept {
			assert(IsReal());
			return dValue;
		}

		Int ToInteger() const;
		Bool ToBoolean() const;
		Real ToReal() const;
		RuntimeObject* ToObject() const;
		ObjectType* ToTypeObject() const;

		AstNode const* AsAstNode() const noexcept {
			assert(IsAstNode());
			return nValue;
		}

		RuntimeObject* AsObject() noexcept {
			assert(IsObject());
			return oValue;
		}

		RuntimeObject const* AsObject() const noexcept {
			assert(IsObject());
			return oValue;
		}

		NativeFunction AsNativeCode() const noexcept {
			assert(IsNativeFunction());
			return fValue;
		}

		Callable* AsCallable() const noexcept {
			assert(IsCallable());
			return cValue;
		}

		Value BinaryOperator(TokenType op, Value const& rhs) const;
		Value UnaryOperator(TokenType op) const;
		Value& Assign(Value const& right, TokenType assign);
		Value& AssignArrayIndex(Value const& index, Value const& right, TokenType assign);

		std::string ToString() const noexcept;

		Value Add(Value const& rhs) const;
		Value Sub(Value const& rhs) const;
		Value Mul(Value const& rhs) const;
		Value Div(Value const& rhs) const;
		Value Mod(Value const& rhs) const;

		Value And(Value const& rhs) const;
		Value Or(Value const& rhs) const;
		Value BitwiseAnd(Value const& rhs) const;
		Value BitwiseOr(Value const& rhs) const;
		Value BitwiseXor(Value const& rhs) const;

		Value Equal(Value const& rhs) const;
		Value NotEqual(Value const& rhs) const;
		Value GreaterThan(Value const& rhs) const;
		Value LessThan(Value const& rhs) const;
		Value GreaterThanOrEqual(Value const& rhs) const;
		Value LessThanOrEqual(Value const& rhs) const;

		Value Negate() const;
		Value Not() const;
		Value BitwiseNot() const;

		Value InvokeIndexer(Value const& index) const;

		void Free() noexcept;

	private:
		union {
			Int iValue;
			Real dValue;
			Bool bValue;
			RuntimeObject* oValue;
			AstNode const* nValue;
			NativeStruct* sValue;
			Callable* cValue;
			char* strValue;
			NativeFunction fValue;
		};
		union {
			struct {
				ValueType m_Type;
				ValueErrorType error;
				uint32_t m_StrLen;
			};
		};
	};

	static_assert(sizeof(Value) == 16);

}

std::ostream& operator<<(std::ostream& os, const Dynamix::Value& v);

