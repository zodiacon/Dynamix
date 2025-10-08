#include "SFTypes.h"

using namespace SFTypes;
using namespace Dynamix;

Vector2uType* Vector2uType::Get() {
    static StructDesc desc{
        std::vector<StructField> {
        { "X", sizeof(float), 0 },
        { "Y", sizeof(float), 4 },
        },
    };
    static Vector2uType type("Vector2u", std::move(desc));
    return &type;
}

Vector2uType::Vector2uType(std::string name, Dynamix::StructDesc desc) : StructType(std::move(name), std::move(desc)) {
}
