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

	class SliceObject : public RuntimeObject, public IClonable, public Enumerable {
	public:
		SliceObject(RuntimeObject* target, Int start, Int size) noexcept;
		~SliceObject() noexcept;

		std::unique_ptr<IEnumerator> GetEnumerator() const override;
		RuntimeObject* Clone() const override;

		void* QueryService(ServiceId id) noexcept override;

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
		Value GetByIndex(Int index) const;

	private:
		struct Enumerator : IEnumerator {
			Enumerator(SliceObject const* slice);
			// Inherited via IEnumerator
			Value GetNextValue() override;

			Int m_Current, m_End;
		private:
			SliceObject const* m_Slice;
		};

		mutable RuntimeObject* m_Target;
		Int m_Start, m_Size;
	};
}
