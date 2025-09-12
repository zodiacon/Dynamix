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

Interpreter::Interpreter(Parser& p, Runtime& rt) : m_Parser(p), m_Runtime(rt) {
	m_Scopes.push(make_unique<Scope>(m_Runtime.GetGlobalScope()));    // global scope
}

Value Interpreter::Eval(AstNode const* root) {
	try {
		return root ? root->Accept(this) : Value();
	}
	catch (ReturnStatement const& ret) {
		return ret.ReturnValue();
	}
}

void Interpreter::RunConstructor(RuntimeObject* instance, MethodInfo const* ctor, std::vector<Value> const& args) {
	assert(ctor->Code.Node->Type() == AstNodeType::Statements);
	Scoper scoper(this);
	Element pThis{ instance };
	CurrentScope()->AddElement("this", move(pThis));
	int i = 0;
	for (auto& arg : args) {
		Element varg{ arg };
		CurrentScope()->AddElement(ctor->Parameters[i++].Name, move(varg));
	}
	Eval(ctor->Code.Node);
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
	auto elements = CurrentScope()->FindElements(expr->Name());
	if (elements.size() == 1) {
		return elements[0]->VarValue;
	}
	if (elements.size() > 1)
		throw RuntimeError(RuntimeErrorType::MultipleSymbols, format("Multiple symbols referring to: '{}'", expr->Name()), expr->Location());
	throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: '{}'", expr->Name()), expr->Location());

}

Value Interpreter::VisitVar(VarValStatement const* expr) {
	if (CurrentScope()->FindElement(expr->Name(), -1, true))
		return Value::Error(ValueErrorType::DuplicateName);

	Element v;
	if (expr->Init())
		v.VarValue = Eval(expr->Init());

	CurrentScope()->AddElement(expr->Name(), move(v));
	return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
	auto lhs = CurrentScope()->FindElement(expr->Lhs());
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
		auto func = CurrentScope()->FindElement(nameExpr->Name(), (int8_t)expr->Arguments().size());
		if (func == nullptr) {
			auto funcs = CurrentScope()->FindElements(nameExpr->Name());
			if (funcs.empty())
				throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown function: '{}'", nameExpr->Name()), expr->Location());
			string expected;
			for (auto& v : funcs)
				expected += format("{},", v->Arity);
			throw RuntimeError(RuntimeErrorType::WrongNumberArguments, format("Wrong number of arguments to '{}' (Expected: {})",
				nameExpr->Name(), expected.substr(0, expected.length() - 1)));
		}
		else {
			f = func->VarValue;
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

		if (!callable->Method.empty()) {
			Scoper scoper(this);
			vector<Value> args;
			for (auto& arg : expr->Arguments())
				args.push_back(Eval(arg.get()));

			try {
				return instance->Invoke(*this, callable->Method, args);
			}
			catch (ReturnStatementException const& ret) {
				return ret.ReturnValue;
			}
			catch (BreakoutStatementException const&) {
			}
		}
	}
	else
		throw RuntimeError(RuntimeErrorType::NonCallable, "Cannot be invoked", expr->Location());

	if (node) {
		assert(node->Type() == AstNodeType::FunctionDeclaration);
		auto decl = reinterpret_cast<FunctionDeclaration const*>(node);
		assert(!instance);
		Scoper scoper(this);
		for (size_t i = 0; i < expr->Arguments().size(); i++) {
			Element v{ Eval(expr->Arguments()[i].get()) };
			CurrentScope()->AddElement(decl->Parameters()[i].Name, move(v));
		}
		try {
			auto result = Eval(decl->Body());
			return result;
		}
		catch (ReturnStatementException const& ret) {
			return ret.ReturnValue;
		}
		catch (BreakoutStatementException const&) {
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
	Scoper scoper(this);
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
	Element v;
	v.VarValue = decl;
	v.Arity = (int8_t)decl->Parameters().size();
	CurrentScope()->AddElement(decl->Name(), move(v));

	return Value();
}

Value Interpreter::VisitReturn(ReturnStatement const* decl) {
	auto ret = decl->ReturnValue()->Accept(this);
	throw ReturnStatementException{ ret };
}

Value Interpreter::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
	switch (stmt->BreakType()) {
		case TokenType::Continue:
			throw ContinueStatementException();
		case TokenType::Break:
			throw BreakStatementException();
		case TokenType::BreakOut:
			throw BreakoutStatementException();
	}
	assert(false);
	return Value();
}

Value Interpreter::VisitFor(ForStatement const* stmt) {
	Scoper scoper(this);
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
	Element e{ decl, ElementFlags::Enum };
	CurrentScope()->AddElement(decl->Name(), move(e));

	return Value();
}

Value Interpreter::VisitExpressionStatement(ExpressionStatement const* expr) {
	return Eval(expr->Expr());
}

Value Interpreter::VisitArrayExpression(ArrayExpression const* expr) {
	auto type = ArrayType::Get();
	std::vector<Value> values;
	values.reserve(expr->Items().size());
	for (auto& item : expr->Items())
		values.emplace_back(Eval(item.get()));

	return type->CreateArray(values);
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
	FieldInfo const* field;
	RuntimeObject* obj = nullptr;
	ObjectType* type = nullptr;

	if (!value.IsObject())
		throw RuntimeError(RuntimeErrorType::ObjectExpected, "Object expected", expr->Location());
	obj = value.AsObject();
	type = obj->Type();
	field = type->GetField(expr->Member());

	if (field)
		return obj ? obj->GetField(field->Name()) : type->GetStaticField(field->Name());

	auto method = type->GetMethod(expr->Member());
	if (!method)
		throw RuntimeError(RuntimeErrorType::UnknownMember, format("Unknown member {} of type {}", expr->Member(), type->Name()), expr->Location());

	if (obj && method->IsStatic())
		throw RuntimeError(RuntimeErrorType::InvalidMemberAccess, format("Cannot access static method '{}' via instance",
			method->Name()), expr->Location());

	else if (!obj && !method->IsStatic())
		throw RuntimeError(RuntimeErrorType::InvalidMemberAccess, format("Cannot access instance method '{}' via class",
			method->Name()), expr->Location());

	auto c = new Callable;
	c->Instance = obj ? obj : type;
	c->Method = method->Name();
	return c;
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
	auto type = m_Runtime.GetObjectType(decl, this);
	Element v{ type, ElementFlags::Class };
	CurrentScope()->AddElement(decl->Name(), move(v));

	return Value();
}

Value Interpreter::VisitNewObjectExpression(NewObjectExpression const* expr) {
	auto v = CurrentScope()->FindElement(expr->ClassName());
	if (v == nullptr)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Class '{}' not found in scope", expr->ClassName()));

	if ((v->Flags & ElementFlags::Class) != ElementFlags::Class)
		throw RuntimeError(RuntimeErrorType::TypeMismatch, format("'{}' is not a class name in scope", expr->ClassName()));

	auto type = m_Runtime.GetObjectType(v->VarValue.AsAstNode());
	std::vector<Value> args;
	for (auto& arg : expr->Args())
		args.push_back(Eval(arg.get()));
	return type->CreateObject(*this, args);
}

Value Interpreter::VisitAssignField(AssignFieldExpression const* expr) {
	auto obj = Eval(expr->Lhs()->Left());
	assert(obj.IsObject());
	obj.AsObject()->AssignField(expr->Lhs()->Member(), Eval(expr->Value()), expr->AssignType().Type);
	return Eval(expr->Lhs());
}

Value Interpreter::VisitForEach(ForEachStatement const* stmt) {
	auto collection = Eval(stmt->Collection());
	// String to be dealt with later
	if (!collection.IsObject())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Expected collection in 'foreach' statement", stmt->Collection()->Location());

	auto enumerable = static_cast<IEnumerable*>(collection.AsObject()->QueryService(ServiceId::Enumerable));
	if (!enumerable)
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Object does not implement the Enumerable interface", stmt->Collection()->Location());

	Scoper scoper(this);
	Element e{};
	CurrentScope()->AddElement(stmt->Name(), e);
	auto enumerator = enumerable->GetEnumerator();
	Value next;
	auto index = CurrentScope()->FindElement(stmt->Name());
	assert(index);

	while (!(next = enumerator->GetNextValue()).IsError()) {
		index->VarValue = move(next);
		Eval(stmt->Body());
	}
	return Value();
}

Value Interpreter::VisitEnumValue(EnumValueExpression const* expr) {
	auto left = Eval(expr->Left());
	if (!left.IsAstNode())
		throw RuntimeError(RuntimeErrorType::Syntax, "Syntax error", expr->Location());

	auto node = left.AsAstNode();
	switch (node->Type()) {
		case AstNodeType::EnumDeclararion:
		{
			auto decl = reinterpret_cast<EnumDeclaration const*>(node);
			if (auto it = decl->Values().find(expr->Value().Lexeme); it != decl->Values().end())
				return it->second;
			throw RuntimeError(RuntimeErrorType::UnknownMember, format("Undefined enum element '{}'", expr->Value().Lexeme), expr->Location());
		}
		case AstNodeType::ClassDeclaration:
			return node;
	}

	throw RuntimeError(RuntimeErrorType::Syntax, "Unknown type", expr->Location());
}
