#include "CoreInterfaces.h"
#include "Interpreter.h"
#include "ArrayType.h"

using namespace Dynamix;

Value Enumerable::Filter(Interpreter& intr, Value predicate) {
	auto en = GetEnumerator();
	Value next;
	std::vector<Value> items;
	while (!(next = en->GetNextValue()).IsError() && intr.Eval(predicate.AsAstNode()).ToBoolean()) {
		items.push_back(next);
	}
	return ArrayType::Get()->CreateArray(std::move(items));
}
