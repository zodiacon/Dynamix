#pragma once

#include <string>
#include <unordered_map>

#include "Value.h"
#include "CoreInterfaces.h"
#include "NoCopyMove.h"

namespace Dynamix {
	class ObjectType;
	class Interpreter;

	enum class InvokeFlags {
		Method,
		GetProperty,
		SetProperty,
		GetField,
		SetField,
		Constructor,
	};

	class RuntimeObject : NoCopy, public IServices {
	public:
		explicit RuntimeObject(ObjectType* type);
		RuntimeObject(RuntimeObject&& other) = default;
		RuntimeObject& operator=(RuntimeObject&& other) = default;

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

		int AddRef();
		int Release();

		Value Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags = InvokeFlags::Method);
		virtual Value InvokeIndexer(Value const& index);
		virtual void AssignIndexer(Value const& index, Value const& value, TokenType assign);

	protected:
		std::unordered_map<std::string, Value> m_FieldValues;

	private:
		int m_RefCount{ 1 };
		ObjectType* m_Type;
	};
}

