#include "Runtime.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include <cassert>
#include "Interpreter.h"
#include "RuntimeObject.h"
#include "EnumType.h"
#include "RangeType.h"
#include "StringType.h"
#include "MathType.h"
#include "ComplexType.h"
#include "ConsoleType.h"
#include "RuntimeType.h"
#ifdef _WIN32
#include <Windows.h>
extern "C" int WINAPI InitModule(Dynamix::Runtime&);
#endif

using namespace Dynamix;
using namespace std;

RuntimeError::RuntimeError(RuntimeErrorType type, std::string msg, CodeLocation location) :
	m_Type(type), m_Message(std::move(msg)), m_Location(std::move(location)) {
}

ObjectPtr<ObjectType> Runtime::BuildType(ClassDeclaration const* decl, Interpreter* intr) {
	ObjectType* baseType = nullptr;
	if (!decl->BaseName().empty()) {
		auto e = intr->CurrentScope().FindElement(decl->BaseName());
		if (e && ((e->Flags & ElementFlags::Class) == ElementFlags::Class))
			baseType = reinterpret_cast<ObjectType*>(e->VarValue.AsObject());
	}

	auto type = new ObjectType(decl->Name(), baseType);
	for (auto& m : decl->Methods()) {
		auto mi = std::make_unique<MethodInfo>(m->Name());
		mi->Arity = (int8_t)m->Parameters().size();
		mi->Flags = m->IsStatic() ? SymbolFlags::Static : SymbolFlags::None;
		if (m->Name() == "new")
			mi->Flags = mi->Flags | SymbolFlags::Ctor;
		mi->Code.Node = m->Body();
		for (auto& p : m->Parameters()) {
			mi->Parameters.emplace_back(MethodParameter{ p.Name, p.DefaultValue.get() });
		}
		type->AddMethod(move(mi));
	}

	auto addField = [&](auto vv) {
		auto fi = make_unique<FieldInfo>(vv->Name());
		fi->Flags = vv->Flags();
		fi->Init = vv->Init();
		auto f = fi.get();
		type->AddField(move(fi));
		if (f->IsStatic()) {
			assert(intr);
			type->SetStaticField(f->Name(), f->Init ? intr->Eval(f->Init) : Value());
		}
		};

	for (auto& f : decl->Fields()) {
		if (f->NodeType() == AstNodeType::VarValStatement) {
			auto vv = reinterpret_cast<VarValStatement const*>(f.get());
			addField(vv);
		}
		else {
			assert(f->NodeType() == AstNodeType::Statements);
			auto stmts = reinterpret_cast<Statements const*>(f.get());
			for (auto& s : stmts->Get()) {
				assert(s->NodeType() == AstNodeType::VarValStatement);
				auto vv = reinterpret_cast<VarValStatement const*>(s.get());
				addField(vv);
			}
		}
	}

	for (auto& t : decl->Types()) {
		type->AddType(BuildType(t.get(), intr));
	}
	RegisterType(type);
	return type;
}

ObjectPtr<ObjectType> Runtime::BuildEnum(EnumDeclaration const* decl) const {
	auto type = new CustomEnumType(decl->Name());
	for (auto& [name, value] : decl->Values()) {
		auto field = std::make_unique<FieldInfo>(name);
		field->Flags = SymbolFlags::Static;
		type->AddField(std::move(field), value);
	}
	return ObjectPtr<ObjectType>(type);
}

int Runtime::LoadModule(std::string_view file) {
#ifdef _WIN32
	auto hDll = ::LoadLibraryA(file.data());
	if (!hDll)
		return HRESULT_FROM_WIN32(::GetLastError());
	auto init = (decltype(InitModule)*)::GetProcAddress(hDll, "InitModule");
	if (!init)
		return HRESULT_FROM_WIN32(::GetLastError());
	int err = init(*this);
	if (err) {
		::FreeLibrary(hDll);
		return err;
	}
	return 0;
#endif
	return false;
}

Runtime& Runtime::AddCode(std::unique_ptr<Statements> code) noexcept {
	m_Code.push_back(move(code));
	return *this;
}

Runtime& Runtime::AddCode(std::vector<std::unique_ptr<Statements>> code) noexcept {
	auto count = m_Code.size();
	m_Code.resize(count + code.size());
	std::move(code.begin(), code.end(), m_Code.begin() + count);
	return *this;
}

std::vector<ObjectType*> Runtime::GetTypes() {
	return std::vector(m_Types.begin(), m_Types.end());
}

void Runtime::RegisterType(ObjectType* type) {
	assert(!m_Types.contains(type));
	m_GlobalScope.AddElement(type->Name(), Element{ static_cast<RuntimeObject*>(type), ElementFlags::Class });
	m_Types.insert(type);
}

void Runtime::RevokeType(ObjectType* type) {
	assert(m_Types.contains(type));
	m_Types.erase(type);
}

Runtime* Runtime::Get() {
	return s_Runtime;
}

void Runtime::ClearCode() {
	m_Code.clear();
}

Runtime::Runtime() {
	s_Runtime = this;
	InitStdLibrary();
}

void Runtime::InitStdLibrary() {

#define ADD_TYPE(name) RegisterType(name##Type::Get());// m_GlobalScope.AddElement(#name, Element{ static_cast<RuntimeObject*>(name##Type::Get()), ElementFlags::Class })
#define ADD_TYPE2(name, typeName) RegisterType(typeName::Get()); // m_GlobalScope.AddElement(#name, Element{ static_cast<RuntimeObject*>(typeName::Get()), ElementFlags::Class })

	ADD_TYPE(Range);
	ADD_TYPE2(String, StringTypeA);
	ADD_TYPE2(StringW, StringTypeW);
	ADD_TYPE(Enum);
	ADD_TYPE(Math);
	ADD_TYPE(Complex);
	ADD_TYPE(Console);
	ADD_TYPE(Runtime);
}

