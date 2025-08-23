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
		println("{},{}: '{}' {}", e.Info.Line, e.Info.Col, e.Info.Lexeme, e.Description);
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
		if (_strcmpi(text, "quit") == 0)
			break;

		auto n = p.Parse(text);
		if(n) {
			try {
				auto result = intr.Run(n.get());
				println("{}", result.ToString());
				nodes.push_back(move(n));
			}
			catch (RuntimeError err) {
				println("Runtime error ({}:{}): {}", err.Location().Line, err.Location().Col, err.Message());
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
	Parser p(t, true);
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

	char text[] = "123 hello 12 for if x 9.34 < > 77 !- \"this is a text\" while";
	auto code = "fn sqr(x) { if(x < 5) { x += 1 } return 2 + x * x; }\n"
		"fn main() { val a = sqr(10); return 0; }";

	auto root = p.Parse(code);
	if (p.HasErrors()) {
		ShowErrors(p);
	}
	else {
		println("{}", root->ToString());
		auto n = p.Parse("var x = 12; x + 3 * 4");
		auto result = n->Accept(&intr);
		print("Result: {}", result.ToString());

		n = p.Parse("fn sqr2(x) { if(x < 5) { return x * x; } else { return x * x * x; } } sqr2(4)");
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

		auto code2 = R"(
		fn f1(x) {
			fn f2(y) {
				return y + x;
			}
			return f2(x) + x * 2;
		}
		f1(6))";

		n = p.Parse(code2);
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

		auto code3 = R"(
var a = 5; println("Zebra!! {}+{}={}", a, 3, a + 3);)";

		n = p.Parse(code3);
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

		n = p.Parse("eval(\"2*8\");");
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

		n = p.Parse("var i = 1; while(i <= 10) { println(\"{}\", i); i += 1; }");
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}
		n = p.Parse("fn max(x, y) { if(x > 0 and y > 0) { println(\"both positive\"); } else { println(\"at least one non-positive\"); } } max(-10, 3);");
		if (n) {
			n->Accept(&intr);
		}

	}
	return 0;
}

