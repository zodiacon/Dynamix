#pragma once

#include "RuntimeObject.h"

namespace Dynamix {
	class StructObjectBase : public RuntimeObject {
	public:
		StructObjectBase(ObjectType* type, Int size) : RuntimeObject(type), m_Size(size), m_Data(::malloc(size)) {}

		template<typename T>
		T& Object() {
			return *static_cast<T*>(m_Data);
		}

		template<typename T>
		T const& Object() const {
			return *static_cast<T*>(m_Data);
		}

		Int Size() const {
			return m_Size;
		}

	private:
		void* m_Data;
		Int m_Size;
	};
}
