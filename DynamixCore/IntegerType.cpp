#include "IntegerType.h"

using namespace Dynamix;

IntegerType* IntegerType::Get() {
    static IntegerType type;
    return &type;
}

IntegerType::IntegerType() : StaticObjectType("Integer") {
}

