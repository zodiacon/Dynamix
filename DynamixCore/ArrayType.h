#pragma once

#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Value.h"

namespace Dynamix {
	class ArrayObject;

	class ArrayType : public ObjectType {
	public:
		static ArrayType& Get();

		ArrayObject* CreateArray(std::vector<Value>& args);

	private:
		ArrayType();
	};

	class ArrayObject : public RuntimeObject {
	public:
		ArrayObject(std::vector<Value> init);
		std::vector<Value>& Items() noexcept {
			return m_Items;
		}

		std::string ToString() const override;

		Value InvokeIndexer(Value const& index) override;
		void AssignIndexer(Value const& index, Value const& value, TokenType assign) override;

	private:
		std::vector<Value> m_Items;
	};
}
