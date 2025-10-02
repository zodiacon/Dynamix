#pragma once

#include "ObjectType.h"

namespace Dynamix {
	class SliceObject;

	class SliceType : public StaticObjectType {
	public:
		static SliceType* Get();

		SliceObject* CreateSlice(RuntimeObject* target, Value const& range) const;

	private:
		SliceType();
	};

	class SliceObject : public RuntimeObject {
	public:
		SliceObject(RuntimeObject* target, Int start, Int size) noexcept;
		~SliceObject() noexcept;

		std::string ToString() const override;

		Int Start() const noexcept {
			return m_Start;
		}
		Int Size() const noexcept {
			return m_Size;
		}
		RuntimeObject const* Target() const noexcept {
			return m_Target;
		}

		Value InvokeIndexer(Value const& index) override;
		void AssignIndexer(Value const& index, Value const& value, TokenType assign) override;

	private:
		RuntimeObject* m_Target;
		Int m_Start, m_Size;
	};
}
