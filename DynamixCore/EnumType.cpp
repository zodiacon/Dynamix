#include "EnumType.h"
#include "TypeHelper.h"

using namespace Dynamix;

EnumType::EnumType() : StaticObjectType("Enum") {
}

EnumType* EnumType::Get() {
	static EnumType type;
	return &type;
}
