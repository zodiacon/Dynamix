#pragma once

#include "Value.h"

#include <atomic>
#include <unordered_map>
#include <string>

namespace Dynamix {
	class Interpreter;
	class Runtime;

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
	};

	using NativeCode = Value(*)(RuntimeObject*, std::vector<Value>&);

	union MemberCode {
		MemberCode() : Node(nullptr) {}
		AstNode* Node;
		NativeCode Native;
	};

	struct MemberInfo {
		MemberInfo(std::string name, MemberType type) : m_Name(std::move(name)), m_Type(type) {}
		std::string const& Name() const {
			return m_Name;
		}

		MemberType Type() const {
			return m_Type;
		}

		MemberVisibility Visibility;
		MemberFlags Flags;

	private:
		std::string m_Name;
		MemberType m_Type;
	};

	struct MethodInfo : MemberInfo {
		explicit MethodInfo(std::string name) : MemberInfo(name, MemberType::Method) {}

		MemberCode Code;
		int8_t Arity;
	};

	struct PropertyInfo : MemberInfo {
		explicit PropertyInfo(std::string name) : MemberInfo(name, MemberType::Property) {}
		MemberCode Set;
		MemberCode Get;
	};

	class ObjectType : public MemberInfo {
	public:
		explicit ObjectType(std::string name) : MemberInfo(name, MemberType::Class) {}

		ObjectType(ObjectType const&) = delete;
		ObjectType& operator=(ObjectType const&) = delete;
		ObjectType(ObjectType&&) = delete;
		ObjectType& operator=(ObjectType&&) = delete;

		virtual RuntimeObject* CreateObject(std::vector<Value>& args);
		virtual void DestroyObject(RuntimeObject* object);

		// instance 
		Value Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		Value Invoke(Interpreter& intr, Value& instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		// static
		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;

		unsigned GetObjectCount() const;

		bool AddMember(std::unique_ptr<MemberInfo> member);
		MemberInfo const* GetMember(std::string const& name) const;

	private:
		std::atomic<unsigned> m_ObjectCount{ 0 };
		std::unordered_map<std::string, std::unique_ptr<MemberInfo>> m_Members;
		std::string m_Name;
	};
}
