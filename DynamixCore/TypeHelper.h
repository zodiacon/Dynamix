#pragma once

#include "SymbolTable.h"

namespace Dynamix {
	struct MethodDef {
		const char* Name;
		int Arity;
		SymbolFlags Flags{ SymbolFlags::Native };
		NativeFunction Code;
	};

	struct FieldDef {
		const char* Name;
		Value TheValue;
		SymbolFlags Flags{ SymbolFlags::Static | SymbolFlags::Const };
	};
}

#define METHOD_EX(name, arity, flags, body)	\
{ #name, arity, flags,	\
[](auto& intr, auto& args) -> Value {	\
	auto isStatic = (flags & SymbolFlags::Static) == SymbolFlags::Static;	\
	assert(args.size() == arity + (isStatic ? 0 : 1));	\
	auto inst = isStatic ? nullptr : GetInstance<Type>(args[0]);	\
	body	\
	} }

#define METHOD(name, arity, body)	METHOD_EX(name, arity, SymbolFlags::Native, body)
#define CTOR(arity) { "new", arity, SymbolFlags::Native | SymbolFlags::Ctor }

#define BEGIN_METHODS(type)	\
using Type = type;	\
MethodDef methods[] {

#define BEGIN_FIELDS FieldDef fields[] {
#define FIELD(name, code) { #name, code }
#define END_FIELDS	\
    };	\
	for (auto& f : fields) {	\
		auto fi = std::make_unique<FieldInfo>(f.Name);	\
		fi->Flags = f.Flags;	\
		AddField(std::move(fi), f.TheValue);	\
	}


#define METHOD0(name)	\
{ #name, 0, \
[](auto& intr, auto& args) -> Value {	\
	assert(args.size() == 1);	\
	return static_cast<Type*>(args[0])->name();	\
	} },

#define METHOD1(name)	\
{ #name, 1, \
[](auto& intr, auto& args) -> Value {	\
	assert(args.size() == 2);	\
	return static_cast<Type*>(args[0])->name(args[1]);	\
	} },

#define END_METHODS()	};	\
	for (auto& m : methods) {	\
		auto mi = std::make_unique<MethodInfo>(m.Name);	\
		mi->Arity = m.Arity;	\
		mi->Code.Native = m.Code;	\
		mi->Flags = m.Flags;	\
		AddMethod(move(mi));	\
	}
