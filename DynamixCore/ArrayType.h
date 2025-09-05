#pragma once

#include "ObjectType.h"
#include "RuntimeObject.h"
#include "Value.h"
#include "CoreInterfaces.h"

namespace Dynamix {
	class ArrayObject;

	class ArrayType : public ObjectType {
	public:
		static ArrayType& Get();

		ArrayObject* CreateArray(std::vector<Value>& args);

	private:
		ArrayType();
	};

	class ArrayObject : public RuntimeObject, public IEnumerable {
	public:
		ArrayObject(std::vector<Value> init);
		std::vector<Value>& Items() noexcept {
			return m_Items;
		}

		std::unique_ptr<IEnumerator> GetEnumerator() const override;

		Int Count() const {
			return Int(m_Items.size());
		}

		Int Add(Value item);
		Int RemoveAt(Int index);
		Int Insert(Int index, Value item);
		void Clear() {
			m_Items.clear();
		}
		Bool IsEmpty() const {
			return Bool(m_Items.empty());
		}
		Int Append(Value list);

		std::string ToString() const override;

		Value InvokeIndexer(Value const& index) override;
		void AssignIndexer(Value const& index, Value const& value, TokenType assign) override;

		ArrayObject* Clone() const;
		void Reverse();
		void ForEach(AstNode const* code);

	protected:
		Int ValidateIndex(Int index) const;

	private:
		std::vector<Value> m_Items;
	};
}
