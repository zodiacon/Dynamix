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
		Null,
		Integer = 1,
		Real = 2,
		Boolean = 4,
		Object = 8,
		AstNode = 0x10,
		Struct = 0x20,
		String = 0x40,
		Error = 0xf000,
	};

	enum class ValueErrorType {
		Unspecfied = 0,
		OutOfMemory,
		DivideByZero,
		UnsupportedBinaryOperator,
		UnsupportedUnaryOperator,
		TypeMismatch,
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

		Int AsInteger() const noexcept;
		Real AsReal() const noexcept;
		Bool AsBoolean() const noexcept;

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
