#include "ComplexType.h"
#include "TypeHelper.h"

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
