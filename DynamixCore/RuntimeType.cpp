#include <thread>
#include <chrono>
#include <print>

#include "RuntimeType.h"
#include "TypeHelper.h"
#include "Runtime.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Tokenizer.h"

using namespace Dynamix;

RuntimeType* RuntimeType::Get() {
	static RuntimeType type;
	return &type;
}

void RuntimeType::Sleep(Int msec) {
	std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

Value RuntimeType::Eval(Interpreter& intr, std::vector<Value> const& args) {
	if (args.size() > 1)
		throw RuntimeError(RuntimeErrorType::TooManyArguments, "Too many arguments in the call to 'Eval' (expected: 1)", intr.Location());

	if (args.empty())
		return Value();

	thread_local Tokenizer tokenizer;
	thread_local Parser parser(tokenizer);
	auto node = parser.Parse(args[0].ToString(), true);
	if (!node)
		return Value::Error(ValueErrorType::Parse);

	return node->Accept(&intr);
}

Value RuntimeType::Ticks() {
	return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

void RuntimeType::DumpStats(Interpreter& intr) {
	auto& rt = intr.GetRuntime();
	auto types = rt.GetTypes();
	using std::println;
	println("Types: {}", types.size());
	for (auto& t : types) {
		println(" Name: {}, Objects: {}", t->Name(), t->GetObjectCount());
	}
}

Value RuntimeType::CreateObject(Interpreter& intr, std::vector<Value> const& args) {
	return Value();
}

RuntimeType::RuntimeType() : StaticObjectType("Runtime") {
	BEGIN_METHODS(RuntimeType)
		METHOD_STATIC(Sleep, 1, RuntimeType::Sleep(args[0].ToInteger()); return Value();),
		METHOD_STATIC(Eval, 1, return RuntimeType::Eval(intr, args);),
		METHOD_STATIC(Ticks, 0, return RuntimeType::Ticks();),
		METHOD_STATIC(DumpStats, 0, RuntimeType::DumpStats(intr); return Value();),
		METHOD_STATIC(CreateObject, -1, return RuntimeType::CreateObject(intr, args);),
		END_METHODS()
}
