#include "StructObject.h"

using namespace Dynamix;

StructType::StructType(std::string name, StructDesc desc) noexcept : ObjectType(std::move(name)), m_Desc(std::move(desc)) {
	uint16_t size = 0;
	for (auto& f : m_Desc.Fields)
		size += f.Size;
	m_Desc.TotalSize = size;
}
