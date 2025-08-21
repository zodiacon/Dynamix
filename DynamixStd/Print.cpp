#include <Value.h>
#include <print>
#include <format>
#include <Interpreter.h>

using namespace Dynamix;

Value print(Interpreter&, std::vector<Value>& args) {
	if (!args.empty()) {
		std::vector<std::string> sargs;
		for (auto& a : args)
			sargs.emplace_back(a.ToString());

		static std::string empty;
		auto fmt = std::make_format_args(
			args.size() > 1 ? sargs[1] : empty,
			args.size() > 2 ? sargs[2] : empty,
			args.size() > 3 ? sargs[3] : empty,
			args.size() > 4 ? sargs[4] : empty
		);
		auto text = std::vformat(args[0].ToString().c_str(), fmt);
		std::print("{}", text);
	}
	return Value();
}

Value println(Interpreter& intr, std::vector<Value>& args) {
	print(intr, args);
	std::println();
	return Value();
}
