#include "CoreInterfaces.h"
#include "Interpreter.h"
#include "ArrayType.h"

using namespace Dynamix;

namespace {
	auto& GetParams(auto node) {
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
		return func->Parameters();
	}
}

Value Enumerable::Filter(Interpreter& intr, Value predicate) {
	auto en = GetEnumerator();
	auto node = predicate.AsAstNode();
	auto& params = GetParams(node);

	std::vector<Value> items;
	Value next;
	while (!(next = en->GetNextValue()).IsError()) {
		Scoper scoper(&intr);
		intr.CurrentScope().AddElement(params[0].Name, Element{ next, ElementFlags::Const });
		if(intr.Invoke(node).ToBoolean()) {
			items.push_back(next);
		}
	}
	return ArrayType::Get()->CreateArray(std::move(items));
}

Value Enumerable::Any(Interpreter& intr, Value predicate) {
	auto en = GetEnumerator();
	auto node = predicate.AsAstNode();
	auto& params = GetParams(node);

	Value next;
	while (!(next = en->GetNextValue()).IsError()) {
		Scoper scoper(&intr);
		intr.CurrentScope().AddElement(params[0].Name, Element{ next, ElementFlags::Const });
		if (intr.Invoke(node).ToBoolean()) {
			return true;
		}
	}
	return false;
}
