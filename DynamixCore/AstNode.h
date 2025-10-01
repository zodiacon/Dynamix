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
		InvokeFunction,
		AnonymousFunction,

		Statement = 0x400,
		Statements,
		FunctionDeclaration,
		VarValStatement,
		ArrayAccess,
		GetMember,
		ClassDeclaration,
		EnumDeclararion,
		InterfaceDeclaration,
	};

	class AstNode {
	public:
		virtual ~AstNode() noexcept = default;
		virtual std::string ToString() const {
			return "";
		}
		void* operator new(size_t size);
		void operator delete(void* p, size_t);

		virtual Value Accept(Visitor* visitor) const = 0;
		virtual AstNodeType NodeType() const noexcept {
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

	class Expression : public AstNode {
	public:
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::Expression;
		}
	};

	struct Parameter {
		std::string Name;
		ParameterFlags Flags{ ParameterFlags::None };
		std::unique_ptr<Expression> DefaultValue;
	};

	class GetMemberExpression : public Expression {
	public:
		GetMemberExpression(std::unique_ptr<Expression> left, std::string member, Token op) noexcept;
		Value Accept(Visitor* visitor) const override;

		Expression const* Left() const noexcept;
		std::string const& Member() const noexcept;
		AstNodeType NodeType() const noexcept {
			return AstNodeType::GetMember;
		}
		Token const& Operator() const noexcept {
			return m_Operator;
		}

	private:
		std::unique_ptr<Expression> m_Left;
		std::string m_Member;
		Token m_Operator;
	};

	class MatchCaseExpression {
	public:
		explicit MatchCaseExpression(std::unique_ptr<Statements> action) noexcept : m_Action(std::move(action)) {}
		void AddCase(std::unique_ptr<Expression> expr) noexcept {
			m_Cases.push_back(std::move(expr));
		}
		void SetCases(std::vector<std::unique_ptr<Expression>> cases) noexcept {
			m_Cases = std::move(cases);
		}
		Statements const* Action() const noexcept {
			return m_Action.get();
		}

	private:
		std::unique_ptr<Statements> m_Action;
		std::vector<std::unique_ptr<Expression>> m_Cases;
	};

	class MatchExpression : public Expression {
	public:
		explicit MatchExpression(std::unique_ptr<Expression> expr) noexcept : m_Expr(std::move(expr)) {}
		Value Accept(Visitor* visitor) const override;
		void AddCase(MatchCaseExpression expr) noexcept {
			m_Cases.push_back(std::move(expr));
		}

		std::vector<MatchCaseExpression> const& Cases() const noexcept {
			return m_Cases;
		}
		Expression const* ToMatch() const noexcept {
			return m_Expr.get();
		}

	private:
		std::unique_ptr<Expression> m_Expr;
		std::vector<MatchCaseExpression> m_Cases;
	};

	class AccessArrayExpression : public Expression {
	public:
		AccessArrayExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> index) noexcept;
		Value Accept(Visitor* visitor) const override;
		AstNodeType NodeType() const noexcept {
			return AstNodeType::ArrayAccess;
		}
		Expression const* Left() const noexcept;
		Expression const* Index() const noexcept;

	private:
		std::unique_ptr<Expression> m_Left, m_Index;
	};

	class Statement : public Expression {
	public:
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::Statement;
		}

	protected:
		Statement() = default;
	};

	class ArrayExpression : public Expression {
	public:
		void Add(std::unique_ptr<Expression> expr);
		Value Accept(Visitor* visitor) const override;

		std::vector<std::unique_ptr<Expression>> const& Items() const;

	private:
		std::vector<std::unique_ptr<Expression>> m_Items;
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
		AstNodeType NodeType() const noexcept {
			return AstNodeType::Statements;
		}

		Value Accept(Visitor* visitor) const override;
		void Add(std::unique_ptr<Statement> stmt);
		Statement const* Append(std::unique_ptr<Statements> stmts);
		std::vector<std::unique_ptr<Statement>> const& Get() const;
		std::unique_ptr<Statement> RemoveAt(int index);
		Statement const* GetAt(int i) const;
		int Count() const noexcept {
			return static_cast<int>(m_Stmts.size());
		}
		std::string ToString() const override;
		std::vector<std::unique_ptr<Statement>> const& All() const noexcept {
			return m_Stmts;
		}

	private:
		std::vector<std::unique_ptr<Statement>> m_Stmts;
	};

	class VarValStatement : public Statement {
	public:
		VarValStatement(std::string name, SymbolFlags flags, std::unique_ptr<Expression> init) noexcept;
		AstNodeType NodeType() const noexcept {
			return AstNodeType::VarValStatement;
		}

		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;

		std::string const& Name() const noexcept;
		Expression const* Init() const noexcept;
		bool IsConst() const noexcept;
		bool IsStatic() const noexcept;
		SymbolFlags Flags() const noexcept {
			return m_Flags;
		}

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Init;
		SymbolFlags m_Flags;
	};

	class AssignExpression : public Expression {
	public:
		AssignExpression(std::string lhs, std::unique_ptr<Expression> rhs, Token assignType) noexcept;
		Value Accept(Visitor* visitor) const override;
		std::string const& Lhs() const noexcept;
		Expression const* Value() const noexcept;
		Token const& AssignType() const noexcept;
		std::string ToString() const override;

	private:
		std::string m_Lhs;
		std::unique_ptr<Expression> m_Value;
		Token m_AssignType;
	};

	class AssignFieldExpression : public Expression {
	public:
		AssignFieldExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Token assignType) noexcept;
		Value Accept(Visitor* visitor) const override;
		Token const& AssignType() const noexcept {
			return m_AssignType;
		}
		GetMemberExpression const* Lhs() const noexcept {
			return reinterpret_cast<GetMemberExpression const*>(m_Lhs.get());
		}
		Expression const* Value() const noexcept {
			return m_Value.get();
		}

	private:
		std::unique_ptr<Expression> m_Lhs;
		std::unique_ptr<Expression> m_Value;
		Token m_AssignType;
	};

	class AssignArrayIndexExpression : public Expression {
	public:
		AssignArrayIndexExpression(std::unique_ptr<Expression> arrayAccess, std::unique_ptr<Expression> rhs, Token assignType) noexcept;
		Value Accept(Visitor* visitor) const override;

		AccessArrayExpression const* ArrayAccess() const noexcept {
			return reinterpret_cast<AccessArrayExpression const*>(m_ArrayAccess.get());
		}
		Expression const* Value() const noexcept {
			return m_Value.get();
		}
		Token const& AssignType() const noexcept {
			return m_AssignType;
		}

	private:
		std::unique_ptr<Expression> m_ArrayAccess;
		std::unique_ptr<Expression> m_Value;
		Token m_AssignType;
	};

	//class PostfixExpression : public Expression {
	//public:
	//	PostfixExpression(std::unique_ptr<Expression> expr, Token token);
	//	Value Accept(Visitor* visitor) const override;

	//	Token const& Operator() const noexcept;
	//	Expression const* Argument() const noexcept;

	//private:
	//	std::unique_ptr<Expression> m_Expr;
	//	Token m_Token;
	//};

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
		NameExpression(std::string name, std::string ns);
		Value Accept(Visitor* visitor) const override;
		std::string const& Name() const noexcept;
		std::string const& NameSpace() const noexcept;
		std::string ToString() const override;
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::Name;
		}

	private:
		std::string m_Name, m_NameSpace;
	};

	class UnaryExpression : public Expression {
	public:
		UnaryExpression(Token op, std::unique_ptr<Expression> arg) noexcept;
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
		explicit LiteralExpression(Value value) noexcept;
		Value Accept(Visitor* visitor) const override;

		std::string ToString() const override;
		Value const& Literal() const noexcept;
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::Literal;
		}

	private:
		Value m_Value;
	};

	class RangeExpression : public Expression {
	public:
		RangeExpression(std::unique_ptr<Expression> start, std::unique_ptr<Expression> end, bool endInclusive = false) noexcept;
		Value Accept(Visitor* visitor) const override;
		Expression const* Start() const noexcept {
			return m_Start.get();
		}
		Expression const* End() const noexcept {
			return m_End.get();
		}
		bool EndInclusive() const noexcept {
			return m_EndInclusive;
		}

	private:
		std::unique_ptr<Expression> m_Start, m_End;
		bool m_EndInclusive;
	};

	class InvokeFunctionExpression : public Expression {
	public:
		InvokeFunctionExpression(std::unique_ptr<Expression> callable, std::vector<std::unique_ptr<Expression>> args);
		Value Accept(Visitor* visitor) const override;
		Expression const* Callable() const;
		std::vector<std::unique_ptr<Expression>> const& Arguments() const;
		std::string ToString() const override;
		AstNodeType NodeType() const noexcept {
			return AstNodeType::InvokeFunction;
		}
	private:
		std::unique_ptr<Expression> m_Callable;
		std::vector<std::unique_ptr<Expression>> m_Arguments;
	};

	enum class UseType {
		Class,
		Namespace,
		Module,
	};

	class UseStatement : public Statement {
	public:
		UseStatement(std::string name, UseType type) noexcept : m_Name(std::move(name)), m_Type(type) {}
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const noexcept {
			return m_Name;
		}
		UseType Type() const noexcept {
			return m_Type;
		}

	private:
		std::string m_Name;
		UseType m_Type;
	};

	class ForEachStatement : public Statement {
	public:
		ForEachStatement(std::string name, std::unique_ptr<Expression> collection, std::unique_ptr<Statements> body) noexcept;
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const noexcept {
			return m_Name;
		}
		Statements const* Body() const noexcept {
			return m_Body.get();
		}
		Expression const* Collection() const noexcept {
			return m_Collection.get();
		}

	private:
		std::string m_Name;
		std::unique_ptr<Expression> m_Collection;
		std::unique_ptr<Statements> m_Body;
	};

	class ForStatement : public Statement {
	public:
		ForStatement() = default;
		ForStatement(std::unique_ptr<Statement> init, std::unique_ptr<Expression> whileExpr, 
			std::unique_ptr<Expression> incExpr, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;
		void SetBody(std::unique_ptr<Statements> body) noexcept {
			m_Body = move(body);
			m_Body->SetParent(this);
		}
		void SetInit(std::unique_ptr<Statement> init) noexcept {
			m_Init = move(init);
			if (m_Init)
				m_Init->SetParent(this);
		}
		void SetWhile(std::unique_ptr<Expression> expr) noexcept {
			m_While = move(expr);
			if(m_While)
				m_While->SetParent(this);
		}
		void SetInc(std::unique_ptr<Expression> expr) noexcept {
			m_Inc = move(expr);
			if(m_Inc)
				m_Inc->SetParent(this);
		}
		Statement const* Init() const noexcept;
		Expression const* While() const noexcept;
		Expression const* Inc() const noexcept;
		Statements const* Body() const noexcept;

	private:
		std::unique_ptr<Expression> m_While, m_Inc;
		std::unique_ptr<Statement> m_Init;
		std::unique_ptr<Statements> m_Body;
	};

	class FunctionEssentials {
	public:
		void SetBody(std::unique_ptr<Expression> body) noexcept {
			m_Body = std::move(body);
		}
		Expression const* Body() const noexcept {
			return m_Body.get();
		}

		void SetParameters(std::vector<Parameter> parameters) noexcept {
			m_Parameters = std::move(parameters);
		}

		std::vector<Parameter> const& Parameters() const noexcept {
			return m_Parameters;
		}

	protected:
		std::vector<Parameter> m_Parameters;
		std::unique_ptr<Expression> m_Body;
	};

	class FunctionDeclaration : public Statement, public FunctionEssentials {
	public:
		explicit FunctionDeclaration(std::string name, bool method = false, bool isStatic = false);
		Value Accept(Visitor* visitor) const override;

		std::string const& Name() const noexcept;
		std::string ToString() const override;
		bool IsMethod() const noexcept {
			return m_Method;
		}
		bool IsStatic() const noexcept {
			return m_Static;
		}
		AstNodeType NodeType() const noexcept {
			return AstNodeType::FunctionDeclaration;
		}
		void SetBody(std::unique_ptr<Expression> body) noexcept;

	private:
		std::string m_Name;
		bool m_Method, m_Static;
	};

	class InterfaceDeclaration : public Statement {
	public:
		explicit InterfaceDeclaration(std::string name) noexcept : m_Name(std::move(name)) {}

		AstNodeType NodeType() const noexcept override {
			return AstNodeType::InterfaceDeclaration;
		}
		Value Accept(Visitor* visitor) const override;

		void SetMethods(std::vector<std::unique_ptr<FunctionDeclaration>> methods) noexcept {
			m_Methods = std::move(methods);
		}
		std::vector<std::unique_ptr<FunctionDeclaration>> const& Methods() const noexcept {
			return m_Methods;
		}
		void AddBaseInterface(std::string name) noexcept {
			m_BaseNames.push_back(std::move(name));
		}

		std::string const& Name() const noexcept {
			return m_Name;
		}
		std::vector<std::string> const& BaseNames() const noexcept {
			return m_BaseNames;
		}

	private:
		std::string m_Name;
		std::vector<std::string> m_BaseNames;
		std::vector<std::unique_ptr<FunctionDeclaration>> m_Methods;
	};

	class ClassDeclaration : public Statement {
	public:
		explicit ClassDeclaration(std::string name, ClassDeclaration const* parent = nullptr) noexcept;
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::ClassDeclaration;
		}
		void SetBaseType(std::string name) noexcept {
			m_BaseName = std::move(name);
		}

		Value Accept(Visitor* visitor) const override;
		void SetMethods(std::vector<std::unique_ptr<FunctionDeclaration>> methods) noexcept {
			m_Methods = std::move(methods);
		}
		void SetFields(std::vector<std::unique_ptr<Statement>> fields) noexcept {
			m_Fields = std::move(fields);
		}
		void SetTypes(std::vector<std::unique_ptr<ClassDeclaration>> types) noexcept {
			m_Types = std::move(types);
		}
		ClassDeclaration const* Parent() const noexcept {
			return m_Parent;
		}
		std::vector<std::unique_ptr<FunctionDeclaration>> const& Methods() const noexcept {
			return m_Methods;
		}
		std::vector<std::unique_ptr<Statement>> const& Fields() const noexcept {
			return m_Fields;
		}
		std::vector<std::unique_ptr<ClassDeclaration>> const& Types() const noexcept {
			return m_Types;
		}
		std::string const& Name() const noexcept {
			return m_Name;
		}
		std::string const& BaseName() const noexcept {
			return m_BaseName;
		}

	private:
		std::string m_Name, m_BaseName;
		std::vector<std::unique_ptr<FunctionDeclaration>> m_Methods;
		std::vector<std::unique_ptr<Statement>> m_Fields;
		std::vector<std::unique_ptr<ClassDeclaration>> m_Types;
		ClassDeclaration const* m_Parent;
	};

	class EnumDeclaration : public Statement {
	public:
		EnumDeclaration(std::string name, std::unordered_map<std::string, long long> values);
		Value Accept(Visitor* visitor) const override;
		AstNodeType NodeType() const noexcept {
			return AstNodeType::EnumDeclararion;
		}
		std::string const& Name() const noexcept;
		std::unordered_map<std::string, long long> const& Values() const noexcept;

	private:
		std::string m_Name;
		std::unordered_map<std::string, long long> m_Values;
	};

	class AnonymousFunctionExpression : public Expression, public FunctionEssentials {
	public:
		AnonymousFunctionExpression(std::vector<Parameter> args, std::unique_ptr<Expression> body);
		AstNodeType NodeType() const noexcept override {
			return AstNodeType::AnonymousFunction;
		}
		Value Accept(Visitor* visitor) const override;
	};

	struct FieldInitializer {
		std::string Name;
		std::unique_ptr<Expression> Init;
	};

	class NewObjectExpression : public Expression {
	public:
		NewObjectExpression(std::string className, std::vector<std::unique_ptr<Expression>> args, std::vector<FieldInitializer> inits)
			: m_ClassName(std::move(className)), m_Args(std::move(args)), m_FieldInit(move(inits)) {}
		Value Accept(Visitor* visitor) const override;
		std::string const& ClassName() const {
			return m_ClassName;
		}
		std::vector<std::unique_ptr<Expression>> const& Args() const noexcept {
			return m_Args;
		}

		void AddFieldInit(FieldInitializer init) noexcept {
			m_FieldInit.push_back(std::move(init));
		}

		std::vector<FieldInitializer> const& FieldInitializers() const noexcept {
			return m_FieldInit;
		}
	private:
		std::string m_ClassName;
		std::vector<std::unique_ptr<Expression>> m_Args;
		std::vector<FieldInitializer> m_FieldInit;
	};

	class BreakOrContinueStatement : public Statement {
	public:
		explicit BreakOrContinueStatement(TokenType type);
		Value Accept(Visitor* visitor) const override;
		TokenType BreakType() const noexcept;

	private:
		TokenType m_Type;
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

	class RepeatStatement : public Statement {
	public:
		RepeatStatement(std::unique_ptr<Expression> times, std::unique_ptr<Statements> body);
		Value Accept(Visitor* visitor) const override;

		Expression const* Times() const noexcept;
		Statements const* Body() const noexcept;

	private:
		std::unique_ptr<Expression> m_Times;
		std::unique_ptr<Statements> m_Body;
	};

	class IfThenElseExpression : public Expression {
	public:
		IfThenElseExpression(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> thenExpr, std::unique_ptr<Statement> elseExpr = nullptr);
		Value Accept(Visitor* visitor) const override;
		std::string ToString() const override;
		AstNodeType NodeType() const noexcept {
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


