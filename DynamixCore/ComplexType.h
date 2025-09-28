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

		Real Image() const {
			return m_Num.imag();
		}

		Real Real() const {
			return m_Num.real();
		}

		double Length() const {
			return std::sqrt(LengthSquared());
		}

		double LengthSquared() const {
			return Real() * Real() + Image() * Image();
		}

		void Real(double value) {
			m_Num.real(value);
		}

		void Image(double value) {
			m_Num.imag(value);
		}

	private:
		std::complex<double> m_Num;
	};
}

