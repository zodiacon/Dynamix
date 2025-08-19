#include "Value.h"
#include <format>
#include <cassert>
#include "Value.h"
#include "RuntimeObject.h"

using namespace Dynamix;

Int Dynamix::Value::ToInteger() const noexcept {
	switch (m_Type) {
		case ValueType::Integer: return iValue;
		case ValueType::Real: return static_cast<Int>(dValue);
		case ValueType::Boolean: return bValue ? 1 : 0;
	}
	assert(false);
	return 0;
}

Bool Dynamix::Value::ToBoolean() const noexcept {
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

Value Dynamix::Value::BinaryOperator(TokenType op, Value const& rhs) const noexcept {
	switch (op) {
		case TokenType::Operator_Plus: return Add(rhs);
		case TokenType::Operator_Minus: return Sub(rhs);
		case TokenType::Operator_Mul: return Mul(rhs);
		case TokenType::Operator_Div: return Div(rhs);
		case TokenType::Operator_Mod: return Mod(rhs);
	}
	return Value::Error(ValueErrorType::UnsupportedBinaryOperator);
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
	return Value::Error();
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
		memcpy(strValue, s, m_StrLen);
	}
}

Value::Value(Value const& other) noexcept : oValue(other.oValue), m_Type(other.m_Type) {
	if (m_Type == ValueType::Object)
		oValue->AddRef();
	else if (m_Type == ValueType::String) {
		m_StrLen = other.m_StrLen;
		strValue = (char*)malloc(m_StrLen);
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
