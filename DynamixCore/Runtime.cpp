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

ObjectType* Runtime::GetObjectType(AstNode const* classNode, Interpreter* intr) {
	assert(classNode->Type() == AstNodeType::ClassDeclaration);
	auto decl = reinterpret_cast<ClassDeclaration const*>(classNode);
	auto type = BuildType(decl, intr);
	return type.Get();
}

ObjectPtr<ObjectType> Runtime::BuildType(ClassDeclaration const* decl, Interpreter* intr) const {
	auto type = new ObjectType(decl->Name());
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
		if (f->Type() == AstNodeType::VarValStatement) {
			auto vv = reinterpret_cast<VarValStatement const*>(f.get());
			addField(vv);
		}
		else {
			assert(f->Type() == AstNodeType::Statements);
			auto stmts = reinterpret_cast<Statements const*>(f.get());
			for (auto& s : stmts->Get()) {
				auto vv = reinterpret_cast<VarValStatement const*>(s.get());
				addField(vv);
			}
		}
	}

	for (auto& t : decl->Types()) {
		type->AddType(BuildType(t.get(), intr));
	}
	return type;
}

Runtime::Runtime(Parser& parser) : m_Parser(parser) {
	InitStdLibrary();
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

