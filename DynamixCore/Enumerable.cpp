#include "CoreInterfaces.h"
#include "Interpreter.h"
#include "ArrayType.h"

using namespace Dynamix;

Value Enumerable::Filter(Interpreter& intr, Value predicate) {
	auto en = GetEnumerator();
	Value next;
	auto node = predicate.AsAstNode();
	FunctionEssentials const* func = nullptr;
	switch (node->NodeType()) {
		case AstNodeType::AnonymousFunction:
			func = static_cast<FunctionEssentials const*>(reinterpret_cast<AnonymousFunctionExpression const*>(node));
			break;
		case AstNodeType::FunctionDeclaration:
			func = static_cast<FunctionEssentials const*>(reinterpret_cast<FunctionDeclaration const*>(node));
			break;
		default:
			assert(false);
			break;
	}
	assert(func);
	auto& params = func->Parameters();
	std::vector<Value> items;
	while (!(next = en->GetNextValue()).IsError()) {
		Scoper scoper(&intr);
		intr.CurrentScope().AddElement(params[0].Name, Element{ next, ElementFlags::Const });
		if(intr.Invoke(node).ToBoolean()) {
			items.push_back(next);
		}
	}
	return ArrayType::Get()->CreateArray(std::move(items));
}
