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
		AstNode* Node;
		NativeCode Native;
	};

	struct MemberInfo {
		std::string Name;
		MemberVisibility Visibility;
		MemberType Type;
		MemberFlags Flags;
	};

	struct MethodInfo : MemberInfo {
		MemberCode Code;
	};

	struct PropertuInfo : MemberInfo {
		MemberCode Set;
		MemberCode Get;
	};

	class ObjectType : MemberInfo {
	public:
		ObjectType(Runtime& rt, std::string name);

		std::string const& Name() const {
			return m_Name;
		}

		RuntimeObject* CreateObject(std::vector<Value>& args);
		// instance 
		Value Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags);
		// static
		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags);

		void DestroyObject(RuntimeObject* instance);

		unsigned GetObjectCount() const;

		bool AddMember(std::unique_ptr<MemberInfo> member);
		MemberInfo const* GetMember(std::string const& name) const;

	private:
		std::atomic<unsigned> m_ObjectCount{ 0 };
		std::unordered_map<std::string, std::unique_ptr<MemberInfo>> m_Members;
		std::string m_Name;
		Runtime& m_Runtime;
	};
}
