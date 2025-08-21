#include <format>
#include <cassert>

#include "Value.h"
#include "RuntimeObject.h"

using namespace Dynamix;

Int Value::ToInteger() const noexcept {
	switch (m_Type) {
		case ValueType::Integer: return iValue;
		case ValueType::Real: return static_cast<Int>(dValue);
		case ValueType::Boolean: return bValue ? 1 : 0;
	}
	assert(false);
	return 0;
}

Bool Value::ToBoolean() const noexcept {
	switch (m_Type) {
		case ValueType::Integer: return iValue ? true : false;
		case ValueType::Real: return dValue ? true : false;
		case ValueType::Boolean: return bValue;
	}
	assert(false);
	return false;
}

Real Value::ToReal() const noexcept {
	switch (m_Type) {
		case ValueType::Integer: return static_cast<Real>(iValue);
		case ValueType::Real: return dValue;
		case ValueType::Boolean: return dValue ? 1 : 0;
	}
	assert(false);
	return 0.0;
}

Value Value::BinaryOperator(TokenType op, Value const& rhs) const noexcept {
	switch (op) {
		case TokenType::Operator_Plus: return Add(rhs);
		case TokenType::Operator_Minus: return Sub(rhs);
		case TokenType::Operator_Mul: return Mul(rhs);
		case TokenType::Operator_Div: return Div(rhs);
		case TokenType::Operator_Mod: return Mod(rhs);

		case TokenType::Operator_Equal: return Equal(rhs);
		case TokenType::Operator_GreaterThan: return GreaterThan(rhs);
		case TokenType::Operator_GreaterThanOrEqual: return GreaterThanOrEqual(rhs);
		case TokenType::Operator_NotEqual: return NotEqual(rhs);
		case TokenType::Operator_LessThan: return LessThan(rhs);
		case TokenType::Operator_LessThanOrEqual: return LessThanOrEqual(rhs);
	}
	return Value::Error(ValueErrorType::UnsupportedBinaryOperator);
}

Value Dynamix::Value::UnaryOperator(TokenType op) const noexcept {
	switch (op) {
		case TokenType::Operator_Minus: return Negate();
	}
	return Value::Error(ValueErrorType::UnsupportedUnaryOperator);
}

std::string Value::ToString() const noexcept {
	switch (m_Type) {
		case ValueType::Null:
			return "";
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

Value Value::Add(Value const& rhs) const noexcept {
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
	}
	return Value::Error(ValueErrorType::TypeMismatch);
}

Value Value::Sub(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer:
			return iValue - rhs.iValue;
		case ValueType::Integer | ValueType::Real:
			return ToReal() - rhs.ToReal();
		case ValueType::Real:
			return dValue - rhs.dValue;
	}
	return Value::Error(ValueErrorType::TypeMismatch);
}

Value Value::Mul(Value const& rhs) const noexcept {
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
	assert(false);
	return Value::Error(ValueErrorType::TypeMismatch);
}

Value Value::Div(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Real:
			if (rhs.dValue == 0)
				return Value::Error();
			return dValue / rhs.dValue;
		case ValueType::Integer:
			if (rhs.iValue == 0)
				return Value::Error();
			return iValue / rhs.iValue;
		case ValueType::Integer | ValueType::Real:
			auto r = rhs.ToReal();
			if (r == 0)
				break;
			return ToReal() / rhs.ToReal();
	}
	assert(false);
	return Value::Error();
}

Value Value::Mod(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: 
			return rhs.iValue == 0 ? Value::Error(ValueErrorType::DivideByZero) : iValue % rhs.iValue;
	}
	return Value::Error(ValueErrorType::TypeMismatch);
}

void Value::Free() noexcept {
	if (m_Type == ValueType::Null)
		return;

	if (m_Type == ValueType::Object)
		oValue->Release();
	else if (m_Type == ValueType::String)
		free(strValue);
	m_Type = ValueType::Null;
}

Value::Value(RuntimeObject* o) noexcept : oValue(o), m_Type(ValueType::Object) {
	o->AddRef();
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
		strValue = (char*)malloc(m_StrLen);
		if (strValue == nullptr) {
			*this = Value::Error(ValueErrorType::OutOfMemory);
			return;
		}
		memcpy(strValue, other.strValue, m_StrLen);
	}
}

Value& Value::operator=(Value const& other) noexcept {
	if (this != &other) {
		Free();
		m_Type = other.m_Type;
	}
	return *this;
}

Dynamix::Value::Value(Value&& other) noexcept : m_Type(other.m_Type), oValue(other.oValue) {
	other.m_Type = ValueType::Null;
}

Value& Value::operator=(Value&& other) noexcept {
	if (this != &other) {
		Free();
		m_Type = other.m_Type;
		oValue = other.oValue;
		other.m_Type = ValueType::Null;
	}
	return *this;
}

Value Value::FromToken(Token const& token) {
	switch (token.Type) {
		case TokenType::Integer: return Value(token.iValue);
		case TokenType::Real: return Value(token.rValue);
		case TokenType::Keyword_True: return Value(true);
		case TokenType::Keyword_False: return Value(false);
		case TokenType::String: return Value(token.Lexeme.c_str());
	}
	assert(false);
	return Value();
}

Value Value::Error(ValueErrorType type) {
	Value err(ValueType::Error);
	err.error = type;
	return err;
}

Value::~Value() {
	Free();
}

Value Value::Equal(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue == rhs.iValue;
		case ValueType::Real: return dValue == rhs.dValue;
		case ValueType::Boolean: return bValue == rhs.bValue;
		case ValueType::Null: return true;
		case ValueType::Integer | ValueType::Real: return ToReal() == rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() == rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) == 0;
	}
	return Value(false);
}

Value Value::NotEqual(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue != rhs.iValue;
		case ValueType::Real: return dValue != rhs.dValue;
		case ValueType::Boolean: return bValue != rhs.bValue;
		case ValueType::Null: return false;
		case ValueType::Integer | ValueType::Real: return ToReal() != rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() != rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) != 0;
	}
	return Value(true);
}

Value Value::LessThan(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue < rhs.iValue;
		case ValueType::Real: return dValue < rhs.dValue;
		case ValueType::Boolean: return bValue < rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() < rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() < rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) < 0;
	}
	return Value(false);
}

Value Value::LessThanOrEqual(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue <= rhs.iValue;
		case ValueType::Real: return dValue <= rhs.dValue;
		case ValueType::Boolean: return bValue <= rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() <= rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() <= rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) <= 0;
		case ValueType::Null: return true;
	}
	return Value(false);
}

Value Dynamix::Value::Negate() const noexcept {
	switch (m_Type) {
		case ValueType::Integer: return -iValue;
		case ValueType::Real: return -dValue;
	}
	return Value::Error(ValueErrorType::TypeMismatch);
}

Value Value::GreaterThan(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue > rhs.iValue;
		case ValueType::Real: return dValue > rhs.dValue;
		case ValueType::Boolean: return bValue > rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() > rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() > rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) > 0;
	}
	return Value(false);
}

Value Value::GreaterThanOrEqual(Value const& rhs) const noexcept {
	switch (Type() | rhs.Type()) {
		case ValueType::Integer: return iValue >= rhs.iValue;
		case ValueType::Real: return dValue >= rhs.dValue;
		case ValueType::Boolean: return bValue >= rhs.bValue;
		case ValueType::Integer | ValueType::Real: return ToReal() >= rhs.ToReal();
		case ValueType::Integer | ValueType::Boolean: return ToBoolean() >= rhs.ToBoolean();
		case ValueType::String: return ::strcmp(strValue, rhs.strValue) >= 0;
		case ValueType::Null: return true;
	}
	return Value(false);
}

//std::ostream& operator<<(std::ostream& os, Value const& v) {
//	os << v.ToString();
//	return os;
//}
