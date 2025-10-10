#include <format>
#include <cassert>

#include "Value.h"
#include "RuntimeObject.h"
#include <stacktrace>
#include "Runtime.h"
#include "ObjectType.h"
#include "StringType.h"

using namespace Dynamix;

Int Value::ToInteger() const {
	switch (m_Type) {
		case ValueType::Integer: return iValue;
		case ValueType::Real: return static_cast<Int>(dValue);
		case ValueType::Boolean: return bValue ? 1 : 0;
	}
	throw RuntimeError(RuntimeErrorType::CannotConvertToInteger, std::format("Cannot convert {} to Integer", ToString()));
}

Bool Value::ToBoolean() const {
	switch (m_Type) {
		case ValueType::Integer: return iValue ? true : false;
		case ValueType::Real: return dValue ? true : false;
		case ValueType::Boolean: return bValue;
		case ValueType::Null: return false;
		case ValueType::String: return m_StrLen > 0;
	}
	throw RuntimeError(RuntimeErrorType::CannotConvertToBoolean, std::format("Cannot convert {} to Boolean", ToString()));
}

Real Value::ToReal() const {
	switch (m_Type) {
		case ValueType::Integer: return static_cast<Real>(iValue);
		case ValueType::Real: return dValue;
		case ValueType::Boolean: return dValue ? 1 : 0;
	}
	throw RuntimeError(RuntimeErrorType::CannotConvertToReal, std::format("Cannot convert {} to Real", ToString()));
}

RuntimeObject const* Value::ToObject() const {
	if (!IsObject())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Object expected");
	return oValue;
}

ObjectType const* Value::ToTypeObject() const {
	if (m_Type != ValueType::Object || !oValue->IsObjectType())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Type Object expected");
	return reinterpret_cast<ObjectType const*>(oValue);
}

Value Value::BinaryOperator(TokenType op, Value const& rhs) const {
	switch (op) {
		case TokenType::Plus: return Add(rhs);
		case TokenType::Minus: return Sub(rhs);
		case TokenType::Mul: return Mul(rhs);
		case TokenType::Div: return Div(rhs);
		case TokenType::Mod: return Mod(rhs);

		case TokenType::Equal: return Equal(rhs);
		case TokenType::GreaterThan: return GreaterThan(rhs);
		case TokenType::GreaterThanOrEqual: return GreaterThanOrEqual(rhs);
		case TokenType::NotEqual: return NotEqual(rhs);
		case TokenType::LessThan: return LessThan(rhs);
		case TokenType::LessThanOrEqual: return LessThanOrEqual(rhs);

		case TokenType::And: return And(rhs);
		case TokenType::Or: return Or(rhs);

		case TokenType::BitwiseAnd: return BitwiseAnd(rhs);
		case TokenType::BitwiseOr: return BitwiseOr(rhs);
		case TokenType::BitwiseXor: return BitwiseXor(rhs);

		case TokenType::StreamLeft: return ShiftLeft(rhs);
		case TokenType::StreamRight: return ShiftRight(rhs);
	}
	throw RuntimeError(RuntimeErrorType::UnknownOperator, std::format("Unsupported operator {}", Token::TypeToString(op)));
}

Value Value::UnaryOperator(TokenType op) const {
	switch (op) {
		case TokenType::Minus: return Negate();
		case TokenType::Not: return !ToBoolean();
		case TokenType::TypeOf:
			return static_cast<RuntimeObject const*>(GetObjectType());
	}
	throw RuntimeError(RuntimeErrorType::UnknownOperator, std::format("Unsupported operator {}", Token::TypeToString(op)));
}

Value& Value::Assign(Value const& right, TokenType assign) {
	switch (assign) {
		case TokenType::Assign: *this = right; break;
		case TokenType::Assign_Add: *this = BinaryOperator(TokenType::Plus, right); break;
		case TokenType::Assign_Sub: *this = BinaryOperator(TokenType::Minus, right); break;
		case TokenType::Assign_Mul: *this = BinaryOperator(TokenType::Mul, right); break;
		case TokenType::Assign_Div: *this = BinaryOperator(TokenType::Div, right); break;
		case TokenType::Assign_Mod: *this = BinaryOperator(TokenType::Mod, right); break;
		case TokenType::Assign_And: *this = BinaryOperator(TokenType::BitwiseAnd, right); break;
		case TokenType::Assign_Or: *this = BinaryOperator(TokenType::BitwiseOr, right); break;
		case TokenType::Assign_Xor: *this = BinaryOperator(TokenType::BitwiseXor, right); break;
	}
	return *this;
}

Value& Value::AssignArrayIndex(Value const& index, Value const& right, TokenType assign) {
	switch (m_Type) {
	case ValueType::Object:
		const_cast<RuntimeObject*>(oValue)->InvokeSetIndexer(index, right, assign);
		break;

	case ValueType::String:
		assert(false);
		break;
	}
	return *this;
}

std::string Value::ToString() const noexcept {
	switch (m_Type) {
		case ValueType::Null:
			return "<empty>";
		case ValueType::Integer:
			return std::format("{}", iValue);
		case ValueType::Real:
			return std::format("{}", dValue);
		case ValueType::Boolean:
			return bValue ? "true" : "false";
		case ValueType::Object:
			return oValue->ToString();
		case ValueType::Error:
			return "<Error>";
		case ValueType::String:
			return strValue;
	}
	return "";
}

Value Value::Add(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer:
			return iValue + rhs.iValue;
		case ValueType::Real:
			return dValue + rhs.dValue;
		case ValueType::Integer | ValueType::Real:
		case ValueType::Real | ValueType::Boolean:
			return ToReal() + rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean:
			return ToInteger() + rhs.ToInteger();
		case ValueType::String:
			return Value((std::string(strValue) + rhs.strValue).c_str());
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot add {} and {}", ToString(), rhs.ToString()));
}

Value Value::Sub(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer:
			return iValue - rhs.iValue;
		case ValueType::Integer | ValueType::Real:
			return ToReal() - rhs.ToReal();
		case ValueType::Real:
			return dValue - rhs.dValue;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot subtract {} from {}", rhs.ToString(), ToString()));
}

Value Value::Mul(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer:
			return iValue * rhs.iValue;
		case ValueType::Real:
			return dValue * rhs.dValue;
		case ValueType::Integer | ValueType::Real:
		case ValueType::Real | ValueType::Boolean:
			return ToReal() * rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean:
			return ToInteger() * rhs.ToInteger();
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot multiply {} and {}", ToString(), rhs.ToString()));
}

Value Value::Div(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Real:
			if (rhs.dValue == 0)
				return Value::Error();
			return dValue / rhs.dValue;
		case ValueType::Integer:
			if (rhs.iValue == 0)
				throw RuntimeError(RuntimeErrorType::DivisionByZero, "Cannot divide by zero");
			return iValue / rhs.iValue;
		case ValueType::Integer | ValueType::Real:
			auto r = rhs.ToReal();
			if (r == 0)
				throw RuntimeError(RuntimeErrorType::DivisionByZero, "Cannot divide by zero");
			return ToReal() / rhs.ToReal();
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot divide {} by {}", ToString(), rhs.ToString()));
}

Value Value::Mod(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: 
			return rhs.iValue == 0 ? Value::Error(ValueErrorType::DivideByZero) : iValue % rhs.iValue;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot modulo {} by {}", ToString(), rhs.ToString()));
}

Value Value::And(Value const& rhs) const {
	return ToBoolean() && rhs.ToBoolean();
}

Value Value::Or(Value const& rhs) const {
	return ToBoolean() || rhs.ToBoolean();
}

Value Value::BitwiseAnd(Value const& rhs) const {
	if (IsInteger() && rhs.IsInteger())
		return Value(iValue & rhs.iValue);

	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot bitwise AND {} by {}", ToString(), rhs.ToString()));
}

Value Value::BitwiseOr(Value const& rhs) const {
	if (IsInteger() && rhs.IsInteger())
		return Value(iValue | rhs.iValue);

	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot bitwise OR {} by {}", ToString(), rhs.ToString()));
}

Value Value::BitwiseXor(Value const& rhs) const {
	if (IsInteger() && rhs.IsInteger())
		return Value(iValue ^ rhs.iValue);

	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot bitwise XOR {} by {}", ToString(), rhs.ToString()));
}

Value Value::ShiftLeft(Value const& rhs) const {
	return ToInteger() << rhs.ToInteger();
}

Value Value::ShiftRight(Value const& rhs) const {
	return ToInteger() >> rhs.ToInteger();
}

Value Value::InvokeIndexer(Value const& index) const {
	switch (m_Type) {
		case ValueType::String:
		{
			auto i = index.ToInteger();
			if (i < 0 || i >= m_StrLen)
				throw RuntimeError(RuntimeErrorType::IndexOutOfRange, std::format("Index {} is out of range", i));
			return Int(strValue[i]);
		}

		case ValueType::Object:
			return const_cast<RuntimeObject*>(oValue)->InvokeGetIndexer(index);
	}
	throw RuntimeError(RuntimeErrorType::IndexerNotSupported, std::format("Indexer not supported on type {}", GetObjectType()->Name()));
}

void Value::Free() noexcept {
	switch (m_Type) {
		case ValueType::Error:
			if (strValue)
				free(strValue);
			break;

		case ValueType::Object:
			oValue->Release();
			break;

		case ValueType::String:
			free(strValue);
			break;

		case ValueType::Callable:
			delete cValue;
			break;
	}
	m_Type = ValueType::Null;
}

Value::Value(RuntimeObject const* o) noexcept : oValue(o), m_Type(ValueType::Object) {
	o->AddRef();
}

Value::Value(Callable* c) noexcept : cValue(c), m_Type(ValueType::Callable) {
}

Value::Value(const char* s) noexcept : m_Type(ValueType::String) {
	strValue = (char*)::malloc((m_StrLen = (unsigned)strlen(s)) + 1);
	if (strValue == nullptr) {
		m_Type = ValueType::Error;
		error = ValueErrorType::OutOfMemory;
	}
	else {
		memcpy(strValue, s, m_StrLen + 1);
	}
}

Value::Value(Value const& other) noexcept : oValue(other.oValue), m_Type(other.m_Type) {
	if (m_Type == ValueType::Object)
		oValue->AddRef();
	else if (m_Type == ValueType::String) {
		m_StrLen = other.m_StrLen;
		strValue = (char*)malloc(m_StrLen + 1);
		if (strValue == nullptr) {
			*this = Value::Error(ValueErrorType::OutOfMemory);
			return;
		}
		memcpy(strValue, other.strValue, m_StrLen + 1);
	}
	else if (m_Type == ValueType::Callable) {
		cValue = new Callable(*other.cValue);
	}
}

Value& Value::operator=(Value const& other) noexcept {
	if (this != &other) {
		Free();
		m_Type = other.m_Type;
		m_StrLen = other.m_StrLen;
		oValue = other.oValue;
		if (m_Type == ValueType::String) {
			strValue = (char*)malloc(m_StrLen + 1);
			memcpy(strValue, other.strValue, m_StrLen + 1);
		}
		else if (m_Type == ValueType::Object) {
			oValue->AddRef();
		}
		else if (m_Type == ValueType::Callable) {
			cValue = new Callable(*other.cValue);
		}
	}
	return *this;
}

Value::Value(Value&& other) noexcept : m_Type(other.m_Type), oValue(other.oValue), m_StrLen(other.m_StrLen) {
	other.m_Type = ValueType::Null;

}

Value& Value::operator=(Value&& other) noexcept {
	if (this != &other) {
		Free();
		m_Type = other.m_Type;
		oValue = other.oValue;
		m_StrLen = other.m_StrLen;
		other.m_Type = ValueType::Null;
	}
	return *this;
}

Value Value::FromToken(Token const& token) {
	switch (token.Type) {
		case TokenType::Integer: return token.Integer;
		case TokenType::Real: return token.Real;
		case TokenType::True: return Value(true);
		case TokenType::False: return Value(false);
		case TokenType::String: return Value(token.Lexeme);
		case TokenType::Empty: return Value();
	}
	assert(false);
	return Value();
}

#ifdef _WIN32
Value Value::HResult(int hr) {
	Value err(ValueType::Error);
	err.m_StrLen = hr;
	return err;
}
#endif
Value Value::Error(ValueErrorType type, const char* desc) {
	Value err(ValueType::Error);
	err.error = type;
	if (desc) {
		err.strValue = (char*)malloc((err.m_StrLen = (uint32_t)strlen(desc)) + 1);
		if (!err.strValue)
			err.error = ValueErrorType::OutOfMemory;
		else
			memcpy(err.strValue, desc, err.m_StrLen + 1);
	}
	else {
		err.strValue = nullptr;
		err.m_StrLen = 0;
	}
	return err;
}

Value::~Value() {
	Free();
}

ObjectType const* Value::GetObjectType() const {
	switch (m_Type) {
		case ValueType::Object: return oValue->Type();
		case ValueType::String: return StringTypeA::Get();
		//case ValueType::Integer: return &IntegerType::Get();
		//case ValueType::Real: return &RealType::Get();
		//case ValueType::Boolean: return &BooleanType::Get();
		//case ValueType::AstNode: return &FunctionType::Get();
	}
	return nullptr;
}

bool Value::IsObjectType() const noexcept {
	return IsObject() && AsObject()->IsObjectType();
}

Value Value::Equal(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue == rhs.iValue;
		case ValueType::Real: return dValue == rhs.dValue;
		case ValueType::Boolean: return bValue == rhs.bValue;
		case ValueType::Null: return true;
		case ValueType::Integer | ValueType::Real: return ToReal() == rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() == rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) == 0;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare {} and {}", ToString(), rhs.ToString()));
}

Value Value::NotEqual(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue != rhs.iValue;
		case ValueType::Real: return dValue != rhs.dValue;
		case ValueType::Boolean: return bValue != rhs.bValue;
		case ValueType::Null: return false;
		case ValueType::Integer | ValueType::Real: return ToReal() != rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() != rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) != 0;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare {} and {}", ToString(), rhs.ToString()));
}

Value Value::LessThan(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue < rhs.iValue;
		case ValueType::Real: return dValue < rhs.dValue;
		case ValueType::Boolean: return bValue < rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() < rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() < rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) < 0;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare {} and {}", ToString(), rhs.ToString()));
}

Value Value::LessThanOrEqual(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue <= rhs.iValue;
		case ValueType::Real: return dValue <= rhs.dValue;
		case ValueType::Boolean: return bValue <= rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() <= rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() <= rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) <= 0;
		case ValueType::Null: return true;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare '{}' and '{}'", ToString(), rhs.ToString()));
}

Value Value::Negate() const {
	switch (m_Type) {
		case ValueType::Integer: return -iValue;
		case ValueType::Real: return -dValue;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot negate '{}'", ToString()));
}

Value Value::Not() const {
	return !ToBoolean();
}

Value Value::BitwiseNot() const {
	if (IsInteger())
		return ~iValue;
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot bitwise not '{}'", ToString()));
}

Value Value::GreaterThan(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue > rhs.iValue;
		case ValueType::Real: return dValue > rhs.dValue;
		case ValueType::Boolean: return bValue > rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() > rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() > rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) > 0;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare {} and {}", ToString(), rhs.ToString()));
}

Value Value::GreaterThanOrEqual(Value const& rhs) const {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue >= rhs.iValue;
		case ValueType::Real: return dValue >= rhs.dValue;
		case ValueType::Boolean: return bValue >= rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() >= rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() >= rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) >= 0;
		case ValueType::Null: return true;
	}
	throw RuntimeError(RuntimeErrorType::TypeMismatch, std::format("Cannot compare {} and {}", ToString(), rhs.ToString()));
}

//std::ostream& operator<<(std::ostream& os, Value const& v) {
//	os << v.ToString();
//	return os;
//}
