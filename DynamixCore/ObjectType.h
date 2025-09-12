#pragma once

#include "Value.h"
#include "NoCopyMove.h"
#include <atomic>
#include <unordered_map>
#include <string>
#include "SymbolTable.h"
#include "RuntimeObject.h"

namespace Dynamix {
	class Interpreter;
	class Runtime;
	class Expression;
	struct Parameter;

	enum class MemberType : uint8_t {
		Field,
		Method,
		Property,
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

		bool IsStatic() const;

		MemberVisibility Visibility{ MemberVisibility::Public };
		SymbolFlags Flags{ SymbolFlags::None };

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
		Expression const* Init{};
	};

	class ObjectType : public RuntimeObject, public MemberInfo {
	public:
		explicit ObjectType(std::string name, ObjectType* base = nullptr);

		virtual RuntimeObject* CreateObject(Interpreter& intr, std::vector<Value> const& args);
		virtual void DestroyObject(RuntimeObject* object);

		// instance 
		Value Invoke(Interpreter& intr, RuntimeObject* instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		Value Invoke(Interpreter& intr, Value& instance, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;
		// static
		Value Invoke(Interpreter& intr, std::string_view name, std::vector<Value>& args, InvokeFlags flags) const;

		void RunClassConstructor(Interpreter& intr);

		unsigned GetObjectCount() const;

		bool AddField(std::unique_ptr<FieldInfo> field);
		bool AddMethod(std::unique_ptr<MethodInfo> method);
		FieldInfo const* GetField(std::string const& name) const;
		MethodInfo const* GetMethod(std::string const& name, int8_t arity = -1) const;
		MethodInfo const* GetClassConstructor() const;
		Value& GetStaticField(std::string const& name);
		void SetStaticField(std::string const& name, Value value);

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
		unsigned m_ObjectCount{ 0 };
		std::unordered_map<std::string, std::unique_ptr<FieldInfo>> m_Fields;
		std::unordered_map<std::string, std::unique_ptr<MethodInfo>> m_Methods;
		std::unordered_map<std::string, std::unique_ptr<MethodInfo>> m_Constructors;
		std::string m_Name;
		ObjectType* m_Base;
		bool m_ClassCtorRun{ false };
	};
}
