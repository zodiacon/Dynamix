#include <cassert>
#include <format>
#include <algorithm>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#include "Interpreter.h"
#include "AstNode.h"
#include "Parser.h"
#include "Runtime.h"
#include "SymbolTable.h"
#include "ArrayType.h"
#include "RangeType.h"

using namespace Dynamix;
using namespace std;

Interpreter::Interpreter(Runtime& rt) : m_Runtime(rt) {
	m_Scopes.push(Scope(m_Runtime.GetGlobalScope()));    // global scope

#ifdef _WIN32
	::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
#endif
}

Value Interpreter::Eval(AstNode const* root) {
	if (!root)
		return Value();

	m_CurrentNode = root;
	try {
		return root->Accept(this);
	}
	catch (ReturnStatement const& ret) {
		return ret.ReturnValue();
	}
}

void Interpreter::RunConstructor(RuntimeObject* instance, MethodInfo const* ctor, std::vector<Value> const& args) {
	assert(ctor->Code.Node->NodeType() == AstNodeType::Statements);
	Scoper scoper(this);
	Element pThis{ instance };
	CurrentScope().AddElement("this", move(pThis));
	int i = 0;
	for (auto& arg : args) {
		Element varg{ arg };
		CurrentScope().AddElement(ctor->Parameters[i++].Name, move(varg));
	}
	Eval(ctor->Code.Node);
}

Value Interpreter::VisitLiteral(LiteralExpression const* expr) {
	return expr->Literal();
}

Value Interpreter::VisitBinary(BinaryExpression const* expr) {
	auto left = Eval(expr->Left());
	switch (expr->Operator()) {
		case TokenType::And:
			if (!left.ToBoolean())
				return false;
			break;
		case TokenType::Or:
			if (left.ToBoolean())
				return true;
			break;
	}

	if (left.IsObject())
		return left.AsObject()->InvokeOperator(*this, expr->Operator(), Eval(expr->Right()));

	return left.BinaryOperator(expr->Operator(), Eval(expr->Right()));
}

Value Interpreter::VisitUnary(UnaryExpression const* expr) {
	return Eval(expr->Arg()).UnaryOperator(expr->Operator());
}

Value Interpreter::VisitName(NameExpression const* expr) {
	auto elements = CurrentScope().FindElements(expr->Name());
	if (elements.size() == 1) {
		return elements[0]->VarValue;
	}
	if (elements.size() > 1) {
		if (elements[0]->Flags == ElementFlags::None)
			return Value(expr->Name().c_str());
		throw RuntimeError(RuntimeErrorType::MultipleSymbols, format("Multiple symbols referring to: '{}'", expr->Name()), expr->Location());
	}
	auto e = CurrentScope().FindElementWithUse(expr->Name());
	if (e) {
		assert(e->VarValue.IsObjectType());
		GetMemberExpression gme(make_unique<NameExpression>(e->VarValue.AsObject()->Type()->Name(), ""), expr->Name(), TokenType::DoubleColon);
		return VisitGetMember(&gme);
	}
	throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: '{}'", expr->Name()), expr->Location());

}

Value Interpreter::VisitVar(VarValStatement const* expr) {
	if (CurrentScope().FindElement(expr->Name(), -1, true))
		return Value::Error(ValueErrorType::DuplicateName);

	Element v;
	if (expr->Init())
		v.VarValue = Eval(expr->Init());

	CurrentScope().AddElement(expr->Name(), move(v));
	return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
	auto lhs = CurrentScope().FindElement(expr->Lhs());
	if (!lhs)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown identifier: {}", expr->Lhs()), expr->Location());

	auto rhs = Eval(expr->Value());
	return lhs->VarValue.Assign(rhs, expr->AssignType());
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
	auto f = Eval(expr->Callable());

	std::vector<Value> args;
	args.reserve(expr->Arguments().size() + 1);
	for (auto& arg : expr->Arguments()) {
		args.emplace_back(Eval(arg.get()));
	}

	if (f.IsNativeFunction()) {
		return (*f.AsNativeCode())(*this, args);
	}
	AstNode const* node = nullptr;
	if (f.IsAstNode()) {
		node = f.AsAstNode();
	}
	else if (f.IsCallable()) {
		auto c = f.AsCallable();
		auto isStatic = (c->Flags & SymbolFlags::Static) == SymbolFlags::Static;
		try {
			return const_cast<RuntimeObject*>(c->Instance.Get())->Invoke(*this, c->Name, args, isStatic ? InvokeFlags::Static : InvokeFlags::Instance);
		}
		catch (ReturnStatementException const& ret) {
			return ret.ReturnValue;
		}
		catch (BreakoutStatementException const&) {
			return Value();
		}
	}
	else if (f.IsString()) {
		auto e = CurrentScope().FindElement(f.ToString(), (int8_t)expr->Arguments().size());
		if (e)
			f = e->VarValue;
		else
			throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Cannot find method '{}' with {} arguments", f.ToString(), expr->Arguments().size()));
		if (f.IsAstNode())
			node = f.AsAstNode();
	}
	if(node) {
		Scoper scoper(this);
		auto node = f.AsAstNode();
		FunctionEssentials const* decl;
		if (node->NodeType() == AstNodeType::FunctionDeclaration) {
			decl = static_cast<FunctionEssentials const*>(reinterpret_cast<FunctionDeclaration const*>(node));
		}
		else {
			assert(node->NodeType() == AstNodeType::AnonymousFunction);
			decl = static_cast<FunctionEssentials const*>(reinterpret_cast<AnonymousFunctionExpression const*>(node));
		}
		if (decl->Parameters().size() != expr->Arguments().size())
			throw RuntimeError(RuntimeErrorType::WrongNumberArguments,
				format("Wrong numnber of arguments. Expected: {}, Provided: {}", decl->Parameters().size(), expr->Arguments().size()), expr->Location());

		for (size_t i = 0; i < expr->Arguments().size(); i++) {
			Element v{ Eval(expr->Arguments()[i].get()) };
			CurrentScope().AddElement(decl->Parameters()[i].Name, move(v));
		}
		try {
			return Eval(decl->Body());
		}
		catch (ReturnStatementException const& ret) {
			return ret.ReturnValue;
		}
		catch (BreakoutStatementException const&) {
			return Value();
		}
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
	Element v{ decl };
	v.Arity = (int8_t)decl->Parameters().size();
	CurrentScope().AddElement(decl->Name(), v);

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
	return func;
}

Value Interpreter::VisitEnumDeclaration(EnumDeclaration const* decl) {
	if (CurrentScope().FindElement(decl->Name(), -1, true)) {
		throw RuntimeError(RuntimeErrorType::DuplicateDefinition, format("Type '{}' already defined in this scope", decl->Name()), decl->Location());
	}
	auto type = m_Runtime.BuildEnum(decl);
	Element e{ (RuntimeObject*)type.Get(), ElementFlags::Enum };
	CurrentScope().AddElement(decl->Name(), move(e));

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

Scope& Interpreter::CurrentScope() {
	return m_Scopes.top();
}

CodeLocation Interpreter::Location() const noexcept {
	return m_CurrentNode ? m_CurrentNode->Location() : CodeLocation();
}

Value Interpreter::Invoke(AstNode const* node, std::vector<Value> const* args) {
	Expression const* body;
	FunctionEssentials const* decl = nullptr;

	if (node->NodeType() == AstNodeType::FunctionDeclaration) {
		decl = static_cast<FunctionEssentials const*>(reinterpret_cast<FunctionDeclaration const*>(node));
	}
	else {
		assert(node->NodeType() == AstNodeType::AnonymousFunction);
		decl = static_cast<FunctionEssentials const*>(reinterpret_cast<AnonymousFunctionExpression const*>(node));
	}
	body = decl->Body();
	auto& params = decl->Parameters();
	Scoper scoper(this);
	if (args) {
		for (size_t i = 0; i < args->size(); i++) {
			Element v{ (*args)[i] };
			CurrentScope().AddElement(params[i].Name, move(v));
		}
	}

	try {
		return Eval(body);
	}
	catch (ReturnStatementException const& ret) {
		return ret.ReturnValue;
	}
	catch (BreakoutStatementException const&) {
	}
}

Value Interpreter::RunMain(int argc, const char* argv[], const char* envp[]) {
	AstNode const* main = nullptr;
	for (auto& node : m_Runtime.Code()) {
		if (node->NodeType() == AstNodeType::FunctionDeclaration) {
			auto decl = reinterpret_cast<FunctionDeclaration const*>(node.get());
			if (decl->Name() == "Main") {
				main = decl;
				break;
			}
		}
	}
	if (main) {
		std::vector<Value> args;
		std::vector<Value> items;
		items.reserve(argc);
		for (int i = 0; i < argc; i++)
			items.push_back(argv[i]);
		auto sargs = ArrayType::Get()->CreateArray(move(items));
		args.push_back(move(sargs));
		return Invoke(main, &args);
	}

	return Value::Error();
}

void Interpreter::PushScope() {
	if (m_Scopes.size() > 100)
		throw RuntimeError(RuntimeErrorType::StackOverflow, "Call stack is too deep");

	m_Scopes.push(Scope(&m_Scopes.top()));
}

void Interpreter::PopScope() {
	m_Scopes.pop();
	assert(!m_Scopes.empty());
}

Value Interpreter::VisitGetMember(GetMemberExpression const* expr) {
	auto value = Eval(expr->Left());
	auto type = value.GetObjectType();
	if (type == nullptr)
		throw RuntimeError(RuntimeErrorType::UnknownMember, format("Unknown member '{}'", expr->Member()), expr->Location());

	auto obj = value.ToObject();
	if (type->HasField(expr->Member()))
		return obj->GetFieldValue(expr->Member());

	auto c = new Callable;
	c->Instance = obj;
	c->Name = expr->Member();
	bool isStatic = expr->Operator() == TokenType::DoubleColon;
	c->Flags = isStatic ? SymbolFlags::Static : SymbolFlags::None;
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

	return arr.AssignArrayIndex(index, Eval(expr->Value()), expr->AssignType());
}

Value Interpreter::VisitClassDeclaration(ClassDeclaration const* decl) {
	auto type = m_Runtime.BuildType(decl, this);
	Element v{ type.Get(), ElementFlags::Class };
	auto name = decl->Name();
	if (decl->Parent())
		name = decl->Parent()->Name() + "::" + name;

	CurrentScope().AddElement(name, move(v));
	for (auto& t : decl->Types()) {
		VisitClassDeclaration(t.get());
	}
	return Value();
}

Value Interpreter::VisitNewObjectExpression(NewObjectExpression const* expr) {
	auto v = CurrentScope().FindElement(expr->ClassName());
	if (v == nullptr)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Class '{}' not found in scope", expr->ClassName()));

	if ((v->Flags & ElementFlags::Class) != ElementFlags::Class)
		throw RuntimeError(RuntimeErrorType::TypeMismatch, format("'{}' is not a class name in scope", expr->ClassName()));

	assert(v->VarValue.IsObject());
	auto type = reinterpret_cast<ObjectType*>(v->VarValue.AsObject());
	std::vector<Value> args;
	for (auto& arg : expr->Arguments())
		args.push_back(Eval(arg.get()));
	auto obj = type->CreateObject(*this, args);
	assert(obj);
	for (auto& init : expr->FieldInitializers()) {
		auto field = type->GetField(init.Name);
		if (field == nullptr)
			throw RuntimeError(RuntimeErrorType::UnknownMember,
				format("Field '{}' on type '{}' does not exist", init.Name, type->Name()), expr->Location());
		obj->AssignField(init.Name, Eval(init.Init.get()));
	}
	Value vobj(obj);
	obj->Release();
	return vobj;
}

Value Interpreter::VisitAssignField(AssignFieldExpression const* expr) {
	auto obj = Eval(expr->Lhs()->Left());
	assert(obj.IsObject());
	obj.AsObject()->AssignField(expr->Lhs()->Member(), Eval(expr->Value()), expr->AssignType());
	return Eval(expr->Lhs());
}

Value Interpreter::VisitForEach(ForEachStatement const* stmt) {
	auto collection = Eval(stmt->Collection());
	// String to be dealt with later
	if (!collection.IsObject())
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Expected collection in 'foreach' statement",
			stmt->Collection()->Location());

	auto enumerable = static_cast<IEnumerable*>(collection.AsObject()->QueryService(ServiceId::Enumerable));
	if (!enumerable)
		throw RuntimeError(RuntimeErrorType::TypeMismatch, "Object does not implement the Enumerable interface", stmt->Collection()->Location());

	Scoper scoper(this);
	Element e{};
	CurrentScope().AddElement(stmt->Name(), e);
	auto enumerator = enumerable->GetEnumerator();
	Value next;
	auto index = CurrentScope().FindElement(stmt->Name());
	assert(index);

	while (!(next = enumerator->GetNextValue()).IsError()) {
		index->VarValue = move(next);
		Eval(stmt->Body());
	}
	return Value();
}

Value Interpreter::VisitRange(RangeExpression const* expr) {
	auto start = Eval(expr->Start()).ToInteger();
	auto end = Eval(expr->End()).ToInteger();

	return RangeType::Get()->CreateRange(start, end + (expr->EndInclusive() ? 1 : 0));
}

Value Interpreter::VisitMatch(MatchExpression const* expr) {
	auto value = Eval(expr->ToMatch());

	auto def = expr->HasDefault() ?	&expr->MatchCases().back() : nullptr;

	for (auto& mc : expr->MatchCases()) {
		if (def == &mc)
			break;
		for (auto& c : mc.Cases()) {
			if (c->NodeType() == AstNodeType::AnonymousFunction) {
				auto af = reinterpret_cast<AnonymousFunctionExpression const*>(c.get());
				assert(af->Parameters().size() == 1);
				Scoper scoper(this);
				CurrentScope().AddElement(af->Parameters()[0].Name, Element{ value });
				if (Eval(af->Body()).ToBoolean()) {
					return Eval(mc.Action());
				}
			}
			else {
				auto cs = Eval(c.get());
				if (cs.Equal(value).ToBoolean()) {
					return Eval(mc.Action());
				}
			}
		}
	}

	if (def)
		return Eval(def->Action());
	return Value();
}

Value Interpreter::VisitUse(UseStatement const* use) {
	auto e = CurrentScope().FindElement(use->Name());
	if (e == nullptr)
		throw RuntimeError(RuntimeErrorType::UnknownIdentifier, format("Unknown name '{}'", use->Name()), use->Location());
	if ((e->Flags & ElementFlags::Class) == ElementFlags::None)
		throw RuntimeError(RuntimeErrorType::InvalidType, format("'{}' is not a class", use->Name()), use->Location());

	CurrentScope().AddUse(use->Name());

	return Value();
}

