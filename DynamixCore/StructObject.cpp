#include "StructObject.h"

using namespace Dynamix;

StructType::StructType(std::string name, StructDesc desc) noexcept : ObjectType(std::move(name)), m_Desc(std::move(desc)) {
	uint16_t size = 0;
	for (auto& f : m_Desc.Fields)
		size += f.Size;
	m_Desc.TotalSize = size;
}

bool StructType::HasField(std::string const& name) const noexcept {
	return std::find_if(m_Desc.Fields.begin(), m_Desc.Fields.end(), [&](auto& f) { return f.Name == name; }) != m_Desc.Fields.end();
}

StructField const* StructType::GetStructField(std::string const& name) const noexcept {
	if (auto const it = std::find_if(m_Desc.Fields.begin(), m_Desc.Fields.end(), [&](auto& f) { return f.Name == name; }); it != m_Desc.Fields.end())
		return &(*it);

	return nullptr;
}

Value Dynamix::ValueFromField(void const* data, StructField const* field) {
	auto p = (uint8_t const*)data + field->Offset;
	auto size = field->Size;

	switch (field->Type) {
		case ValueType::Real:
			assert(size == sizeof(float) || size == sizeof(double));
			return size == sizeof(float) ? Real(*(float*)(p)) : Real(*(double*)(p));

		case ValueType::Integer:
			assert(size <= sizeof(Int));
			if (size == sizeof(int8_t))
				return Int(*(int8_t*)p);
			if (size == sizeof(int16_t))
				return Int(*(int16_t*)p);
			if (size <= sizeof(int))
				return Int(*(int*)p);
			return *(Int*)p;
	}
	return Value::Error();
}
