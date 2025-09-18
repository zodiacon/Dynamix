#pragma once

#include "SymbolTable.h"

namespace Dynamix {
	struct MethodDef {
		const char* Name;
		int Arity;
		NativeFunction Code;
		SymbolFlags Flags{ SymbolFlags::Native };
	};
};

#define METHOD(name, arity, body)	\
{ #name, arity,	\
[](auto& intr, auto& args) -> Value {	\
	assert(args.size() == (arity + 1));	\
	auto inst = GetInstance<Type>(args[0]);	\
	body	\
	} },

#define BEGIN_METHODS(type)	\
using Type = type;	\
MethodDef methods[] {

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
