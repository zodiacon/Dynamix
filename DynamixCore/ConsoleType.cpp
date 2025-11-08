#include <print>
#include <vector>
#include <string>
#include <iostream>
#include "ConsoleType.h"
#include "TypeHelper.h"
#include "Runtime.h"

using namespace Dynamix;

ConsoleType* ConsoleType::Get() {
	static ConsoleType type;
	return &type;
}

std::string Format(std::vector<Value> const& args) {
	std::string result;
	if (!args.empty()) {
		auto fmt = args[0].ToString();
		size_t start = 0;
		int i = 1;
		while (true) {
			auto next = fmt.find('{', start);
			if (next == std::string::npos)
				break;

			if(i == args.size())
				throw RuntimeError(RuntimeErrorType::TooFewArguments, "Not enough format specifiers");

			auto close = fmt.find('}', next + 1);
			if (close == std::string::npos)
				break;

			auto str = args[i++].ToString(fmt.substr(next, close - next + 1).c_str());
			result += fmt.substr(start, next - start) + str;
			start = close + 1;
		}
	}
	return result;
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
