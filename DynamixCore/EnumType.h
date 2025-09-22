#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class EnumType : public StaticObjectType {
	public:
		static EnumType* Get();

		static Value ToString(ObjectType* type, Int value);
		static Value Parse(ObjectType* type, Value name);

	protected:
		EnumType();
		std::unordered_map<Int, std::string> m_RevFields;
	};

	class CustomEnumType : public ObjectType {
	public:
		explicit CustomEnumType(std::string name);

		bool AddField(std::unique_ptr<FieldInfo> field, Value value) override;
		virtual const char* ToString(Int value) const;

	private:
		std::unordered_map<Int, std::string> m_RevFields;
	};
}

