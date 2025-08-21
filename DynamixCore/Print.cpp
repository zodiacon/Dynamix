#include <print>
//#include <format>
//#include <ostream>
//#include <sstream>
#include <ranges>

#include "print.h"
#include "Value.h"
#include "Interpreter.h"

using namespace Dynamix;

Value print(Interpreter&, std::vector<Value>& args) {
	if (!args.empty()) {
		static std::string empty;
		std::vector<std::string> values;
		values.reserve(args.size());
		for (auto& arg : args)
			values.emplace_back(arg.ToString());
		auto fmt = std::make_format_args(
			args.size() > 1 ? values[1] : empty,
			args.size() > 2 ? values[2] : empty,
			args.size() > 3 ? values[3] : empty,
			args.size() > 4 ? values[4] : empty,
			args.size() > 5 ? values[5] : empty,
			args.size() > 6 ? values[6] : empty
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
