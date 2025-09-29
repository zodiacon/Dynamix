#include <thread>
#include <chrono>

#include "RuntimeType.h"
#include "TypeHelper.h"
#include "Runtime.h"
#include "Interpreter.h"

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

	auto node = intr.Parse(args[0].ToString(), true);
	if (!node)
		return Value::Error(ValueErrorType::Parse);

	return node->Accept(&intr);
}

RuntimeType::RuntimeType() : StaticObjectType("Runtime") {
	BEGIN_METHODS(RuntimeType)
		METHOD_STATIC(Sleep, 1, RuntimeType::Sleep(args[0].ToInteger()); return Value();),
		METHOD_STATIC(Eval, 1, return RuntimeType::Eval(intr, args);),
		END_METHODS()
}