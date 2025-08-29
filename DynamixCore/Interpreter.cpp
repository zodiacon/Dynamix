#include <cassert>
#include <format>

#include "Interpreter.h"
#include "AstNode.h"
#include "Parser.h"
#include "Runtime.h"
#include "SymbolTable.h"
#include "ArrayType.h"

using namespace Dynamix;
using namespace std;

Interpreter::Interpreter(Parser& p, Runtime* rt) : m_Parser(p), m_Runtime(rt) {
	m_Scopes.push(make_unique<Scope>());    // global scope
	for (auto sym : p.GlobalSymbols()) {
		if (sym->Type == SymbolType::NativeFunction) {
			Variable v;
			v.Flags = VariableFlags::NativeFunction;
			v.VarValue = sym->Code;
			CurrentScope()->AddVariable(sym->Name, move(v));
		}
	}
}

Value Interpreter::Eval(AstNode const* root) {
	return root ? root->Accept(this) : Value();
}

Value Interpreter::VisitLiteral(LiteralExpression const* expr) {
	return Value::FromToken(expr->Literal());
}

Value Interpreter::VisitBinary(BinaryExpression const* expr) {
	auto left = Eval(expr->Left());
	switch (expr->Operator().Type) {
		case TokenType::And:
			if (!left.ToBoolean())
				return false;
			break;
		case TokenType::Or:
			if (left.ToBoolean())
				return true;
			break;
	}

	return left.BinaryOperator(expr->Operator().Type, Eval(expr->Right()));
}

Value Interpreter::VisitUnary(UnaryExpression const* expr) {
	return Eval(expr->Arg()).UnaryOperator(expr->Operator().Type);
}

Value Interpreter::VisitName(NameExpression const* expr) {
	auto v = CurrentScope()->FindVariable(expr->Name());
	if (v) {
		return v->VarValue;
	}
	auto sym = expr->Symbols()->FindSymbol(expr->Name());
	if (sym)
		return sym->Ast;
	throw RuntimeError(RuntimeErrorType::UnknownIdentifier, "Unknown identifier", expr->Location());

}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
	return Value();
}

Value Interpreter::VisitVar(VarValStatement const* expr) {
	if (CurrentScope()->FindVariable(expr->Name(), true))
		return Value::Error(ValueErrorType::DuplicateName);

	Variable v;
	if (expr->Init())
		v.VarValue = Eval(expr->Init());

	CurrentScope()->AddVariable(expr->Name(), move(v));
	return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
	// assume a single variable for now
	assert(expr->Left()->Type() == AstNodeType::Name);
	auto name = reinterpret_cast<NameExpression const*>(expr->Left());

	auto v = CurrentScope()->FindVariable(name->Name());
	if (!v)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: {}", name->Name()), expr->Left()->Location());

	return v->VarValue.Assign(Eval(expr->Value()), expr->AssignType().Type);
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
	auto f = Eval(expr->Callable());
	NativeFunction native = nullptr;
	AstNode const* node = nullptr;
	RuntimeObject* instance = nullptr;
	if (f.IsNativeFunction())
		native = f.AsNativeCode();
	else if (f.IsAstNode())
		node = f.AsAstNode();
	else if (f.IsCallable()) {
		auto c = f.AsCallable();
		instance = c->Instance;
		native = c->Native;
		node = c->Node;
	}
	else
		throw RuntimeError(RuntimeErrorType::NonCallable, "Cannot be invoked", expr->Location());

	if (node) {
		auto decl = reinterpret_cast<FunctionDeclaration const*>(node);

		PushScope();
		if(instance)
			CurrentScope()->AddVariable("this", Variable{ instance });
		for (size_t i = 0; i < expr->Arguments().size(); i++) {
			Variable v{ expr->Arguments()[i]->Accept(this) };
			CurrentScope()->AddVariable(decl->Parameters()[i].Name, move(v));
		}
		try {
			auto result = Eval(decl->Body());
			PopScope();
			return result;
		}
		catch (ReturnStatementException const& ret) {
			PopScope();
			return ret.ReturnValue;
		}
		catch (BreakAllStatementException const&) {
		}
	}
	else {
		assert(native);

		std::vector<Value> args;
		args.reserve(expr->Arguments().size() + 1);
		if (instance)
			args.push_back(instance);
		for (auto& v : expr->Arguments())
			args.emplace_back(Eval(v.get()));
		return (*native)(*this, args);
	}
}

Value Interpreter::VisitWhile(WhileStatement const* stmt) {
	PushScope();
	while (Eval(stmt->Condition()).ToBoolean()) {
		try {
			Eval(stmt->Body());
		}
		catch (BreakStatementException const&) {
			break;
		}
		catch (ContinueStatementException const&) {
		}
	}
	PopScope();
	return Value();
}

Value Interpreter::VisitIfThenElse(IfThenElseExpression const* expr) {
	auto cond = expr->Condition()->Accept(this);
	if (cond.IsError())
		return cond;

	if (cond.ToBoolean())
		return expr->Then()->Accept(this);
	if (expr->Else())
		return expr->Else()->Accept(this);
	return Value();
}

Value Interpreter::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
	Variable v;
	v.VarValue = decl;
	CurrentScope()->AddVariable(decl->Name(), move(v));

	return Value();
}

Value Interpreter::VisitReturn(ReturnStatement const* decl) {
	auto ret = decl->ReturnValue()->Accept(this);
	throw ReturnStatementException{ ret };
}

Value Interpreter::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
	if (stmt->IsContinue())
		throw ContinueStatementException();
	else
		throw BreakStatementException();

}

Value Interpreter::VisitFor(ForStatement const* stmt) {
	PushScope();
	Eval(stmt->Init());
	while (Eval(stmt->While()).ToBoolean()) {
		try {
			if (stmt->Body()) {
				Eval(stmt->Body());
			}
		}
		catch (BreakStatementException) {
			break;
		}
		catch (ContinueStatementException) {
		}
		Eval(stmt->Inc());
	}
	PopScope();
	return Value();
}

Value Interpreter::VisitStatements(Statements const* stmts) {
	Value result;
	for (auto& stmt : stmts->Get()) {
		result = Eval(stmt.get());
	}
	return result;
}

Value Interpreter::VisitAnonymousFunction(AnonymousFunctionExpression const* func) {
	return Value();
}

Value Interpreter::VisitEnumDeclaration(EnumDeclaration const* decl) {
	return Value();
}

Value Interpreter::VisitExpressionStatement(ExpressionStatement const* expr) {
	return Eval(expr->Expr());
}

Value Interpreter::VisitArrayExpression(ArrayExpression const* expr) {
	auto& type = ArrayType::Get();
	std::vector<Value> values;
	values.reserve(expr->Items().size());
	for (auto& item : expr->Items())
		values.emplace_back(Eval(item.get()));

	return type.CreateArray(values);
}

Value Interpreter::VisitRepeat(RepeatStatement const* stmt) {
	auto times = Eval(stmt->Times()).ToInteger();
	for (; times > 0; --times) {
		try {
			Eval(stmt->Body());
		}
		catch (BreakStatementException const&) {
			break;
		}
		catch (ContinueStatementException const&) {
		}
	}
	return Value();
}

Scope* Interpreter::CurrentScope() {
	return m_Scopes.top().get();
}

void Interpreter::PushScope() {
	m_Scopes.push(make_unique<Scope>(m_Scopes.top().get()));
}

void Interpreter::PopScope() {
	m_Scopes.pop();
	assert(!m_Scopes.empty());
}

std::unique_ptr<AstNode> Interpreter::Parse(std::string_view code) const {
	return m_Parser.Parse(code);
}

Value Interpreter::VisitGetMember(GetMemberExpression const* expr) {
	auto value = Eval(expr->Left());
	if (!value.IsObject())
		throw RuntimeError(RuntimeErrorType::ObjectExpected, "Object expected", expr->Location());
	auto obj = value.AsObject();
	auto member = obj->Type().GetMember(expr->Member());
	if (!member)
		throw RuntimeError(RuntimeErrorType::UnknownMember, format("Unknown member {} of type {}", expr->Member(), obj->Type().Name()), expr->Location());
	
	auto method = (MethodInfo*)member;
	auto c = new Callable;
	c->Instance = obj;
	if ((method->Flags & MemberFlags::Native) == MemberFlags::Native)
		c->Native = method->Code.Native;
	else
		c->Node = method->Code.Node;
	return c;
}

Value Interpreter::VisitAccessArray(AccessArrayExpression const* expr) {
	auto index = Eval(expr->Index());
	auto value = Eval(expr->Left());
	return value.InvokeIndexer(index);
}
