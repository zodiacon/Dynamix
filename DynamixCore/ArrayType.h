#pragma once

#include "ObjectType.h"
#include "Value.h"
#include "CoreInterfaces.h"

namespace Dynamix {
	class ArrayObject;
	class RuntimeObject;

	class ArrayType : public StaticObjectType {
	public:
		static ArrayType* Get();

		ArrayObject* CreateArray(std::vector<Value> args);

	private:
		ArrayType();
	};

	class ArrayObject : public RuntimeObject, public Enumerable, public IClonable, public ISliceable {
	public:
		ArrayObject(std::vector<Value> init);
		std::vector<Value>& Items() noexcept {
			return m_Items;
		}

		std::unique_ptr<IEnumerator> GetEnumerator() const override;
		RuntimeObject* Clone() const override;
		SliceObject const* Slice(Int start, Int count) const override;

		void* QueryService(ServiceId id) override;

		Int Count() const {
			return Int(m_Items.size());
		}

		Int Add(Value item);
		Int RemoveAt(Int index);
		Int Insert(Int index, Value item);
		void Clear() {
			m_Items.clear();
		}
		void Resize(Int newsize) {
			m_Items.resize(newsize);
		}
		Bool IsEmpty() const {
			return Bool(m_Items.empty());
		}
		Int Append(Value list);

		std::string ToString() const override;

		Value InvokeIndexer(Value const& index) override;
		void AssignIndexer(Value const& index, Value const& value, TokenType assign) override;

		void Reverse();
		void ForEach(AstNode const* code);

	protected:
		Int ValidateIndex(Int index) const;

	private:
		std::vector<Value> m_Items;

	};
}
