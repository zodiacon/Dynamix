#pragma once

#include <string>
#include <unordered_map>

#include "Value.h"
#include "CoreInterfaces.h"
#include "NoCopyMove.h"
#include "ObjectPtr.h"

namespace Dynamix {
	class ObjectType;
	class Interpreter;

	enum class InvokeFlags {
		Instance = 0,
		Method = 1,
		GetProperty = 2,
		SetProperty = 3,
		GetField = 4,
		SetField = 5,
		Constructor = 0x10,
		Static = 0x20,
	};

	class RuntimeObject : NoCopy, public IServices {
	public:
		explicit RuntimeObject(ObjectType* type);
		RuntimeObject(RuntimeObject&& other) = default;
		RuntimeObject& operator=(RuntimeObject&& other) = default;
		virtual ~RuntimeObject() noexcept;

		virtual void Construct(std::vector<Value> const& args);
		virtual void Destruct();
		virtual bool IsObjectType() const {
			return false;
		}
		void AssignField(std::string const& name, Value value, TokenType assignType = TokenType::Assign);
		Value GetField(std::string const& name) const;

		ObjectType* Type() {
			return m_Type;
		}

		ObjectType const* Type() const{
			return m_Type;
		}

		virtual std::string ToString() const;

		virtual int AddRef() const noexcept;
		virtual int Release() const noexcept;

		Value Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags = InvokeFlags::Method);
		virtual Value InvokeIndexer(Value const& index);
		virtual void AssignIndexer(Value const& index, Value const& value, TokenType assign);

	protected:
		std::unordered_map<std::string, Value> m_FieldValues;

	private:
		mutable int m_RefCount{ 1 };
		ObjectType* m_Type;
	};

}

