#pragma once

#include <vector>
#include "RuntimeObject.h"
#include "ObjectType.h"

namespace Dynamix {
	struct StructField {
		std::string Name;
		uint16_t Size;
		uint16_t Offset;
	};

	struct StructDesc {
		std::vector<StructField> Fields;
		uint16_t TotalSize;
	};

	class StructType : public ObjectType {
	public:
		StructType(std::string name, StructDesc desc) noexcept;

		StructDesc const& Desc() const noexcept {
			return m_Desc;
		}

	private:
		StructDesc m_Desc;
	};

	class StructObject : public RuntimeObject {
	public:
		explicit StructObject(StructType* type) noexcept;
		~StructObject();

		template<typename T>
		T& Object() {
			return *static_cast<T*>(m_Data);
		}

		template<typename T>
		T const& Object() const noexcept {
			return *static_cast<T*>(m_Data);
		}

		template<typename T>
		T const* Get() const noexcept {
			return static_cast<T const*>(&m_Data);
		}

		template<typename T>
		T* Get() noexcept {
			return static_cast<T*>(&m_Data);
		}

		Int Size() const noexcept {
			return m_Size;
		}

	private:
		void* m_Data;
		Int m_Size;
	};

	template<typename T>
	class StructObjectT : public RuntimeObject {
	public:
		StructObjectT(StructType* type) noexcept: RuntimeObject(type) {}
		StructObjectT(StructType* type, T value) noexcept : RuntimeObject(type), m_Data(std::move(value)) {}

		template<typename T>
		T& Object() noexcept {
			return m_Data;
		}

		template<typename T>
		T const& Object() const noexcept {
			return m_Data;
		}

	private:
		T m_Data;
	};
}
