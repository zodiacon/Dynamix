#include "RealType.h"

using namespace Dynamix;

RealType* RealType::Get() {
    static RealType type;
    return &type;
}

RealType::RealType() : StaticObjectType("Real") {
}
