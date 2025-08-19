#pragma once

#include "Visitor.h"
#include "Value.h"

namespace Dynamix {
	class Interpreter : public Visitor {
	public:
		// Inherited via Visitor
		Value VisitLiteral(LiteralExpression const* expr) override;
		Value VisitBinary(BinaryExpression const* expr) override;
		Value VisitUnary(UnaryExpression const* expr) override;
		Value VisitName(NameExpression const* expr) override;
		Value VisitBlock(BlockExpression const* expr) override;
		Value VisitVar(VarValStatement const* expr) override;
		Value VisitAssign(AssignExpression const* expr) override;
		Value VisitInvokeFunction(InvokeFunctionExpression const* expr) override;
		Value VisitWhile(WhileStatement const* stmt) override;
		Value VisitIfThenElse(IfThenElseExpression const* expr) override;
		Value VisitFunctionDeclaration(FunctionDeclaration const* decl) override;
		Value VisitReturn(ReturnStatement const* decl) override;
		Value VisitBreakContinue(BreakOrContinueStatement const* stmt) override;
		Value VisitFor(ForStatement const* stmt) override;
		Value VisitStatements(Statements const* stmts) override;
		Value VisitAnonymousFunction(AnonymousFunctionExpression const* func) override;
		Value VisitEnumDeclaration(EnumDeclaration const* decl) override;
		Value VisitExpressionStatement(ExpressionStatement const* expr) override;
	};
}
