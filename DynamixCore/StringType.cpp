#include "StringType.h"

using namespace Dynamix;

StringType* StringType::Get() {
	static StringType type;
	return &type;
}

StringType::StringType() : StaticObjectType("String") {
}
