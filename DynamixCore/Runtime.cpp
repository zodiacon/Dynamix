#include "Runtime.h"
#include "Parser.h"
#include <cstdlib>
#include <print>
#include "Interpreter.h"
#include "RuntimeObject.h"

using namespace Dynamix;

RuntimeError::RuntimeError(RuntimeErrorType type, std::string msg, CodeLocation location) :
	m_Type(type), m_Message(std::move(msg)), m_Location(std::move(location)) {
}

Runtime::Runtime(Parser& parser) : m_Parser(parser) {
}

void Runtime::AddNativeFunctions() {
	extern void InitStdLibrary(Parser&);

	InitStdLibrary(m_Parser);
}

bool Runtime::Init() {
	AddNativeFunctions();
	return true;
}


