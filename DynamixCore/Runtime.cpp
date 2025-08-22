#include "Runtime.h"
#include "Interfaces.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include "Interpreter.h"
#include "RuntimeObject.h"

using namespace Dynamix;

struct DefaultAllocator : Allocator {
	void* Allocate(size_t size) {
		return std::malloc(size);
	}

	void Free(void* p) {
		std::free(p);
	}
};

DefaultAllocator g_DefaultAllocator;

Runtime::Runtime(Parser& parser) : m_Parser(parser) {
	m_Allocator = &g_DefaultAllocator;
}

Allocator* Runtime::GetAllocator() const {
	return m_Allocator;
}

void Runtime::AddNativeFunctions() {
	extern void InitStdLibrary(Parser&);

	InitStdLibrary(m_Parser);
}

bool Runtime::Init() {
	AddNativeFunctions();
	return true;
}

void Runtime::DestroyObject(RuntimeObject* instance) {
	//instance->Type().RemoveObject(instance);
	instance->Destruct();
	instance->~RuntimeObject();
	GetAllocator()->Free(instance);
}

RuntimeObject* Runtime::CreateObject(ObjectType* type, std::vector<Value>& args) {
	auto obj = (RuntimeObject*)GetAllocator()->Allocate(sizeof(RuntimeObject));
	new (obj) RuntimeObject(*type);
	obj->Construct(args);
	// type->AddObject(obj);
	return obj;
}

RuntimeError::RuntimeError(RuntimeErrorType type, std::stacktrace trace) {
}
