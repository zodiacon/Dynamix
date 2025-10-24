#include "ObjectInstance.h"
#include "TypeHelper.h"

using namespace Dynamix;

ObjectInstance* ObjectInstance::Get() {
	static ObjectInstance type;
	return &type;
}

Value ObjectInstance::IsObject(Value const& value) noexcept {
	return value.IsObject();
}

Value ObjectInstance::IsEmpty(Value const& value) noexcept {
	return value.IsNull();
}

ObjectInstance::ObjectInstance() : StaticObjectType("Object") {
	BEGIN_METHODS(ObjectInstance)
		METHOD_STATIC(IsObject, 1, return ObjectInstance::IsObject(args[0]);),
		METHOD_STATIC(IsEmpty, 1, return ObjectInstance::IsEmpty(args[0]);),
		END_METHODS()
}
