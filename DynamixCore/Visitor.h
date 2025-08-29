#pragma once

#include "Value.h"

namespace Dynamix {
	class LiteralExpression;
	class BinaryExpression;
	class UnaryExpression;
	class NameExpression;
	class BlockExpression;
	class VarValStatement;
	class InvokeFunctionExpression;
	class RepeatStatement;
	class WhileStatement;
	class IfThenElseExpression;
	class FunctionDeclaration;
	class AssignExpression;
	class ReturnStatement;
	class BreakOrContinueStatement;
	class ForStatement;
	class Statements;
	class AnonymousFunctionExpression;
	class EnumDeclaration;
	class ExpressionStatement;
	class ArrayExpression;
	class GetMemberExpression;
	class AccessArrayExpression;

	class Visitor {
	public:
		virtual ~Visitor() noexcept = default;

		virtual Value VisitLiteral(LiteralExpression const* expr) = 0;
		virtual Value VisitBinary(BinaryExpression const* expr) = 0;
		virtual Value VisitUnary(UnaryExpression const* expr) = 0;
		virtual Value VisitName(NameExpression const* expr) = 0;
		virtual Value VisitBlock(BlockExpression const* expr) = 0;
		virtual Value VisitVar(VarValStatement const* expr) = 0;
		virtual Value VisitAssign(AssignExpression const* expr) = 0;
		virtual Value VisitInvokeFunction(InvokeFunctionExpression const* expr) = 0;
		virtual Value VisitRepeat(RepeatStatement const* expr) = 0;
		virtual Value VisitWhile(WhileStatement const* stmt) = 0;
		virtual Value VisitIfThenElse(IfThenElseExpression const* expr) = 0;
		virtual Value VisitFunctionDeclaration(FunctionDeclaration const* decl) = 0;
		virtual Value VisitReturn(ReturnStatement const* decl) = 0;
		virtual Value VisitBreakContinue(BreakOrContinueStatement const* stmt) = 0;
		virtual Value VisitFor(ForStatement const* stmt) = 0;
		virtual Value VisitStatements(Statements const* stmts) = 0;
		virtual Value VisitAnonymousFunction(AnonymousFunctionExpression const* func) = 0;
		virtual Value VisitEnumDeclaration(EnumDeclaration const* decl) = 0;
		virtual Value VisitExpressionStatement(ExpressionStatement const* expr) = 0;
		virtual Value VisitArrayExpression(ArrayExpression const* expr) = 0;
		virtual Value VisitGetMember(GetMemberExpression const* expr) = 0;
		virtual Value VisitAccessArray(AccessArrayExpression const* expr) = 0;
	};
}
