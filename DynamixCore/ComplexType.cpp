#include "ComplexType.h"
#include "TypeHelper.h"
#include "Runtime.h"

using namespace Dynamix;

ComplexType* ComplexType::Get() {
    static ComplexType type;
    return &type;
}

ComplexObject* Dynamix::ComplexType::Create(Real real, Real image) {
	return new ComplexObject(real, image);
}

RuntimeObject* ComplexType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	return Create(args.size() > 0 ? args[0].ToReal() : 0, args.size() > 1 ? args[1].ToReal() : 0);
}

ComplexType::ComplexType() : StaticObjectType("Complex") {
	BEGIN_METHODS(ComplexObject)
		METHOD(Real, 0, return inst->Real();),
		METHOD(Image, 0, return inst->Image();),
		METHOD(Real, 1, inst->Real(args[1].ToReal()); return inst;),
		METHOD(Image, 1, inst->Image(args[1].ToReal()); return inst;),
		METHOD(Length, 0, return inst->Length();),
		METHOD(LengthSquared, 0, return inst->LengthSquared();),
		END_METHODS()
}

Value ComplexObject::InvokeOperator(Interpreter& intr, TokenType op, Value const& rhs) const {
	switch (rhs.Type()) {
		case ValueType::Integer:
		case ValueType::Real:
			return new ComplexObject(m_Num + std::complex(rhs.ToReal()));

		case ValueType::Object:
			break;

		default:
			return RuntimeObject::InvokeOperator(intr, op, rhs);
	}

	auto obj = rhs.ToObject();
	if (obj->Type() == ComplexType::Get()) {
		auto other = reinterpret_cast<ComplexObject*>(obj);
		switch (op) {
			case TokenType::Plus: return new ComplexObject(m_Num + other->m_Num);
			case TokenType::Minus: return new ComplexObject(m_Num - other->m_Num);
			case TokenType::Mul: return new ComplexObject(m_Num * other->m_Num);
			case TokenType::Div:
				if(other->IsZero())
					throw RuntimeError(RuntimeErrorType::DivisionByZero, "Cannot divide by zero");
				return new ComplexObject(m_Num / other->m_Num);
		}
	}
	return RuntimeObject::InvokeOperator(intr, op, rhs);
}

std::string ComplexObject::ToString() const {
	return std::format("({},{}*i)", Real(), Image());
}
