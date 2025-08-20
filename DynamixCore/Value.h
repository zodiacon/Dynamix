#pragma once

#include <string>
#include "Token.h"
#include "EnumClassBitwise.h"

namespace Dynamix {
	class RuntimeObject;
	class AstNode;
	class NativeStruct;

	using Real = double;
	using Int = long long;
	using Bool = bool;

	enum class ValueType : uint16_t {
		Invalid = 0,
		Null = 1,
		Integer = 2,
		Real = 4,
		Boolean = 8,
		Object = 0x10,
		AstNode = 0x20,
		Struct = 0x40,
		String = 0x80,
		Error = 0xf000,
	};

	enum class ValueErrorType {
		Unspecfied = 0,
		OutOfMemory,
		DivideByZero,
		UnsupportedBinaryOperator,
		UnsupportedUnaryOperator,
		TypeMismatch,
		DuplicateName,
		UndefinedSymbol,
	};

	class Value final {
	public:
		Value() noexcept : m_Type(ValueType::Null) {}
		Value(Int v) noexcept : iValue(v), m_Type(ValueType::Integer) {}
		Value(Real d) noexcept : dValue(d), m_Type(ValueType::Real) {}
		Value(Bool b) noexcept : bValue(b), m_Type(ValueType::Boolean) {}
		Value(RuntimeObject* o) noexcept;
		Value(ValueType t) noexcept : m_Type(t) {}
		Value(const char* s) noexcept;

		Value(Value const& other) noexcept;
		Value& operator=(Value const& other) noexcept;
		Value(Value&& other) noexcept;
		Value& operator=(Value&& other) noexcept;

		static Value FromToken(Token const& token);
		static Value Error(ValueErrorType type = ValueErrorType::Unspecfied);

		~Value() noexcept;

		ValueType Type() const noexcept {
			return m_Type;
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

		Int ToInteger() const noexcept;
		Bool ToBoolean() const noexcept;
		Real ToReal() const noexcept;

		Value BinaryOperator(TokenType op, Value const& rhs) const noexcept;
		Value UnaryOperator(TokenType op) const noexcept;

		std::string ToString() const noexcept;

		Value Add(Value const& rhs) const noexcept;
		Value Sub(Value const& rhs) const noexcept;
		Value Mul(Value const& rhs) const noexcept;
		Value Div(Value const& rhs) const noexcept;
		Value Mod(Value const& rhs) const noexcept;

		Value And(Value const& rhs) const noexcept;
		Value Or(Value const& rhs) const noexcept;
		Value BitwiseAnd(Value const& rhs) const noexcept;
		Value BitwiseOr(Value const& rhs) const noexcept;
		Value BitwiseXor(Value const& rhs) const noexcept;

		Value Equal(Value const& rhs) const noexcept;
		Value NotEqual(Value const& rhs) const noexcept;
		Value GreaterThan(Value const& rhs) const noexcept;
		Value LessThan(Value const& rhs) const noexcept;
		Value GreaterThanOrEqual(Value const& rhs) const noexcept;
		Value LessThanOrEqual(Value const& rhs) const noexcept;

		Value Negate() const noexcept;
		Value Not() const noexcept;
		Value BitwiseNot() const noexcept;

		void Free() noexcept;

	private:
		union {
			Int iValue;
			Real dValue;
			Bool bValue;
			RuntimeObject* oValue;
			AstNode* nValue;
			NativeStruct* sValue;
			ValueErrorType error;
			char* strValue;
		};
		ValueType m_Type;
		unsigned m_StrLen;
	};

	static_assert(sizeof(Value) <= 16);
}
