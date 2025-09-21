#pragma once

#include <vector>

namespace Dynamix {
	class Value;
	class Interpreter;

	Value Print(Interpreter&, std::vector<Value>& args);
	Value PrintLine(Interpreter&, std::vector<Value>& args);
	Value Eval(Interpreter&, std::vector<Value>& args);
	Value Sleep(Interpreter&, std::vector<Value>& args);
}
