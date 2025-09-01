#pragma once

#include "Value.h"

#include <atomic>
#include <unordered_map>
#include <string>

namespace Dynamix {
	class Interpreter;
	class Runtime;
	class Expression;
	struct Parameter;

	enum class InvokeFlags {
		None,
		GetProperty,
		SetProperty,
		GetField,
		SetField,
		Constructor,
	};

	enum class MemberType : uint8_t {
		Field,
		Method,
		Property,
		Constructor,
		Destructor,
		Operator,
		Class,
		Struct,
	};

	enum class MemberVisibility : uint8_t {
		Public,
		Module,
		Protected,
		Private,
	};

	enum class MemberFlags : uint16_t {
		None = 0,
		Static = 1,
		ReadOnly = 2,
		Const = 4,
		Native = 8,
		Ctor = 0x10,
	};

	union MemberCode {
		AstNode const* Node;
		NativeFunction Native;
	};

	struct MemberInfo {
		MemberInfo(std::string name, MemberType type) : m_Name(std::move(name)), m_Type(type) {}
		std::string const& Name() const {
			return m_Name;
		}

		MemberType Type() const {
			return m_Type;
		}

		MemberVisibility Visibility{ MemberVisibility::Public };
		MemberFlags Flags{ MemberFlags::None };

	private:
		std::string m_Name;
		MemberType m_Type;
	};

	struct MethodParameter {
		std::string Name;
		Expression* DefaultValue;
	};

	struct MethodInfo : MemberInfo {
		explicit MethodInfo(std::string name) : MemberInfo(name, MemberType::Method) {}

		MemberCode Code{};
		int8_t Arity{ 0 };
		std::vector<MethodParameter> Parameters;
	};

	struct FieldInfo : MemberInfo {
		explicit FieldInfo(std::string name) : MemberInfo(name, MemberType::Field) {}
		Expression const* Init;
	};

	class ObjectType : public MemberInfo {
	public:
		explicit ObjectType(std::string name) : MemberInfo(name, MemberType::Class) {}

		ObjectType(ObjectType const&) = delete;
		ObjectType& operator=(ObjectType const&) = delete;
		ObjectType(ObjectType&&) = delete;
		ObjectType& operator=(ObjectType&&) = delete;

		virtual RuntimeObject* CreateObject(std::vector<Value> const& args);
		virtual void DestroyObject(RuntimeObject* object);

		// instance 
		Value Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		Value Invoke(Interpreter& intr, Value& instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		// static
		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;

		unsigned GetObjectCount() const;

		bool AddMember(std::unique_ptr<MemberInfo> member);
		MemberInfo const* GetMember(std::string const& name) const;

		template<typename T>
		static T* GetInstance(RuntimeObject* obj) {
			return reinterpret_cast<T*>(obj);
		}

		template<typename T>
		static T const* GetInstance(Value const& v) {
			return reinterpret_cast<T const*>(v.AsObject());
		}

		template<typename T>
		static T* GetInstance(Value& v) {
			return reinterpret_cast<T*>(v.AsObject());
		}

	private:
		std::atomic<unsigned> m_ObjectCount{ 0 };
		std::unordered_map<std::string, std::unique_ptr<MemberInfo>> m_Members;
		std::string m_Name;
	};
}
