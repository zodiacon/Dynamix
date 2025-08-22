#include "Runtime.h"
#include "Interfaces.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include "Interpreter.h"

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

