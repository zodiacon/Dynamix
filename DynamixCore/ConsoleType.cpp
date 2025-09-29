#include "ConsoleType.h"
#include "TypeHelper.h"
#include <print>
#include <vector>
#include <string>
#include <iostream>

using namespace Dynamix;

ConsoleType* ConsoleType::Get() {
	static ConsoleType type;
	return &type;
}

std::string Format(std::vector<Value> const& args) {
	std::string text;
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
			args.size() > 6 ? values[6] : empty,
			args.size() > 7 ? values[6] : empty,
			args.size() > 8 ? values[6] : empty,
			args.size() > 9 ? values[6] : empty
		);
		text = std::vformat(args[0].ToString().c_str(), fmt);
	}
	return text;
}

Value ConsoleType::Write(std::vector<Value> const& args) {
	auto text = Format(args);
	std::print("{}", text);
	return Int(text.length());
}

Value ConsoleType::WriteLine(std::vector<Value> const& args) {
	auto text = Format(args);
	std::println("{}", text);
	return Int(text.length());
}

Value ConsoleType::Error(std::vector<Value> const& args) {
	auto text = Format(args);
	std::print(stderr, "{}", text);
	return Int(text.length());
}

Value ConsoleType::ErrorLine(std::vector<Value> const& args) {
	auto text = Format(args);
	std::println(stderr, "{}", text);
	return Int(text.length());
}

Value ConsoleType::ReadLine() {
	std::string text;
	std::getline(std::cin, text);
	return text.c_str();
}

ConsoleType::ConsoleType() : StaticObjectType("Console") {
	BEGIN_METHODS(ConsoleType)
		METHOD_STATIC(Write, -1, return ConsoleType::Write(args);),
		METHOD_STATIC(WriteLine, -1, return ConsoleType::WriteLine(args);),
		METHOD_STATIC(Error, -1, return ConsoleType::Error(args);),
		METHOD_STATIC(ErrorLine, -1, return ConsoleType::ErrorLine(args);),
		METHOD_STATIC(ReadLine, 0, return ConsoleType::ReadLine();),
		END_METHODS()
}
