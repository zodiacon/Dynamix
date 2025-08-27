#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Value.h"
#include "Visitor.h"
#include "Token.h"
#include "SymbolTable.h"
#include "ObjectType.h"
#include "ParseError.h"

namespace Dynamix {
	enum class AstNodeType {
		None = 0,

		Expression = 0x200,
		Literal,
		IfThenElse,
		Name,

		Statement = 0x400,
		Statements,
		FunctionDeclaration,
		VarValStatement,
	};

	class AstNode {
	public:
		virtual ~AstNode() noexcept = default;
		virtual std::string ToString() const {
			return "";
		}
		virtual Value Accept(Visitor* visitor) const = 0;
		virtual AstNodeType Type() const noexcept {
			return AstNodeType::None;
		}

		void SetLocation(CodeLocation loc) {
			m_Location = std::move(loc);
		}

		CodeLocation const& Location() const {
			return m_Location;
		}

		SymbolTable* Symbols() noexcept {
			return &m_Symbols;
		}

		SymbolTable const* Symbols() const {
			return &m_Symbols;
		}

		void SetParent(AstNode* parent) noexcept {
			m_Symbols.SetParent(parent->Symbols());
		}

		void SetParentSymbols(SymbolTable* symbols) noexcept {
			m_Symbols.SetParent(symbols);
		}

	private:
		SymbolTable m_Symbols;
		CodeLocation m_Location;
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
		AstNodeType Type() const noexcept override {
			return AstNodeType::Expression;
		}
	};

	class Statement : public Expression {
	public:
		AstNodeType Type() const noexcept override {
			return AstNodeType::Statement;
		}

	protected:
		Statement() = default;
	};

	class ExpressionStatement final : public Statement {
	public:
		ExpressionStatement(std::unique_ptr<Expression> expr, bool semicolon);
		Value Accept(Visitor* visitor) const override;
		Expression const* Expr() const;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Expr;
		bool m_Semicolon;
	};

	class Statements final : public Statement {
	public:
		AstNodeType Type() const noexcept {
			return AstNodeType::Statements;
		}

		Value Accept(Visitor* visitor) const override;
		void Add(std::unique_ptr<Statement> stmt);
		std::vector<std::unique_ptr<Statement>> const& Get() const;
		Statement const* GetAt(int i) const;
		int Count() const;
		std::string ToString() const override;

	private:
		std::vector<std::unique_ptr<Statement>> m_Stmts;
	};

	class VarValStatement : public Statement {
	public:
		VarValStatement(std::string name, bool isConst, std::unique_ptr<Expression> init);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;

		std::string const& Name() const noexcept;
		Expression const* Init() const noexcept;
		bool IsConst() const noexcept;

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Init;
		bool m_IsConst;
	};

	class AssignExpression : public Expression {
	public:
		AssignExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Token assignType);
		Value Accept(Visitor* visitor) const override;
		Expression const* Left() const noexcept;
		Expression const* Value() const noexcept;
		Token const& AssignType() const noexcept;
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

		Token const& Operator() const noexcept;
		Expression const* Argument() const noexcept;

	private:
		std::unique_ptr<Expression> m_Expr;
		Token m_Token;
	};

	class BinaryExpression : public Expression {
	public:
		BinaryExpression(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;

		Expression* Left() const noexcept;
		Expression* Right() const noexcept;
		Token const& Operator() const noexcept;

	private:
		std::unique_ptr<Expression> m_Left, m_Right;
		Token m_Operator;
	};

	class NameExpression : public Expression {
	public:
		NameExpression(std::string ns, std::string name);
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const noexcept;
		std::string const& NameSpace() const noexcept;
		std::string ToString() const override;
		AstNodeType Type() const noexcept override {
			return AstNodeType::Name;
		}

	private:
		std::string m_Name, m_NameSpace;
	};

	class UnaryExpression : public Expression {
	public:
		UnaryExpression(Token op, std::unique_ptr<Expression> arg);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;
		Token const& Operator() const noexcept;
		Expression* Arg() const noexcept;

	private:
		std::unique_ptr<Expression> m_Arg;
		Token m_Operator;
	};

	class LiteralExpression : public Expression {
	public:
		explicit LiteralExpression(Token token);
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;
		Token const& Literal() const noexcept;
		AstNodeType Type() const noexcept override {
			return AstNodeType::Literal;
		}

	private:
		Token m_Token;
	};

	class InvokeFunctionExpression : public Expression {
	public:
		InvokeFunctionExpression(std::unique_ptr<Expression> callable, std::vector<std::unique_ptr<Expression>> args);
		Value Accept(Visitor* visitor) const override;
		Expression const* Callable() const;
		std::vector<std::unique_ptr<Expression>> const& Arguments() const;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Callable;
		std::vector<std::unique_ptr<Expression>> m_Arguments;
	};

	class ForStatement : public Statement {
	public:
		ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> whileExpr, 
			std::unique_ptr<Expression> incExpr, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;

		Statement const* Init() const noexcept;
		Expression const* While() const noexcept;
		Expression const* Inc() const noexcept;
		Statements const* Body() const noexcept;

	private:
		std::unique_ptr<Expression> m_While, m_Inc;
		std::unique_ptr<Statement> m_Init;
		std::unique_ptr<Statements> m_Body;
	};

	class FunctionDeclaration : public Statement {
	public:
		explicit FunctionDeclaration(std::string name);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const noexcept;
		std::vector<Parameter> const& Parameters() const noexcept;
		Expression const* Body() const noexcept;
		std::string ToString() const override;
		void Parameters(std::vector<Parameter> parameters) noexcept;
		void Body(std::unique_ptr<Expression> body) noexcept;

		AstNodeType Type() const noexcept {
			return AstNodeType::FunctionDeclaration;
		}

	private:
		std::string m_Name;
		std::vector<Parameter> m_Parameters;
		std::unique_ptr<Expression> m_Body;
	};

	class ClassDeclaration : Statement {
	public:
		explicit ClassDeclaration(std::unique_ptr<ObjectType> type);

		ObjectType const* GetObjectType() const;

	private:
		std::unique_ptr<ObjectType> m_ObjectType;
	};

	class EnumDeclaration : public Statement {
	public:
		EnumDeclaration(std::string name, std::unordered_map<std::string, long long> values);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const noexcept;
		std::unordered_map<std::string, long long> const& Values() const noexcept;

	private:
		std::string m_Name;
		std::unordered_map<std::string, long long> m_Values;
	};

	class AnonymousFunctionExpression : public Expression {
	public:
		AnonymousFunctionExpression(std::vector<Parameter> args, std::unique_ptr<Expression> body);
		Value Accept(Visitor* visitor) const override;
		std::vector<Parameter> const& Args() const noexcept;
		Expression const* Body() const noexcept;

	private:
		std::vector<Parameter> m_Args;
		std::unique_ptr<Expression> m_Body;
	};

	class BreakOrContinueStatement : public Statement {
	public:
		explicit BreakOrContinueStatement(bool cont);
		Value Accept(Visitor* visitor) const override;
		bool IsContinue() const noexcept;

	private:
		bool m_IsContinue;
	};

	class WhileStatement : public Statement {
	public:
		WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;

		Expression const* Condition() const noexcept;
		Statements const* Body() const noexcept;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Statements> m_Body;
	};

	class ReturnStatement : public Statement {
	public:
		explicit ReturnStatement(std::unique_ptr<Expression> expr = nullptr);
		Value Accept(Visitor* visitor) const override;
		Expression const* ReturnValue() const noexcept;
		std::string ToString() const override;

	private:
		std::unique_ptr<Expression> m_Expr;
	};

	class IfThenElseExpression : public Expression {
	public:
		IfThenElseExpression(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenExpr, std::unique_ptr<Statement> elseExpr = nullptr);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;
		AstNodeType Type() const noexcept {
			return AstNodeType::IfThenElse;
		}

		Expression const* Condition() const noexcept;
		Statement const* Then() const noexcept;
		Statement const* Else() const noexcept;

	private:
		std::unique_ptr<Expression> m_Condition;
		std::unique_ptr<Statement> m_Then, m_Else;
	};

}


