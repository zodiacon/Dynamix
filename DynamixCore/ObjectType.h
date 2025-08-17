#pragma once

#include "Value.h"
#include "Runtime.h"

#include <atomic>

namespace Dynamix {
	class ObjectType {
	public:
		explicit ObjectType(Runtime& rt);
		ObjectType(ObjectType const&) = delete;
		ObjectType& operator=(ObjectType const&) = delete;

		RuntimeObject* CreateObject(Value* args = nullptr, int count = 0);
		// instance method
		Value Invoke(RuntimeObject* instance, std::string_view name, Value* args = nullptr, int count = 0);
		// static method
		Value Invoke(std::string_view name, Value* args = nullptr, int count = 0);

		void DestroyObject(RuntimeObject* instance);

		unsigned GetObjectCount() const;

	private:
		std::atomic<unsigned> m_ObjectCount{ 0 };
		Runtime& m_Runtime;
	};
}
