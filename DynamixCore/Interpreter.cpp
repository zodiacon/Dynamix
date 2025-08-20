#include <cassert>
#include <format>

#include "Interpreter.h"
#include "AstNode.h"
#include "Parser.h"

using namespace Dynamix;
using namespace std;

Dynamix::Interpreter::Interpreter(Parser& p) : m_Parser(p) {
    m_Scopes.push(make_unique<Scope>());    // global scope
}

Value Interpreter::VisitLiteral(LiteralExpression const* expr) {
    return Value::FromToken(expr->Literal());
}

Value Interpreter::VisitBinary(BinaryExpression const* expr) {
    return expr->Left()->Accept(this).BinaryOperator(expr->Operator().Type, expr->Right()->Accept(this));
}

Value Interpreter::VisitUnary(UnaryExpression const* expr) {
    return expr->Arg()->Accept(this).UnaryOperator(expr->Operator().Type);
}

Value Interpreter::VisitName(NameExpression const* expr) {
    auto v = CurrentScope()->FindVariable(expr->Name());
    if (!v)
        return Value::Error(ValueErrorType::UndefinedSymbol);

    return v->VarValue;
}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
    return Value();
}

Value Interpreter::VisitVar(VarValStatement const* expr) {
    if(CurrentScope()->FindVariable(expr->Name(), true))
        return Value::Error(ValueErrorType::DuplicateName);

    Variable v;
    if (expr->Init())
        v.VarValue = expr->Init()->Accept(this);

    CurrentScope()->AddVariable(expr->Name(), move(v));
    return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
    return Value();
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
    assert(expr->Symbols());
    assert(expr->Symbols()->Parent());
    auto f = expr->Symbols()->Parent()->FindSymbol(format("{}/{}", expr->Name(), expr->Arguments().size()));
    if (!f)
        return Value::Error(ValueErrorType::UndefinedSymbol);

    auto decl = reinterpret_cast<FunctionDeclaration*>(f->Ast);
    
    PushScope();
    for(size_t i = 0; i < decl->Parameters().size(); i++) {
        Variable v{ expr->Arguments()[i]->Accept(this) };
        CurrentScope()->AddVariable(decl->Parameters()[i].Name, move(v));
    }
    auto result = decl->Body()->Accept(this);
    PopScope();
    if (m_Return)
        m_Return = false;
    return result;
}

Value Interpreter::VisitWhile(WhileStatement const* stmt) {
    while (stmt->Condition()->Accept(this).ToBoolean()) {
        stmt->Body()->Accept(this);
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
    return Value();
}

Value Interpreter::VisitReturn(ReturnStatement const* decl) {
    m_ReturnValue = decl->ReturnValue()->Accept(this);
    m_Return = true;
    return m_ReturnValue;
}

Value Interpreter::VisitBreakContinue(BreakOrContinueStatement const* stmt) {
    return Value();
}

Value Interpreter::VisitFor(ForStatement const* stmt) {
    return Value();
}

Value Interpreter::VisitStatements(Statements const* stmts) {
    Value result;
    for (auto& stmt : stmts->Get()) {
        result = stmt->Accept(this);
        if (m_Return) {
            break;
        }
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
