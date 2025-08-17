#pragma once

#include <string>

namespace Dynamix {
	class RuntimeObject;

	enum class ValueType : size_t {
		Null,
		Integer,
		Real,
		Boolean,
		Object,
	};

	class Value final {
	public:
		Value() : m_Type(ValueType::Null) {}
		Value(long long v) : iValue(v), m_Type(ValueType::Integer) {}
		Value(double d) : dValue(d), m_Type(ValueType::Real) {}
		Value(bool b) : bValue(b), m_Type(ValueType::Boolean) {}
		Value(RuntimeObject* o);

		~Value() noexcept;

		ValueType Type() const {
			return m_Type;
		}

		bool IsInteger() const {
			return m_Type == ValueType::Integer;
		}

		long long Integer() const;
		double Real() const;
		bool Boolean() const;

		bool IsReal() const {
			return m_Type == ValueType::Real;
		}
		bool IsBoolean() const {
			return m_Type == ValueType::Boolean;
		}
		bool IsObject() const {
			return m_Type == ValueType::Object;
		}

		Value ToInteger() const;
		Value ToBoolean() const;
		Value ToReal() const;
		
		std::string ToString() const;

	private:
		union {
			long long iValue;
			double dValue;
			bool bValue;
			RuntimeObject* oValue;
		};
		ValueType m_Type;
	};

	static_assert(sizeof(Value) == 16);
}


