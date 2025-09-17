#pragma once

#include <string>
#include <format>
#include <sstream>
#include <vector>
#include <cassert>

#include "Token.h"
#include "EnumClassBitwise.h"

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

	enum class ValueType : uint32_t {
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

	enum class ValueErrorType {
		Unspecfied = 0,
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
		RuntimeObject* Instance{ nullptr };
		AstNode const* Node{ nullptr };
		NativeFunction Native{ nullptr };
		std::string Method;
	};

	class Value final {
	public:
		constexpr Value() noexcept : m_Type(ValueType::Null) {}
		constexpr Value(Int v) noexcept : iValue(v), m_Type(ValueType::Integer) {}
		constexpr Value(int v) noexcept : iValue(v), m_Type(ValueType::Integer) {}
		constexpr Value(Real d) noexcept : dValue(d), m_Type(ValueType::Real) {}
		constexpr Value(Bool b) noexcept : bValue(b), m_Type(ValueType::Boolean) {}
		constexpr Value(ValueType t) noexcept : m_Type(t) {}
		constexpr Value(AstNode const* node) : nValue(node), m_Type(ValueType::AstNode) {}
		constexpr Value(NativeFunction f) : fValue(f), m_Type(ValueType::NativeFunction) {}
		Value(const char* s) noexcept;
		Value(RuntimeObject* o) noexcept;
		Value(Callable* c) noexcept;

		Value(Value const& other) noexcept;
		Value& operator=(Value const& other) noexcept;
		Value(Value&& other) noexcept;
		Value& operator=(Value&& other) noexcept;

		static Value FromToken(Token const& token);
		static Value Error(ValueErrorType type = ValueErrorType::Unspecfied);

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
			assert(m_Type == ValueType::Integer);
			return iValue;
		}

		Int ToInteger() const;
		Bool ToBoolean() const;
		Real ToReal() const;
		RuntimeObject* ToObject() const;

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

		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags);

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
			ValueErrorType error;
			char* strValue;
			NativeFunction fValue;
		};
		union {
			struct {
				ValueType m_Type;
				uint32_t m_StrLen;
			};
			AstNode* MemberNode;
			NativeFunction memberCode;
		};
	};

	static_assert(sizeof(Value) == 16);

}

std::ostream& operator<<(std::ostream& os, const Dynamix::Value& v);

//template<>
//struct std::formatter<Dynamix::Value> {
//	template<typename Context>
//	constexpr auto parse(Context& ctx) {
//		return ctx.end();
//	}
//
//	template<typename Context>
//	constexpr auto format(Dynamix::Value const& v, Context& ctx) const {
//		std::ostringstream out;
//		out << v;
//		return ranges::copy(move(out).str(), ctx.out()).out;
//	}
//};
//

