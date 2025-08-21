#pragma once

#include <Value.h>
#include <vector>
#include <Interpreter.h>

using namespace Dynamix;

Value print(Interpreter&, std::vector<Value>& args);
Value println(Interpreter& intr, std::vector<Value>& args);

