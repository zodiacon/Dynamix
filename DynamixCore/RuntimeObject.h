#pragma once

#include <atomic>
#include <string_view>
#include <vector>

#include "Value.h"

namespace Dynamix {
	class ObjectType;
	class Interpreter;
	enum class InvokeFlags;

	class RuntimeObject {
	public:
		explicit RuntimeObject(ObjectType& type);
		RuntimeObject(RuntimeObject const& other) = delete;
		RuntimeObject& operator=(RuntimeObject const& other) = delete;
		RuntimeObject(RuntimeObject&& other) = default;
		RuntimeObject& operator=(RuntimeObject&& other) = default;

		void Construct(std::vector<Value>& args);
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

		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags);

	private:
		std::atomic<int> m_RefCount{ 0 };
		ObjectType& m_Type;
	};
}

