// Dynamix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Parser.h>
#include <print>
#include <AstNode.h>
#include <Interpreter.h>

using namespace Dynamix;
using namespace std;

void ShowErrors(Parser const& p, bool repl = false) {
	if (!repl)
		println("{} Errors:", p.Errors().size());
	for (auto& e : p.Errors()) {
		if (repl)
			println("{}", e.Description());
		else
			println("({},{}): {}", e.Location().Line, e.Location().Col, e.Description());
	}
}

int RunRepl(Parser& p, Interpreter& intr) {
	char text[256]{};
	for (;;) {
		print(">> ");
		gets_s(text);
		if (_strcmpi(text, "$quit") == 0)
			break;

		auto n = p.Parse(text, true);
		if (n) {
			try {
				auto result = intr.Eval(n);
				if (!result.IsNull())
					println("{}", result.ToString());
			}
			catch (RuntimeError err) {
				println("Runtime error: {}", err.Message());
			}
		}
		else {
			ShowErrors(p, true);
		}
	}
	return 0;
}

void Usage() {
	println("Dynamix v0.1");
	println("Usage: dynamix run <file> [file]...[-- params] (parse files and run Main function)");
	println("       dynamix load [file]...					(parse files and run REPL)");
}

int main(int argc, const char* argv[], const char* envp[]) {
	if (argc <= 1) {
		Usage();
		return 0;
	}

	enum Command {
		Invalid,
		Run,
		Load
	};

	auto cmd = Command::Invalid;

	if (_stricmp(argv[1], "run") == 0)
		cmd = Command::Run;
	else if (_stricmp(argv[1], "load") == 0)
		cmd = Command::Load;
	else {
		println("Unknown command: {}", argv[1]);
		Usage();
		return 1;
	}

	Tokenizer t;
	Parser p(t);
	Runtime rt;
	Interpreter intr(p, rt);

	std::unique_ptr<AstNode> program;
	bool error = false;
	int params = 0;
	for (int i = 2; i < argc; i++) {
		if (_stricmp(argv[i], "--") == 0) {
			params = i + 1;
			break;
		}
		auto code = p.ParseFile(argv[i]);
		if (!code) {
			ShowErrors(p);
			error = true;
		}
	}

	if (error)
		return 0;

	auto result = intr.Eval(p.Program());

	switch (cmd) {
		case Command::Load:
		{
			if (!result.IsNull())
				println("{}", result.ToString());
			return RunRepl(p, intr);
		}

		case Command::Run:
		{
			result = intr.RunMain(params > 0 ? argc - params : 0, params > 0 ? argv + params : nullptr, envp);
			if (!result.IsNull())
				println("{}", result.ToString());
			break;
		}
	}

	return 0;
}

