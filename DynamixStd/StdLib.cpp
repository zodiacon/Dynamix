#include <SymbolTable.h>
#include <Parser.h>
#include <format>
#include "print.h"

using namespace Dynamix;


void InitStdLibrary(Parser& parser) {

	struct {
		const char* Name;
		NativeFunction Code;
		int Arity{ -1 };
	} functions[] = {
		{ "print", print },
		{ "println", println },
	};

	for (auto& f : functions) {
		Symbol s;
		s.Type = SymbolType::NativeFunction;
		s.Name = std::format("{}/{}", f.Name, (f.Arity < 0 ? "*" : std::to_string(f.Arity)));
		s.Code = f.Code;
		parser.AddSymbol(std::move(s));
	}
}
