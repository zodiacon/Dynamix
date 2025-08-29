#pragma once

#include "ObjectType.h"
#include "RuntimeObject.h"

namespace Dynamix {
	class Value;
	class RuntimeArrayObject;

	class ArrayType : public ObjectType {
	public:
		static ArrayType& Get();

		RuntimeArrayObject* CreateArray(std::vector<Value>& args);

	private:
		ArrayType();
	};

	class RuntimeArrayObject : public RuntimeObject {
	public:
		RuntimeArrayObject(std::vector<Value> init);
		std::vector<Value>& Items() noexcept {
			return m_Items;
		}

		std::string ToString() const override;

		Value InvokeIndexer(Value const& index) override;

	private:
		std::vector<Value> m_Items;
	};
}
