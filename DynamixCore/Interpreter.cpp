#include "Interpreter.h"
#include "AstNode.h"

using namespace Dynamix;
using namespace std;

Value Interpreter::VisitLiteral(LiteralExpression const* expr) {
    return Value::FromToken(expr->Literal());
}

Value Interpreter::VisitBinary(BinaryExpression const* expr) {
    return expr->Left()->Accept(this).BinaryOperator(expr->Operator().Type, expr->Right()->Accept(this));
}

Value Interpreter::VisitUnary(UnaryExpression const* expr) {
    return Value();
}

Value Interpreter::VisitName(NameExpression const* expr) {
    return Value();
}

Value Interpreter::VisitBlock(BlockExpression const* expr) {
    return Value();
}

Value Interpreter::VisitVar(VarValStatement const* expr) {
    return Value();
}

Value Interpreter::VisitAssign(AssignExpression const* expr) {
    return Value();
}

Value Interpreter::VisitInvokeFunction(InvokeFunctionExpression const* expr) {
    return Value();
}

Value Interpreter::VisitWhile(WhileStatement const* stmt) {
    return Value();
}

Value Interpreter::VisitIfThenElse(IfThenElseExpression const* expr) {
    return Value();
}

Value Interpreter::VisitFunctionDeclaration(FunctionDeclaration const* decl) {
    return Value();
}

Value Interpreter::VisitReturn(ReturnStatement const* decl) {
    return Value();
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
