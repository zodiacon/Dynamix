#pragma once

#include <vector>

namespace Dynamix {
	class Value;
	class Interpreter;
}

Dynamix::Value print(Dynamix::Interpreter&, std::vector<Dynamix::Value>& args);
Dynamix::Value println(Dynamix::Interpreter&, std::vector<Dynamix::Value>& args);
Dynamix::Value eval(Dynamix::Interpreter&, std::vector<Dynamix::Value>& args);

