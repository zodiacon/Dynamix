#include "AstNode.h"
#include <format>

using namespace Dynamix;
using namespace std;

static size_t g_AstNodeCount;
static size_t g_TotalMemory;

void* AstNode::operator new(size_t size) {
	++g_AstNodeCount;
	g_TotalMemory += size;
	return ::operator new(size);
}

void AstNode::operator delete(void* p, size_t size) {
	--g_AstNodeCount;
	g_TotalMemory -= size;
	return ::operator delete(p);
}

BinaryExpression::BinaryExpression(unique_ptr<Expression> left, TokenType op, unique_ptr<Expression> right)
	: m_Left(move(left)), m_Right(move(right)), m_Operator(op) {
	m_Left->SetParent(this);
	m_Right->SetParent(this);
}

Value BinaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitBinary(this);
}

string BinaryExpression::ToString() const {
	return format("({} {} {})", m_Left->ToString(), Token::TypeToString(m_Operator), m_Right->ToString());
}

GetMemberExpression::GetMemberExpression(unique_ptr<Expression> left, string member, TokenType op) noexcept 
	: m_Left(move(left)), m_Member(move(member)), m_Operator(op) {
}

Expression const* GetMemberExpression::Left() const noexcept {
	return m_Left.get();
}

std::string const& GetMemberExpression::Member() const noexcept {
	return m_Member;
}

Value GetMemberExpression::Accept(Visitor* visitor) const {
	return visitor->VisitGetMember(this);
}

Expression* BinaryExpression::Left() const noexcept {
	return m_Left.get();
}

Expression* BinaryExpression::Right() const noexcept {
	return m_Right.get();
}

LiteralExpression::LiteralExpression(Value value) noexcept : m_Value(move(value)) {
}

Value LiteralExpression::Accept(Visitor* visitor) const {
	return visitor->VisitLiteral(this);
}

string LiteralExpression::ToString() const {
	return m_Value.ToString();
}

Value const& LiteralExpression::Literal() const noexcept {
	return m_Value;
}

NameExpression::NameExpression(string name, string ns) : m_Name(move(name)), m_NameSpace(move(ns)) {
}

Value NameExpression::Accept(Visitor* visitor) const {
	return visitor->VisitName(this);
}

TokenType BinaryExpression::Operator() const noexcept {
	return m_Operator;
}

string const& NameExpression::Name() const noexcept {
	return m_Name;
}

string NameExpression::ToString() const {
	return m_Name;
}

UnaryExpression::UnaryExpression(TokenType op, unique_ptr<Expression> arg) noexcept : m_Operator(op), m_Arg(move(arg)) {
	m_Arg->SetParent(this);
}

Value UnaryExpression::Accept(Visitor* visitor) const {
	return visitor->VisitUnary(this);
}

string UnaryExpression::ToString() const {
	return format("({}{})", Token::TypeToString(m_Operator), m_Arg->ToString());
}

TokenType UnaryExpression::Operator() const noexcept {
	return m_Operator;
}

Expression* UnaryExpression::Arg() const noexcept {
	return m_Arg.get();
}

VarValStatement::VarValStatement(string name, SymbolFlags flags, unique_ptr<Expression> init) noexcept
	: m_Name(move(name)), m_Init(move(init)), m_Flags(flags) {
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
	return (m_Flags & SymbolFlags::Const) == SymbolFlags::Const;
}

bool VarValStatement::IsStatic() const noexcept {
	return (m_Flags & SymbolFlags::Static) == SymbolFlags::Static;
}

AssignExpression::AssignExpression(string lhs, unique_ptr<Expression> rhs, TokenType assignType) noexcept
	: m_Lhs(move(lhs)), m_Value(move(rhs)), m_AssignType(assignType) {
	m_Value->SetParent(this);
}

Value AssignExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAssign(this);
}

AccessArrayExpression::AccessArrayExpression(unique_ptr<Expression> left, unique_ptr<Expression> index) noexcept : m_Left(move(left)), m_Index(move(index)) {
}

Value AccessArrayExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAccessArray(this);
}

Expression const* AccessArrayExpression::Left() const noexcept {
	return m_Left.get();
}

Expression const* AccessArrayExpression::Index() const noexcept {
	return m_Index.get();
}

string const& AssignExpression::Lhs() const noexcept {
	return m_Lhs;
}

Expression const* AssignExpression::Value() const noexcept {
	return m_Value.get();
}

TokenType AssignExpression::AssignType() const noexcept {
	return m_AssignType;
}

string AssignExpression::ToString() const {
	return format("{} {} {}", Lhs(), Token::TypeToString(AssignType()), Value()->ToString());
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

RepeatStatement::RepeatStatement(unique_ptr<Expression> times, unique_ptr<Statements> body) : m_Times(move(times)), m_Body(move(body)) {
}

Value RepeatStatement::Accept(Visitor* visitor) const {
	return visitor->VisitRepeat(this);
}

Expression const* RepeatStatement::Times() const noexcept {
	return m_Times.get();
}

Statements const* RepeatStatement::Body() const noexcept {
	return m_Body.get();
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

FunctionDeclaration::FunctionDeclaration(string name, bool method, bool isStatic) : m_Name(move(name)), m_Method(method), m_Static(isStatic) {
}

Value FunctionDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitFunctionDeclaration(this);
}

string const& FunctionDeclaration::Name() const noexcept {
	return m_Name;
}

void FunctionDeclaration::SetBody(std::unique_ptr<Expression> body) noexcept {
	m_Body = move(body);
	m_Body->SetParent(this);
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

BreakOrContinueStatement::BreakOrContinueStatement(TokenType type) : m_Type(type) {
}

Value BreakOrContinueStatement::Accept(Visitor* visitor) const {
	return visitor->VisitBreakContinue(this);
}

TokenType BreakOrContinueStatement::BreakType() const noexcept {
	return m_Type;
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

AnonymousFunctionExpression::AnonymousFunctionExpression(vector<Parameter> params, unique_ptr<Expression> body) {
	m_Parameters = move(params);
	m_Body = move(body);
}

Value AnonymousFunctionExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAnonymousFunction(this);
}

EnumDeclaration::EnumDeclaration(std::string name, std::unordered_map<std::string, long long> values) : m_Name(move(name)), m_Values(move(values)) {
}

unordered_map<std::string, long long> const& EnumDeclaration::Values() const noexcept {
	return m_Values;
}

Value EnumDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitEnumDeclaration(this);
}

string const& EnumDeclaration::Name() const noexcept {
	return m_Name;
}

vector<unique_ptr<Expression>> const& Dynamix::InvokeFunctionExpression::Arguments() const {
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

Statement const* Statements::Append(std::unique_ptr<Statements> stmts) {
	auto count = m_Stmts.size();
	m_Stmts.resize(m_Stmts.size() + stmts->Count());
	std::move(stmts->m_Stmts.begin(), stmts->m_Stmts.end(), m_Stmts.begin() + count);
	return m_Stmts.data()->get() + count;
}

Statement const* Statements::GetAt(int i) const {
	return i < 0 || i >= m_Stmts.size() ? nullptr : m_Stmts[i].get();
}

vector<unique_ptr<Statement>> const& Statements::Get() const {
	return m_Stmts;
}

unique_ptr<Statement> Statements::RemoveAt(int index) {
	auto stmt = move(m_Stmts[index]);
	m_Stmts.erase(m_Stmts.begin() + index);
	return stmt;
}

string Statements::ToString() const {
	std::string result;
	for (auto& s : m_Stmts) {
		result += s->ToString();
		result += "\n";
	}
	return result;
}

void ArrayExpression::Add(std::unique_ptr<Expression> expr) {
	m_Items.push_back(move(expr));
}

Value ArrayExpression::Accept(Visitor* visitor) const {
	return visitor->VisitArrayExpression(this);
}

vector<unique_ptr<Expression>> const& ArrayExpression::Items() const {
	return m_Items;
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

ClassDeclaration::ClassDeclaration(std::string name, ClassDeclaration const* parent) noexcept : m_Name(move(name)), m_Parent(parent) {
}

Value ClassDeclaration::Accept(Visitor* visitor) const {
	return visitor->VisitClassDeclaration(this);
}

void ClassDeclaration::AddInterface(std::string name) {
	m_Interfaces.push_back(move(name));
}

AssignArrayIndexExpression::AssignArrayIndexExpression(unique_ptr<Expression> arrayAccess, unique_ptr<Expression> rhs, TokenType assignType) noexcept 
: m_ArrayAccess(move(arrayAccess)), m_Value(move(rhs)), m_AssignType(assignType) {
}

Value AssignArrayIndexExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAssignArrayIndex(this);
}

Value NewObjectExpression::Accept(Visitor* visitor) const {
	return visitor->VisitNewObjectExpression(this);
}

AssignFieldExpression::AssignFieldExpression(unique_ptr<Expression> lhs, unique_ptr<Expression> rhs, TokenType assignType) noexcept 
	: m_Lhs(move(lhs)), m_Value(move(rhs)), m_AssignType(assignType) {
}

Value AssignFieldExpression::Accept(Visitor* visitor) const {
	return visitor->VisitAssignField(this);
}

ForEachStatement::ForEachStatement(string name, unique_ptr<Expression> collection, unique_ptr<Statements> body) noexcept 
	: m_Name(move(name)), m_Collection(move(collection)), m_Body(move(body)) {
	m_Body->SetParent(this);
	m_Collection->SetParent(this);
}

Value ForEachStatement::Accept(Visitor* visitor) const {
	return visitor->VisitForEach(this);
}

RangeExpression::RangeExpression(unique_ptr<Expression> start, unique_ptr<Expression> end, bool endInclusive) noexcept 
	: m_Start(move(start)), m_End(move(end)), m_EndInclusive(endInclusive) {
}

Value RangeExpression::Accept(Visitor* visitor) const {
	return visitor->VisitRange(this);
}

Value MatchExpression::Accept(Visitor* visitor) const {
	return visitor->VisitMatch(this);
}

Value UseStatement::Accept(Visitor* visitor) const {
	return visitor->VisitUse(this);
}

Value InterfaceDeclaration::Accept(Visitor* visitor) const {
	return Value();
}
