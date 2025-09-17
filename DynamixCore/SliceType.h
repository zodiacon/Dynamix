#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class SliceType : public ObjectType {
	public:
		static SliceType* Get();

	private:
		SliceType();
	};

	class SliceObject : public RuntimeObject {
	public:
		SliceObject(RuntimeObject* target, Int start, Int count) noexcept;
		~SliceObject() noexcept;

		Int Start() const noexcept {
			return m_Start;
		}
		Int Count() const noexcept {
			return m_Count;
		}
		RuntimeObject* Target() const noexcept {
			return m_Target;
		}

	private:
		RuntimeObject* m_Target;
		Int m_Start, m_Count;
	};
}
