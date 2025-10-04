// Dynamix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <print>
#include <string>
#include <iostream>
#include <cassert>
#include <AstNode.h>
#include <Interpreter.h>
#include <Parser.h>

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

bool ParseMetaCommand(std::string const& text, Parser& p, Interpreter& intr) {
	auto space = text.find_first_of(" \t\n");
	if (space == string::npos)
		space = text.length();

	auto word = text.substr(0, space);
	if (_stricmp(word.c_str(), "$loadfile") == 0) {
		auto node = p.ParseFile(text.substr(space + 1));
		if (!node) {
			ShowErrors(p);
			return false;
		}
		intr.Eval(node.get());
		intr.GetRuntime().AddCode(move(node));
		return true;
	}
	if (_stricmp(word.c_str(), "$loadmod") == 0) {
		auto err = intr.GetRuntime().LoadModule(text.substr(space + 1));
		if (err)
			println("Error loading module ({})", err);
		return err == 0;
	}
	if (_stricmp(word.c_str(), "$erase") == 0) {
		intr.GetRuntime().ClearCode();
		p.Clear();
		return true;
	}
	std::println("Unknown command");
	return false;
}

int RunRepl(Parser& p, Interpreter& intr) {
	std::string text;
	vector<unique_ptr<Statements>> program;
	for (;;) {
		print(">> ");
		getline(cin, text);
		if (text.empty())
			continue;

		if (_strcmpi(text.c_str(), "$quit") == 0)
			break;
		if (text[0] == '$') {
			ParseMetaCommand(text, p, intr);
			continue;
		}

		auto n = p.Parse(text, true);
		if (n) {
			try {
				auto result = intr.Eval(n.get());
				if (!result.IsNull())
					println("{}", result.ToString());
				program.push_back(move(n));
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
	Interpreter intr(rt);

	vector<unique_ptr<Statements>> program;
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
		else {
			program.push_back(move(code));
		}
	}

	if (error)
		return 0;

	Value result;
	for (auto& code : program) {
		result = intr.Eval(code.get());
	}

	rt.AddCode(move(program));

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

