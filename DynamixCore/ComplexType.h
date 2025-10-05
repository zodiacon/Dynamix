#pragma once

#include "ObjectType.h"
#include "StructObjectBase.h"
#include <complex>

namespace Dynamix {
	class ComplexObject;

	class ComplexType final : public StaticObjectType {
	public:
		static ComplexType* Get();

		ComplexObject* Create(Real real = 0, Real image = 0);
		RuntimeObject* CreateObject(Interpreter& intr, std::vector<Value> const& args) override;

	private:
		ComplexType();
	};

	class ComplexObject : public RuntimeObject {
	public:
		ComplexObject(Real real = 0, Real image = 0) : RuntimeObject(ComplexType::Get()), m_Num(real, image) {}
		ComplexObject(std::complex<double> const& c) : ComplexObject(c.real(), c.imag()) {}

		Value InvokeOperator(Interpreter& intr, TokenType op, Value const& rhs) const override;

		bool IsZero() const noexcept {
			return Image() == 0 && Real() == 0;
		}

		Real Image() const noexcept {
			return m_Num.imag();
		}

		Real Real() const noexcept {
			return m_Num.real();
		}

		double Length() const noexcept {
			return std::sqrt(LengthSquared());
		}

		double LengthSquared() const noexcept {
			return Real() * Real() + Image() * Image();
		}

		void Real(double value) noexcept {
			m_Num.real(value);
		}

		void Image(double value) noexcept {
			m_Num.imag(value);
		}

		std::string ToString() const override;

	private:
		std::complex<double> m_Num;
	};
}

