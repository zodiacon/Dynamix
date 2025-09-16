#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class RangeObject;

	class RangeType : public ObjectType {
	public:
		static RangeType* Get();

		RangeObject* CreateRange(Int start, Int end);

	private:
		RangeType() : ObjectType("Range") {}
	};

	class RangeObject : public RuntimeObject, public IEnumerable {
	public:
		RangeObject(Int start, Int end);

		void* QueryService(ServiceId id) override;

		std::unique_ptr<IEnumerator> GetEnumerator() const override;

		Int Start() const noexcept {
			return m_Start;
		}

		Int End() const noexcept {
			return m_End;
		}

	private:
		struct Enumerator : IEnumerator {
			Enumerator(Int start, Int end);
			// Inherited via IEnumerator
			Value GetNextValue() override;

			Int m_Current, m_End;

		};

		Int m_Start, m_End;

	};

}

