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
	m_Scopes.push(make_unique<Scope>(m_Runtime ? m_Runtime->GetGlobalScope() : nullptr));    // global scope
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
	throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: '{}'", expr->Name()), expr->Location());

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
	auto lhs = CurrentScope()->FindVariable(expr->Lhs());
	if (!lhs)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: {}", expr->Lhs()), expr->Location());

	auto rhs = Eval(expr->Value());
	return lhs->VarValue.Assign(rhs, expr->AssignType().Type);
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
	NativeFunction native = nullptr;
	AstNode const* node = nullptr;
	RuntimeObject* instance = nullptr;
	Callable* callable = nullptr;
	Value f;
	if (expr->Callable()->Type() == AstNodeType::Name) {
		auto nameExpr = reinterpret_cast<NameExpression const*>(expr->Callable());
		auto v = CurrentScope()->FindVariable(format("{}/{}", nameExpr->Name(), expr->Arguments().size()));
		if (v) {
			f = v->VarValue;
		}
	}
	if (f.IsNull())
		f = Eval(expr->Callable());

	if (f.IsNativeFunction())
		native = f.AsNativeCode();
	else if (f.IsAstNode())
		node = f.AsAstNode();
	else if (f.IsCallable()) {
		callable = f.AsCallable();
		instance = callable->Instance;
		native = callable->Native;
		node = callable->Node;

		if (callable->Method) {
			PushScope();
			if (instance && (callable->Method->Flags & MemberFlags::Static) == MemberFlags::None)
				CurrentScope()->AddVariable("this", Variable{ instance });
			for (size_t i = 0; i < expr->Arguments().size(); i++) {
				Variable v{ Eval(expr->Arguments()[i].get()) };
				CurrentScope()->AddVariable(callable->Method->Parameters[i].Name, move(v));
			}
			try {
				auto result = Eval(node);
				PopScope();
				return result;
			}
			catch (ReturnStatementException const& ret) {
				PopScope();
				return ret.ReturnValue;
			}
			catch (BreakAllStatementException const&) {
				PopScope();
			}
		}
	}
	else
		throw RuntimeError(RuntimeErrorType::NonCallable, "Cannot be invoked", expr->Location());

	if (node) {
		auto decl = reinterpret_cast<FunctionDeclaration const*>(node);
		assert(!instance);
		PushScope();
		for (size_t i = 0; i < expr->Arguments().size(); i++) {
			Variable v{ Eval(expr->Arguments()[i].get()) };
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
			PopScope();
		}
	}
	else {
		assert(native);

		std::vector<Value> args;
		args.reserve(expr->Arguments().size() + 1);
		if (instance)
			args.push_back(instance);
		for (auto& arg : expr->Arguments()) {
			args.emplace_back(Eval(arg.get()));
		}
		return (*native)(*this, args);
	}
	return Value();
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
	CurrentScope()->AddVariable(format("{}/{}", decl->Name(), decl->Parameters().size()), move(v));

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
		if (stmt->Body()) {
			try {
				Eval(stmt->Body());
			}
			catch (BreakStatementException) {
				break;
			}
			catch (ContinueStatementException) {
			}
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

	switch (member->Type()) {
		case MemberType::Method:
		{
			auto method = (MethodInfo*)member;
			auto c = new Callable;
			c->Instance = obj;
			if ((method->Flags & MemberFlags::Native) == MemberFlags::Native)
				c->Native = method->Code.Native;
			else
				c->Node = method->Code.Node;
			c->Method = method;
			return c;
		}
		case MemberType::Field:
			return obj->GetField(member->Name());
	}
	assert(false);
	return Value();
}

Value Interpreter::VisitAccessArray(AccessArrayExpression const* expr) {
	auto index = Eval(expr->Index());
	auto value = Eval(expr->Left());
	return value.InvokeIndexer(index);
}

Value Interpreter::VisitAssignArrayIndex(AssignArrayIndexExpression const* expr) {
	auto arr = Eval(expr->ArrayAccess()->Left());
	auto index = Eval(expr->ArrayAccess()->Index());

	return arr.AssignArrayIndex(index, Eval(expr->Value()), expr->AssignType().Type);
}

Value Interpreter::VisitClassDeclaration(ClassDeclaration const* decl) {
	Variable v{ decl, VariableFlags::Class };
	CurrentScope()->AddVariable(decl->Name(), move(v));

	return Value();
}

Value Interpreter::VisitNewObjectExpression(NewObjectExpression const* expr) {
	auto v = CurrentScope()->FindVariable(expr->ClassName());
	if (v == nullptr)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Class '{}' not found in scope", expr->ClassName()));

	if ((v->Flags & VariableFlags::Class) != VariableFlags::Class)
		throw RuntimeError(RuntimeErrorType::TypeMismatch, format("'{}' is not a class name in scope", expr->ClassName()));

	auto type = m_Runtime->GetObjectType(v->VarValue.AsAstNode());
	std::vector<Value> args;
	for (auto& arg : expr->Args())
		args.push_back(Eval(arg.get()));
	return type->CreateObject(*this, args);
}

Value Interpreter::VisitAssignField(AssignFieldExpression const* expr) {
	auto obj = Eval(expr->Lhs()->Left());
	assert(obj.IsObject());
	obj.AsObject()->SetField(expr->Lhs()->Member(), Eval(expr->Value()));
	return expr->Lhs();
}
