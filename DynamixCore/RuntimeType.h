#pragma once

#include <vector>
#include "ObjectType.h"

namespace Dynamix {
	class Interpreter;

	class RuntimeType : public StaticObjectType {
	public:
		static RuntimeType* Get();

		static void Sleep(Int msec);
		static Value Eval(Interpreter& intr, std::vector<Value> const& args);
		static Value Ticks();

	private:
		RuntimeType();
	};
}

