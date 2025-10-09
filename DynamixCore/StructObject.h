#pragma once

#include <vector>
#include <map>
#include <cassert>

#include "RuntimeObject.h"
#include "ObjectType.h"

namespace Dynamix {
	struct StructField {
		std::string Name;
		ValueType Type;
		uint16_t Size;
		uint16_t Offset;
	};

	struct StructDesc {
		std::vector<StructField> Fields;
		uint16_t TotalSize;
	};

	Value ValueFromField(void const* data, StructField const* field);

	class StructType : public ObjectType {
	public:
		StructType(std::string name, StructDesc desc) noexcept;

		StructDesc const& Desc() const noexcept {
			return m_Desc;
		}

		bool HasField(std::string const& name) const noexcept;
		StructField const* GetStructField(std::string const& name) const noexcept;

	private:
		StructDesc m_Desc;
		std::map<std::string, int> m_FieldsMap;
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

		bool HasField(std::string const& name) const noexcept override {
			return Type()->HasField(name);
		}
		//void AssignField(std::string const& name, Value value, TokenType assignType = TokenType::Assign) override;
		//Value GetFieldValue(std::string const& name) const override;

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

		bool HasField(std::string const& name) const noexcept override {
			return Type()->HasField(name);
		}
		Value GetFieldValue(std::string const& name) const override {
			auto field = reinterpret_cast<StructType const*>(Type())->GetStructField(name);
			assert(field);
			return ValueFromField(&m_Data, field);
		}

	private:
		T m_Data;
	};
}
