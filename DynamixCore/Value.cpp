#include "Value.h"
#include <format>

#include "Value.h"
#include "RuntimeObject.h"

std::string Dynamix::Value::ToString() const {
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
			return oValue->Invoke("ToString").ToString();
	}
	return "";
}

Dynamix::Value::Value(RuntimeObject* o) : oValue(o), m_Type(ValueType::Object) {
	o->AddRef();
}

Dynamix::Value::~Value() {
	if (m_Type == ValueType::Object)
		oValue->Release();
}
