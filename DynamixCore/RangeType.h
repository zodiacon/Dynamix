#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class RangeObject;

	class RangeType : public ObjectType {
	public:
		static RangeType* Get();

		RangeObject* CreateRange(Int start, Int end);

	private:
		RangeType();
	};

	class RangeObject : public RuntimeObject, public IEnumerable, public IClonable {
	public:
		RangeObject(Int start, Int end);

		void* QueryService(ServiceId id) override;

		std::unique_ptr<IEnumerator> GetEnumerator() const override;

		Int Start() const noexcept {
			return m_Start;
		}
		void Start(Int start) noexcept {
			m_Start = start;
			if (m_Start > m_End)
				std::swap(m_Start, m_End);
		}
		Int End() const noexcept {
			return m_End;
		}
		void End(Int end) noexcept {
			m_End = end;
			if (m_Start > m_End)
				std::swap(m_Start, m_End);
		}

		void Shift(Int offset) noexcept {
			m_Start += offset;
			m_End += offset;
		}

		RuntimeObject* Clone() const override;

		Int Size() const noexcept {
			return m_End - m_Start;
		}

		std::string ToString() const override;

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

