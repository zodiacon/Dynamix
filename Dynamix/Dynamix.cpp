// Dynamix.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Parser.h>
#include <print>
#include <AstNode.h>
#include <Interpreter.h>

using namespace Dynamix;
using namespace std;

void ShowErrors(Parser const& p) {
	for (auto& e : p.Errors()) {
		println("{},{}: '{}' {}", e.Info.Line, e.Info.Col, e.Info.Lexeme, e.Description);
	}
}

int main() {
	Tokenizer t;
	Parser p(t, true);
	Runtime rt(p);
	rt.Init();
	char text[] = "123 hello 12 for if x 9.34 < > 77 !- \"this is a text\" while";
	auto code = "fn sqr(x) { if(x < 5) { x += 1 } return 2 + x * x; }\n"
		"fn main() { val a = sqr(10); return 0; }";

	//auto code = "fn main(args) { var x = 12; x = x + 1; return x + 2 * args + y; }";

	auto root = p.Parse(code);
	if (p.HasErrors()) {
		ShowErrors(p);
	}
	else {
		println("{}", root->ToString());
		auto n = p.Parse("var x = 12; x + 3 * 4");
		Interpreter intr(p, &rt);
		auto result = n->Accept(&intr);
		print("Result: {}", result.ToString());

		n = p.Parse("fn sqr2(x) { if(x < 5) { return x * x; } else { return x * x * x; } } sqr2(4)");
		if (n) {
			auto result = n->Accept(&intr);
			println ("Result: {}", result.ToString());
		}

		auto code2 = R"(
		fn f1(x) {
			fn f2(y) {
				return y + 1;
			}
			return f2(x) + x * 2;
		}
		f1(6))";

		n = p.Parse(code2);
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

		n = p.Parse(R"(
var a = 3; println("{}={}", 12, a + 1);)");
		if (n) {
			auto result = n->Accept(&intr);
			println("Result: {}", result.ToString());
		}

	}
	return 0;
}

