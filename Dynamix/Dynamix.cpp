// Dynamix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Parser.h>
#include <print>
#include <AstNode.h>
#include <Interpreter.h>

using namespace Dynamix;
using namespace std;

void ShowErrors(Parser const& p) {
	println("{} Errors:", p.Errors().size());
	for (auto& e : p.Errors()) {
		println("{},{}: {}", e.Location().Line, e.Location().Col, e.Description());
	}
}

int RunRepl(Parser& p, Interpreter& intr, std::unique_ptr<AstNode> node) {
	std::vector<std::unique_ptr<AstNode>> nodes;
	if(node)
		nodes.push_back(move(node));

	char text[256]{};
	for (;;) {
		print(">> ");
		gets_s(text);
		if (_strcmpi(text, "$quit") == 0)
			break;

		auto n = p.Parse(text, true);
		if(n) {
			try {
				auto result = intr.Eval(n.get());
				if(!result.IsNull())
					println("{}", result.ToString());
				nodes.push_back(move(n));
			}
			catch (RuntimeError err) {
				println("Runtime error: {}", err.Message());
			}
		}
		else {
			ShowErrors(p);
		}
	}
	return 0;
}

int main(int argc, const char* argv[]) {
	Tokenizer t;
	Parser p(t);
	Runtime rt(p);
	rt.Init();
	Interpreter intr(p, &rt);

	std::unique_ptr<AstNode> program;
	if (argc > 1) {
		program = p.ParseFile(argv[1]);
		if (program) {
			auto result = program->Accept(&intr);
			println("{}", result.ToString());
		}
		else {
			ShowErrors(p);
		}
	}

	return RunRepl(p, intr, move(program));
}

