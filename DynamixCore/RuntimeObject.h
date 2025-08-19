#pragma once

#include <atomic>
#include <string_view>

#include "RuntimeObject.h"
#include "Value.h"

namespace Dynamix {
	class ObjectType;

	class RuntimeObject {
	public:
		explicit RuntimeObject(ObjectType& type);
		RuntimeObject(RuntimeObject const& other) = delete;
		RuntimeObject& operator=(RuntimeObject const& other) = delete;
		RuntimeObject(RuntimeObject&& other) = default;
		RuntimeObject& operator=(RuntimeObject&& other) = default;

		void Construct(Value* args, int count);
		void Destruct();

		ObjectType& Type() {
			return m_Type;
		}

		ObjectType const& Type() const{
			return m_Type;
		}

		std::string ToString() const;

		int AddRef();
		int Release();

		Value Invoke(std::string_view name, Value* args = nullptr, int count = 0);

	private:
		std::atomic<int> m_RefCount{ 0 };
		ObjectType& m_Type;
	};
}

