#include "DebugType.h"
#include "TypeHelper.h"
#include "Runtime.h"

#ifdef _WIN32
#include <Windows.h>
#endif

using namespace Dynamix;

DebugType* DebugType::Get() {
	static DebugType type;
	return &type;
}

bool DebugType::Assert(Value const& expr) {
	if (!expr.ToBoolean())
		throw AssertFailedException(expr);
	return true;
}

void DebugType::Break() {
#ifdef _WIN32
	::DebugBreak();
#endif
}

DebugType::DebugType() : StaticObjectType("Debug") {
	BEGIN_METHODS(DebugType)
		METHOD_STATIC(Assert, 1, return DebugType::Assert(args[0]);),
		METHOD_STATIC(Break, 0, DebugType::Break(); return Value();),
		END_METHODS()
}

