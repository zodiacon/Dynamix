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

	class Interpreter final : public Visitor, NoCopy {
	public:
		Interpreter(Parser& p, Runtime& rt);

		Value Eval(AstNode const* root);

		void RunConstructor(RuntimeObject* instance, MethodInfo const* ctor, std::vector<Value> const& args);

		// Inherited via Visitor
		Value VisitLiteral(LiteralExpression const* expr) override;
		Value VisitBinary(BinaryExpression const* expr) override;
		Value VisitUnary(UnaryExpression const* expr) override;
		Value VisitName(NameExpression const* expr) override;
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
		Value VisitArrayExpression(ArrayExpression const* expr) override;
		Value VisitRepeat(RepeatStatement const* stmt) override;
		Value VisitGetMember(GetMemberExpression const* expr) override;
		Value VisitAccessArray(AccessArrayExpression const* expr) override;
		Value VisitAssignArrayIndex(AssignArrayIndexExpression const* expr) override;
		Value VisitClassDeclaration(ClassDeclaration const* decl) override;
		Value VisitNewObjectExpression(NewObjectExpression const* expr) override;
		Value VisitAssignField(AssignFieldExpression const* expr) override;
		Value VisitForEach(ForEachStatement const* stmt) override;
		Value VisitRange(RangeExpression const* expr) override;

		Scope* CurrentScope();

		std::unique_ptr<AstNode> Parse(std::string_view code) const;

		friend class Scoper;

	protected:
		void PushScope();
		void PopScope();
		struct Scoper {
			Scoper(Interpreter* intr) : m_Intr(intr) {
				intr->PushScope();
			}
			~Scoper() {
				m_Intr->PopScope();
			}

		private:
			Interpreter* m_Intr;
		};

	private:
		Parser& m_Parser;
		Runtime& m_Runtime;
		std::stack<std::unique_ptr<Scope>> m_Scopes;
	};
}
