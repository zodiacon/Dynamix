#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "Value.h"
#include "Visitor.h"
#include "Token.h"
#include "SymbolTable.h"

namespace Dynamix {
	enum class AstNodeType {
		None,
		Name,
		VarValStatement,
		Literal,
		Expression = 0x200,
		Statement = 0x400,
		Declaration = 0x800,
	};

	class AstNode {
	public:
		virtual ~AstNode() noexcept = default;
		virtual std::string ToString() const {
			return "";
		}
		virtual Value Accept(Visitor* visitor) const = 0;
		virtual AstNodeType Type() const {
			return AstNodeType::None;
		}
		SymbolTable* Symbols() {
			return &m_Symbols;
		}

	private:
		SymbolTable m_Symbols;
	};

	enum class ParameterFlags {
		None = 0,
		Ref = 1,
		Out = 3,
	};

	struct Parameter {
		std::string Name;
		ParameterFlags Flags;
	};

	class Expression : public AstNode {
	public:
		AstNodeType Type() const override {
			return AstNodeType::Expression;
		}
	protected:
		Expression() = default;
	};

	class Statement : public Expression {
	public:
		AstNodeType Type() const override {
			return AstNodeType::Statement;
		}

	protected:
		Statement() = default;
	};

	class ExpressionStatement final : public Statement {
	public:
		explicit ExpressionStatement(std::unique_ptr<Expression> expr);
		Value Accept(Visitor* visitor) const override;
		Expression const* Expr() const;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class Statements final : public Statement {
	public:
		Value Accept(Visitor* visitor) const override;
		void Add(std::unique_ptr<Statement> stmt);
		std::vector<std::unique_ptr<Statement>> const& Get() const;
		std::string ToString() const override;

	private:
		std::vector<std::unique_ptr<Statement>> m_Stmts;
	};

	class VarValStatement : public Statement {
	public:
		VarValStatement(std::string name, bool isConst, std::unique_ptr<Expression> init);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;

		std::string const& Name() const;
		Expression const* Init() const;
		bool IsConst() const;

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Init;
		bool m_IsConst;
	};

	class AssignExpression : public Expression {
	public:
		AssignExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Token assignType);
		Value Accept(Visitor* visitor) const override;
		Expression const* Left() const;
		Expression const* Value() const;
		Token const& AssignType() const;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Left;
		std::unique_ptr<Expression> m_Expr;
		Token m_AssignType;
	};

	class PostfixExpression : public Expression {
	public:
		PostfixExpression(std::unique_ptr<Expression> expr, Token token);
		Value Accept(Visitor* visitor) const override;

		Token const& Operator() const;
		Expression const* Argument() const;

	private:
		std::unique_ptr<Expression> m_Expr;
		Token m_Token;
	};

	class BinaryExpression : public Expression {
	public:
		BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;

		Expression* Left() const;
		Expression* Right() const;
		Token const& Operator() const;

	private:
		std::unique_ptr<Expression> m_Left, m_Right;
		Token m_Operator;
	};

	class NameExpression : public Expression {
	public:
		explicit NameExpression(std::string name);
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const;
		std::string ToString() const override;
		AstNodeType Type() const override {
			return AstNodeType::Name;
		}

	private:
		std::string m_Name;
	};

	class UnaryExpression : public Expression {
	public:
		UnaryExpression(Token op, std::unique_ptr<Expression> arg);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;
		Token const& Operator() const;
		Expression* Arg() const;

	private:
		std::unique_ptr<Expression> m_Arg;
		Token m_Operator;
	};

	class LiteralExpression : public Expression {
	public:
		explicit LiteralExpression(Token token);
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;
		Token const& Literal() const;
		AstNodeType Type() const override {
			return AstNodeType::Literal;
		}

	private:
		Token m_Token;
	};

	class InvokeFunctionExpression : public Expression {
	public:
		InvokeFunctionExpression(std::string name, std::vector<std::unique_ptr<Expression>> args);
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const;
		std::vector<std::unique_ptr<Expression>> const& Arguments() const;
		std::string ToString() const override;

	private:
		std::string m_Name;
		std::vector<std::unique_ptr<Expression>> m_Arguments;
	};

	class ForStatement : public Statement {
	public:
		ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> whileExpr, std::unique_ptr<Expression> incExpr, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;

		Statement const* Init() const;
		Expression const* While() const;
		Expression const* Inc() const;
		BlockExpression const* Body() const;

	private:
		std::unique_ptr<Expression> m_While, m_Inc;
		std::unique_ptr<Statement> m_Init;
		std::unique_ptr<Statements> m_Body;
	};

	class FunctionDeclaration : public Statement {
	public:
		explicit FunctionDeclaration(std::string name);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const;
		std::vector<Parameter> const& Parameters() const;
		Expression const* Body() const;
		std::string ToString() const override;
		void Parameters(std::vector<Parameter> parameters);
		void Body(std::unique_ptr<Expression> body);

	private:
		std::string m_Name;
		std::vector<Parameter> m_Parameters;
		std::unique_ptr<Expression> m_Body;
	};

	class EnumDeclaration : public Statement {
	public:
		EnumDeclaration(std::string name, std::unordered_map<std::string, long long> values);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const;
		std::unordered_map<std::string, long long> const& Values() const;

	private:
		std::string m_Name;
		std::unordered_map<std::string, long long> m_Values;
	};

	class AnonymousFunctionExpression : public Expression {
	public:
		AnonymousFunctionExpression(std::vector<Parameter> args, std::unique_ptr<Expression> body);
		Value Accept(Visitor* visitor) const override;
		std::vector<Parameter> const& Args() const;
		Expression const* Body() const;

	private:
		std::vector<Parameter> m_Args;
		std::unique_ptr<Expression> m_Body;
	};

	class BreakOrContinueStatement : public Statement {
	public:
		explicit BreakOrContinueStatement(bool cont);
		Value Accept(Visitor* visitor) const override;
		bool IsContinue() const;

	private:
		bool m_IsContinue;
	};

	class WhileStatement : public Statement {
	public:
		WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;

		Expression const* Condition() const;
		Statements const* Body() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Statements> m_Body;
	};

	class ReturnStatement : public Statement {
	public:
		explicit ReturnStatement(std::unique_ptr<Expression> expr = nullptr);
		Value Accept(Visitor* visitor) const override;
		Expression const* ReturnValue() const;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class IfThenElseExpression : public Expression {
	public:
		IfThenElseExpression(std::unique_ptr<Expression> condition, std::unique_ptr<Expression> thenExpr, std::unique_ptr<Expression> elseExpr = nullptr);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;

		Expression const* Condition() const;
		Expression const* Then() const;
		Expression const* Else() const;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Expression> m_Then, m_Else;
	};

}


