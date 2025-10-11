#pragma once

#include <Visitor.h>
#include <Tokenizer.h>

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
	class AssignArrayIndexExpression;
	class ClassDeclaration;
	class NewObjectExpression;
	class AssignFieldExpression;
	class ForEachStatement;
	class RangeExpression;
	class MatchExpression;
	class UseStatement;
	class AstNode;

	class TreeViewVisitor : public Visitor {
	public:
		TreeViewVisitor(HWND hTreeView, Dynamix::Tokenizer& t);
		void Visit(AstNode* node, HTREEITEM hRoot);

	private:
		Value VisitLiteral(LiteralExpression const* expr) override;
		Value VisitBinary(BinaryExpression const* expr) override;
		Value VisitUnary(UnaryExpression const* expr) override;
		Value VisitName(NameExpression const* expr) override;
		Value VisitVar(VarValStatement const* expr) override;
		Value VisitAssign(AssignExpression const* expr) override;
		Value VisitInvokeFunction(InvokeFunctionExpression const* expr) override;
		Value VisitRepeat(RepeatStatement const* expr) override;
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
		Value VisitArrayExpression(ArrayExpression const* expr) override;
		Value VisitGetMember(GetMemberExpression const* expr) override;
		Value VisitAccessArray(AccessArrayExpression const* expr) override;
		Value VisitAssignArrayIndex(AssignArrayIndexExpression const* expr) override;
		Value VisitClassDeclaration(ClassDeclaration const* decl) override;
		Value VisitNewObjectExpression(NewObjectExpression const* expr) override;
		Value VisitAssignField(AssignFieldExpression const* expr) override;
		Value VisitForEach(ForEachStatement const* stmt) override;
		Value VisitRange(RangeExpression const* expr) override;
		Value VisitMatch(MatchExpression const* expr) override;
		Value VisitUse(UseStatement const* use) override;

		CTreeViewCtrl m_Tree;
		HTREEITEM m_hCurrent{};
		Tokenizer& m_Tokenizer;
	};
}