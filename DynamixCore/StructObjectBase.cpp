#include "StructObject.h"

using namespace Dynamix;

StructObject::StructObject(StructType* type) noexcept : RuntimeObject(type), m_Data(::malloc(type->Desc().TotalSize)) {
}

StructObject::~StructObject() {
	::free(m_Data);
}
