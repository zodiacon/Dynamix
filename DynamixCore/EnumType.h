#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class EnumType : public StaticObjectType {
	public:
		static EnumType* Get();

		static Value ToString(ObjectType const* type, Int value);
		static Value Parse(ObjectType const* type, Value name);

	protected:
		EnumType();
		std::unordered_map<Int, std::string> m_RevFields;
	};

	class CustomEnumType : public ObjectType {
	public:
		explicit CustomEnumType(std::string name);

		bool AddField(std::unique_ptr<FieldInfo> field, Value value) override;
		Value ToString(Int value) const;
		Value Parse(const char* name) const;

	private:
		std::unordered_map<Int, std::string> m_RevFields;
	};
}

