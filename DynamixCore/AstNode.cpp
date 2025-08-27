#include "AstNode.h"
#include <format>

using namespace Dynamix;
using namespace std;

BinaryExpression::BinaryExpression(unique_ptr<Expression> left, Token op, unique_ptr<Expression> right)
	: m_Left(move(left)), m_Right(move(right)), m_Operator(move(op)) {
	m_Left->SetParent(this);
	m_Right->SetParent(this);
}

Value BinaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitBinary(this);
}

string BinaryExpression::ToString() const {
	return format("({} {} {})", m_Left->ToString(), m_Operator.Lexeme, m_Right->ToString());
}

Expression* BinaryExpression::Left() const noexcept {
	return m_Left.get();
}

Expression* BinaryExpression::Right() const noexcept {
	return m_Right.get();
}

LiteralExpression::LiteralExpression(Token token) : m_Token(move(token)) {
}

Value LiteralExpression::Accept(Visitor* visitor) const {
	return visitor->VisitLiteral(this);
}

string LiteralExpression::ToString() const {
	return m_Token.Lexeme;
}

Token const& LiteralExpression::Literal() const noexcept {
	return m_Token;
}

NameExpression::NameExpression(string name, string ns) : m_Name(move(name)), m_NameSpace(move(ns)) {
}

Value NameExpression::Accept(Visitor* visitor) const {
	return visitor->VisitName(this);
}

Token const& BinaryExpression::Operator() const noexcept {
	return m_Operator;
}

string const& NameExpression::Name() const noexcept {
	return m_Name;
}

string NameExpression::ToString() const {
	return m_Name;
}

UnaryExpression::UnaryExpression(Token op, unique_ptr<Expression> arg) : m_Arg(move(arg)), m_Operator(move(op)) {
	m_Arg->SetParent(this);
}

Value UnaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitUnary(this);
}

string UnaryExpression::ToString() const {
	return format("({}{})", m_Operator.Lexeme, m_Arg->ToString());
}

Token const& UnaryExpression::Operator() const noexcept {
	return m_Operator;
}

Expression* UnaryExpression::Arg() const noexcept {
	return m_Arg.get();
}

VarValStatement::VarValStatement(string name, bool isConst, unique_ptr<Expression> init)
	: m_Name(move(name)), m_Init(move(init)), m_IsConst(isConst) {
	if (m_Init)
		m_Init->SetParent(this);
}

Value VarValStatement::Accept(Visitor* visitor) const {
	return visitor->VisitVar(this);
}

string VarValStatement::ToString() const {
	return format("{} {} = {};", IsConst() ? "val" : "var", m_Name, Init() ? Init()->ToString() : "");
}

string const& VarValStatement::Name() const noexcept {
	return m_Name;
}

Expression const* VarValStatement::Init() const noexcept {
	return m_Init.get();
}

bool VarValStatement::IsConst() const noexcept {
	return m_IsConst;
}

AssignExpression::AssignExpression(unique_ptr<Expression> lhs, unique_ptr<Expression> rhs, Token assignType)
	: m_Left(move(lhs)), m_Expr(move(rhs)), m_AssignType(assignType) {
	m_Left->SetParent(this);
	m_Expr->SetParent(this);
}

Value AssignExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAssign(this);
}

Expression const* AssignExpression::Left() const noexcept {
	return m_Left.get();
}

Expression const* AssignExpression::Value() const noexcept {
	return m_Expr.get();
}

Token const& AssignExpression::AssignType() const noexcept {
	return m_AssignType;
}

string AssignExpression::ToString() const {
	return format("{} {} {}", Left()->ToString(), AssignType().Lexeme, Value()->ToString());
}

InvokeFunctionExpression::InvokeFunctionExpression(unique_ptr<Expression> callable, vector<unique_ptr<Expression>> args) :
	m_Callable(move(callable)), m_Arguments(move(args)) {
	for (auto& arg : m_Arguments)
		arg->SetParent(this);
}

Expression const* InvokeFunctionExpression::Callable() const {
	return m_Callable.get();
}

Value InvokeFunctionExpression::Accept(Visitor* visitor) const {
	return visitor->VisitInvokeFunction(this);
}

WhileStatement::WhileStatement(unique_ptr<Expression> condition, unique_ptr<Statements> body) :
	m_Condition(move(condition)), m_Body(move(body)) {
	m_Condition->SetParent(this);
	m_Body->SetParent(this);
}

Value WhileStatement::Accept(Visitor* visitor) const {
	return visitor->VisitWhile(this);
}

Expression const* WhileStatement::Condition() const noexcept {
	return m_Condition.get();
}

Statements const* WhileStatement::Body() const noexcept {
	return m_Body.get();
}

IfThenElseExpression::IfThenElseExpression(unique_ptr<Expression> condition, unique_ptr<Statement> thenExpr, unique_ptr<Statement> elseExpr) :
	m_Condition(move(condition)), m_Then(move(thenExpr)), m_Else(move(elseExpr)) {
	m_Condition->SetParent(this);
	m_Then->SetParent(this);
	if (m_Else)
		m_Else->SetParent(this);
}

Value IfThenElseExpression::Accept(Visitor* visitor) const {
	return visitor->VisitIfThenElse(this);
}

string IfThenElseExpression::ToString() const {
	auto ifThen = format("if ({}) {{\n {}\n}}", Condition()->ToString(), Then()->ToString());
	if (Else())
		ifThen += format("\nelse {{\n {} \n}}", Else()->ToString());
	return ifThen;
}

Expression const* IfThenElseExpression::Condition() const noexcept {
	return m_Condition.get();
}

Statement const* IfThenElseExpression::Then() const noexcept {
	return m_Then.get();
}

Statement const* IfThenElseExpression::Else() const noexcept {
	return m_Else.get();
}

FunctionDeclaration::FunctionDeclaration(string name) :	m_Name(move(name)) {
}

Value FunctionDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitFunctionDeclaration(this);
}

string const& FunctionDeclaration::Name() const noexcept {
	return m_Name;
}

vector<Parameter> const& FunctionDeclaration::Parameters() const noexcept {
	return m_Parameters;
}

void FunctionDeclaration::Parameters(vector<Parameter> params) noexcept {
	m_Parameters = move(params);
}

Expression const* FunctionDeclaration::Body() const noexcept {
	return m_Body.get();
}

void FunctionDeclaration::Body(std::unique_ptr<Expression> body) noexcept {
	m_Body = move(body);
}

std::string FunctionDeclaration::ToString() const {
	std::string params;
	for (auto& param : Parameters())
		params += format("{}, ", param.Name);
	auto decl = format("fn {} ({})\n ", Name(), params.substr(0, params.length() - 2));
	return decl + Body()->ToString();
}

ReturnStatement::ReturnStatement(unique_ptr<Expression> expr) : m_Expr(move(expr)) {
	if (m_Expr)
		m_Expr->SetParent(this);
}

Value ReturnStatement::Accept(Visitor* visitor) const {
	return visitor->VisitReturn(this);
}

Expression const* ReturnStatement::ReturnValue() const noexcept {
	return m_Expr.get();
}

std::string ReturnStatement::ToString() const {
	return format("return {};", ReturnValue() ? ReturnValue()->ToString() : "");
}

BreakOrContinueStatement::BreakOrContinueStatement(bool cont) : m_IsContinue(cont) {
}

Value BreakOrContinueStatement::Accept(Visitor* visitor) const {
	return visitor->VisitBreakContinue(this);
}

bool BreakOrContinueStatement::IsContinue() const noexcept {
	return m_IsContinue;
}

ForStatement::ForStatement(unique_ptr<Statement> init, unique_ptr<Expression> whileExpr, unique_ptr<Expression> incExpr, unique_ptr<Statements> body) :
	m_Init(move(init)), m_While(move(whileExpr)), m_Inc(move(incExpr)), m_Body(move(body)) {
}

Value ForStatement::Accept(Visitor* visitor) const {
	return visitor->VisitFor(this);
}

Statement const* ForStatement::Init() const noexcept {
	return m_Init.get();
}

Expression const* ForStatement::While() const noexcept {
	return m_While.get();
}

Expression const* ForStatement::Inc() const noexcept {
	return m_Inc.get();
}

Statements const* ForStatement::Body() const noexcept {
	return m_Body.get();
}

AnonymousFunctionExpression::AnonymousFunctionExpression(vector<Parameter> args, unique_ptr<Expression> body) :
	m_Args(move(args)), m_Body(move(body)) {
}

Value AnonymousFunctionExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAnonymousFunction(this);
}

vector<Parameter> const& AnonymousFunctionExpression::Args() const noexcept {
	return m_Args;
}

Expression const* AnonymousFunctionExpression::Body() const noexcept {
	return m_Body.get();
}

EnumDeclaration::EnumDeclaration(std::string name, std::unordered_map<std::string, long long> values) : m_Name(move(name)), m_Values(move(values)) {
}

std::unordered_map<std::string, long long> const& EnumDeclaration::Values() const noexcept {
	return m_Values;
}

Value EnumDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitEnumDeclaration(this);
}

std::string const& EnumDeclaration::Name() const noexcept {
	return m_Name;
}

vector<std::unique_ptr<Expression>> const& Dynamix::InvokeFunctionExpression::Arguments() const {
	return m_Arguments;
}

string InvokeFunctionExpression::ToString() const {
	string result;
	for (auto& arg : Arguments())
		result += arg->ToString() + ", ";
	if (result.empty())
		result = ", ";
	return format("{} ({})", m_Callable->ToString(), result.substr(0, result.length() - 2));
}

Value Statements::Accept(Visitor* visitor) const {
	return visitor->VisitStatements(this);
}

void Statements::Add(unique_ptr<Statement> stmt) {
	stmt->SetParent(this);
	m_Stmts.push_back(move(stmt));
}

Statement const* Statements::GetAt(int i) const {
	return i < 0 || i >= m_Stmts.size() ? nullptr : m_Stmts[i].get();
}

int Statements::Count() const {
	return static_cast<int>(m_Stmts.size());
}

vector<unique_ptr<Statement>> const& Statements::Get() const {
	return m_Stmts;
}

string Statements::ToString() const {
	std::string result;
	for (auto& s : m_Stmts) {
		result += s->ToString();
		result += "\n";
	}
	return result;
}

ExpressionStatement::ExpressionStatement(std::unique_ptr<Expression> expr, bool sc) : m_Expr(move(expr)), m_Semicolon(sc) {
	m_Expr->SetParent(this);
}

Value ExpressionStatement::Accept(Visitor* visitor) const {
	auto result = visitor->VisitExpressionStatement(this);
	return m_Semicolon ? Value() : result;
}

Expression const* ExpressionStatement::Expr() const {
	return m_Expr.get();
}

string ExpressionStatement::ToString() const {
	return Expr()->ToString();
}

ClassDeclaration::ClassDeclaration(std::unique_ptr<ObjectType> type) : m_ObjectType(move(type)) {
}
