#include "Runtime.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include <cassert>
#include "Interpreter.h"
#include "RuntimeObject.h"
#include "print.h"

using namespace Dynamix;
using namespace std;

RuntimeError::RuntimeError(RuntimeErrorType type, std::string msg, CodeLocation location) :
	m_Type(type), m_Message(std::move(msg)), m_Location(std::move(location)) {
}

ObjectType* Runtime::GetObjectType(AstNode const* classNode) {
	assert(classNode->Type() == AstNodeType::Class);
	auto decl = reinterpret_cast<ClassDeclaration const*>(classNode);
	auto it = m_Types.find(decl->Name());
	if (it != m_Types.end())
		return it->second.get();

	//
	// create a new type
	//
	auto type = make_unique<ObjectType>(decl->Name());
	for (auto& m : decl->Methods()) {
		auto mi = std::make_unique<MethodInfo>(m->Name());
		mi->Arity = (int8_t)m->Parameters().size();
		if (m->Name() == "new")
			mi->Flags = MemberFlags::Ctor;
		mi->Code.Node = m->Body();
		for (auto& p : m->Parameters()) {
			mi->Parameters.emplace_back(MethodParameter{ p.Name, p.DefaultValue.get() });
		}
		type->AddMethod(move(mi));
	}

	for (auto& f : decl->Fields()) {
		if (f->Type() == AstNodeType::VarValStatement) {
			auto vv = reinterpret_cast<VarValStatement const*>(f.get());
			auto fi = make_unique<FieldInfo>(vv->Name());
			fi->Init = vv->Init();
			type->AddField(move(fi));
		}
		else {
			assert(f->Type() == AstNodeType::Statements);
			auto stmts = reinterpret_cast<Statements const*>(f.get());
			for (auto& s : stmts->Get()) {
				auto vv = reinterpret_cast<VarValStatement const*>(s.get());
				auto fi = make_unique<FieldInfo>(vv->Name());
				fi->Init = vv->Init();
				type->AddField(move(fi));
			}
		}
	}

	auto p = type.get();
	m_Types.insert({ decl->Name(), move(type) });

	return p;
}

Runtime::Runtime(Parser& parser) : m_Parser(parser) {
}

bool Runtime::Init() {
	if (!m_Parser.Init())
		return false;

	InitStdLibrary();

	return true;
}

void Runtime::InitStdLibrary() {
	struct {
		const char* Name;
		NativeFunction Code;
		SymbolFlags Flags{ SymbolFlags::VarArg };
		int Arity{ -1 };
	} functions[] = {
		{ "print", print },
		{ "println", println },
		{ "eval", eval, SymbolFlags::None, 1 },
	};

	for (auto& f : functions) {
		Symbol s;
		s.Type = SymbolType::NativeFunction;
		s.Name = f.Name;
		s.Flags = f.Flags;
		m_Parser.AddSymbol(std::move(s));
		Element v{ f.Code };
		v.Arity = f.Arity;
		m_GlobalScope.AddElement(f.Name, move(v));
	}
}

