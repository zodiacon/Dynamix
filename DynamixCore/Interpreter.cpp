#include <cassert>
#include <format>

#include "Interpreter.h"
#include "AstNode.h"
#include "Parser.h"
#include "Runtime.h"
#include "SymbolTable.h"

using namespace Dynamix;
using namespace std;

Dynamix::Interpreter::Interpreter(Parser& p, Runtime* rt) : m_Parser(p), m_Runtime(rt) {
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
		v.VarValue = expr->Init()->Accept(this);

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

	return v->VarValue.Assign(expr->Value()->Accept(this), expr->AssignType().Type);
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
	auto f = Eval(expr->Callable());
	if (f.IsAstNode()) {
		auto decl = reinterpret_cast<FunctionDeclaration const*>(f.AsAstNode());

		PushScope();
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
			m_InLoop = 0;
			m_LoopAction = LoopAction::None;
			return ret.ReturnValue;
		}
	}

	if (!f.IsNativeFunction())
		throw RuntimeError(RuntimeErrorType::NonCallable, "Cannot be invoked", expr->Location());

	std::vector<Value> args;
	args.reserve(expr->Arguments().size());
	for (auto& v : expr->Arguments())
		args.emplace_back(v->Accept(this));
	return (*f.AsNativeCode())(*this, args);
}

Value Interpreter::VisitWhile(WhileStatement const* stmt) {
	m_InLoop++;
	PushScope();
	while (stmt->Condition()->Accept(this).ToBoolean()) {
		stmt->Body()->Accept(this);
		if (m_LoopAction == LoopAction::Break) {
			m_LoopAction = LoopAction::None;
			break;
		}
	}
	PopScope();
	m_InLoop--;
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
	m_LoopAction = stmt->IsContinue() ? LoopAction::Continue : LoopAction::Break;
	return Value();
}

Value Interpreter::VisitFor(ForStatement const* stmt) {
	PushScope();
	Eval(stmt->Init());
	m_InLoop++;
	while (Eval(stmt->While()).ToBoolean()) {
		PushScope();
		if (stmt->Body()) {
			Eval(stmt->Body());
			if (m_LoopAction == LoopAction::Break) {
				m_LoopAction = LoopAction::None;
				break;
			}
		}
		PopScope();
		if (stmt->Inc())
			stmt->Inc()->Accept(this);
	}
	m_InLoop--;
	PopScope();
	return Value();
}

Value Interpreter::VisitStatements(Statements const* stmts) {
	Value result;
	for (auto& stmt : stmts->Get()) {
		result = stmt->Accept(this);
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
	return expr->Expr()->Accept(this);
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
