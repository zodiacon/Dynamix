#include "BooleanType.h"

using namespace Dynamix;

BooleanType* BooleanType::Get() {
    static BooleanType type;
    return &type;
}

BooleanType::BooleanType() : StaticObjectType("Boolean") {
}

