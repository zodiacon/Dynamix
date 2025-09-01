#include "Runtime.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include <cassert>
#include "Interpreter.h"
#include "RuntimeObject.h"

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
		if (!m->Parameters().empty() && m->Parameters()[0].Name != "this")
			mi->Flags = MemberFlags::Static;
		if (m->Name() == "new")
			mi->Flags = mi->Flags | MemberFlags::Ctor;
		mi->Code.Node = m->Body();
		for (auto& p : m->Parameters()) {
			mi->Parameters.emplace_back(MethodParameter{ p.Name, p.DefaultValue.get() });
		}
		type->AddMember(move(mi));
	}
	auto p = type.get();
	m_Types.insert({ decl->Name(), move(type) });
	return p;
}

Runtime::Runtime(Parser& parser) : m_Parser(parser) {
}

void Runtime::AddNativeFunctions() {
	extern void InitStdLibrary(Parser&);

	InitStdLibrary(m_Parser);
}

bool Runtime::Init() {
	if (!m_Parser.Init())
		return false;

	AddNativeFunctions();
	return true;
}


