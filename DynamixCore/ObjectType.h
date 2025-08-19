#pragma once

#include "Value.h"
#include "Runtime.h"

#include <atomic>

namespace Dynamix {
	class ObjectType {
	public:
		ObjectType(Runtime& rt, std::string name);
		ObjectType(ObjectType const&) = delete;
		ObjectType& operator=(ObjectType const&) = delete;

		std::string const& Name() const {
			return m_Name;
		}

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
		std::string m_Name;
	};
}
