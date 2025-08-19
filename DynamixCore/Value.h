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
		DivideByZero,
		UnsupportedBinaryOperator,
		UnsupportedUnaryOperator,
	};

	class Value final {
	public:
		Value() : m_Type(ValueType::Null) {}
		Value(Int v) : iValue(v), m_Type(ValueType::Integer) {}
		Value(Real d) : dValue(d), m_Type(ValueType::Real) {}
		Value(Bool b) : bValue(b), m_Type(ValueType::Boolean) {}
		Value(RuntimeObject* o);
		Value(ValueType t) : m_Type(t) {}

		static Value FromToken(Token const& token);
		static Value Error(ValueErrorType type = ValueErrorType::Unspecfied);

		~Value() noexcept;

		ValueType Type() const {
			return m_Type;
		}

		bool IsInteger() const {
			return m_Type == ValueType::Integer;
		}

		Int AsInteger() const;
		Real AsReal() const;
		Bool AsBoolean() const;

		bool IsReal() const {
			return m_Type == ValueType::Real;
		}
		bool IsBoolean() const {
			return m_Type == ValueType::Boolean;
		}
		bool IsObject() const {
			return m_Type == ValueType::Object;
		}
		bool IsAstNode() const {
			return m_Type == ValueType::AstNode;
		}
		bool IsStruct() const {
			return m_Type == ValueType::Struct;
		}

		Int ToInteger() const;
		Bool ToBoolean() const;
		Real ToReal() const;

		Value BinaryOperator(TokenType op, Value const& rhs) const;
		Value UnaryOperator(TokenType op) const;

		std::string ToString() const;

		Value Add(Value const& rhs) const;
		Value Sub(Value const& rhs) const;
		Value Mul(Value const& rhs) const;
		Value Div(Value const& rhs) const;
		Value Mod(Value const& rhs) const;

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

template<>
constexpr bool enable_bitmask_operators(Dynamix::ValueType) { return true; }

