#pragma once

#include <stack>
#include <memory>
#include "Scope.h"
#include "Visitor.h"
#include "Value.h"
#include "Runtime.h"

namespace Dynamix {
	class Scope;
	class Parser;
	class SymbolTable;
	class Runtime;
	class AstNode;

	class Interpreter : public Visitor {
	public:
		explicit Interpreter(Parser& p, Runtime* rt = nullptr);

		Value Run(AstNode* root);

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

		void PushScope();
		void PopScope();

		Scope* CurrentScope();
		void AddNativeFunctions();

		std::unique_ptr<AstNode> Parse(std::string_view code) const;

	private:
		enum class LoopAction {
			None,
			Break,
			Continue,
			BreakAll,
		};
		Parser& m_Parser;
		Runtime* m_Runtime;
		std::stack<std::unique_ptr<Scope>> m_Scopes;
		Value m_ReturnValue;
		LoopAction m_LoopAction{ LoopAction::None };
		int m_InLoop{ 0 };
		bool m_Return{ false };
	};

}
