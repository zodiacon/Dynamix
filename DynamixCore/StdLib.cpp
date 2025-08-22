#include <format>
#include "print.h"
#include "Parser.h"
#include "SymbolTable.h"

using namespace Dynamix;

void InitStdLibrary(Parser& parser) {

	struct {
		const char* Name;
		NativeFunction Code;
		SymbolFlags Flags{ SymbolFlags::VarArg };
		int8_t Arity{ -1 };
	} functions[] = {
		{ "print", print },
		{ "println", println },
		{ "eval", eval, SymbolFlags::None, 1 },
	};

	for (auto& f : functions) {
		Symbol s;
		s.Type = SymbolType::NativeFunction;
		s.Name = f.Name;
		s.Arity = f.Arity;
		s.Code = f.Code;
		s.Flags = f.Flags;
		parser.AddSymbol(std::move(s));
	}
}
