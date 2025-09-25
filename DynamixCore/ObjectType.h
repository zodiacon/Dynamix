#pragma once

#include "Value.h"
#include "NoCopyMove.h"
#include <atomic>
#include <unordered_map>
#include <string>
#include "SymbolTable.h"
#include "RuntimeObject.h"
#include "ObjectPtr.h"

namespace Dynamix {
	class Interpreter;
	class Runtime;
	class Expression;
	struct Parameter;
	class Scope;

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
		virtual void DestroyObject(RuntimeObject const* object) const;
		bool IsObjectType() const override {
			return true;
		}

		// instance 
		Value Invoke(Interpreter& intr, RuntimeObject* instance, std::string const& name, std::vector<Value>& args, InvokeFlags flags) const;
		// static
		Value Invoke(Interpreter& intr, std::string const& name, std::vector<Value>& args, InvokeFlags flags) const;

		void RunClassConstructor(Interpreter& intr);

		unsigned GetObjectCount() const;

		virtual bool AddField(std::unique_ptr<FieldInfo> field, Value value = Value());
		bool AddMethod(std::unique_ptr<MethodInfo> method);
		bool AddType(ObjectPtr<ObjectType> type);

		FieldInfo const* GetField(std::string const& name) const noexcept;
		MethodInfo const* GetMethod(std::string const& name, int8_t arity = -1) const noexcept;
		MethodInfo const* GetClassConstructor() const;
		MemberInfo const* GetMember(std::string const& name) const;

		void AddTypesToScope(Scope& scope);

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
		friend class RuntimeObject;
		void ObjectCreated(RuntimeObject* obj);
		void ObjectDestroyed(RuntimeObject* obj);

		mutable unsigned m_ObjectCount{ 0 };
		std::unordered_map<std::string, std::unique_ptr<FieldInfo>> m_Fields;
		std::unordered_map<std::string, std::unique_ptr<MethodInfo>> m_Methods;
		std::unordered_map<std::string, std::unique_ptr<MethodInfo>> m_Constructors;
		std::unordered_map<std::string, ObjectPtr<ObjectType>> m_Types;
		std::unordered_map<std::string, MemberInfo*> m_Members;
		ObjectType* m_Base;
		bool m_ClassCtorRun{ false };
	};

	class StaticObjectType : public ObjectType {
	public:
		StaticObjectType(std::string name, ObjectType* baseType = nullptr);

		int AddRef() const noexcept override {
			return 2;
		}
		int Release() const noexcept override {
			return 1;
		}
	};
}

