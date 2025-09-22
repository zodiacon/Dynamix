#include "EnumType.h"
#include "TypeHelper.h"

using namespace Dynamix;

EnumType* EnumType::Get() {
	static EnumType type;
	return &type;
}

EnumType::EnumType() : StaticObjectType("Enum") {
	BEGIN_METHODS(EnumType)
		METHOD_EX(ToString, 2, SymbolFlags::Native | SymbolFlags::Static, return EnumType::ToString(args[0].ToTypeObject(), args[1].ToInteger());),
		END_METHODS()
}

Value EnumType::ToString(ObjectType* type, Int value) {
	auto etype = reinterpret_cast<CustomEnumType const*>(type);
	return etype->ToString(value);
}

CustomEnumType::CustomEnumType(std::string name) : ObjectType(std::move(name), EnumType::Get()) {
	BEGIN_METHODS(CustomEnumType)
		METHOD(ToString, 1, return inst->ToString(args[1].ToInteger());),
		END_METHODS()
}

bool CustomEnumType::AddField(std::unique_ptr<FieldInfo> field, Value value) {
	auto f = field.get();
	if(!ObjectType::AddField(std::move(field), value))
		return false;

	return m_RevFields.insert({ value.AsInteger(), f->Name() }).second;
}

const char* CustomEnumType::ToString(Int value) const {
	return "Zebra";
}

