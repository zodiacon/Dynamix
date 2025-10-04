#pragma once

#include <unordered_map>
#include <memory>
#include <stack>
#include <string_view>
#include <vector>
#include <string>
#include <span>
#include "Tokenizer.h"
#include "ParseError.h"
#include "Parselets.h"
#include "AstNode.h"
#include "SymbolTable.h"

namespace Dynamix {
	class AstNode;

	class Parser {
	public:
		explicit Parser(Tokenizer& t);
		virtual ~Parser() noexcept = default;
		void Clear();

		std::unique_ptr<Statements> Parse(std::string_view text, bool repl = false, int line = 1);
		std::unique_ptr<Statements> ParseFile(std::string_view filename);
		std::vector<std::unique_ptr<Statements>> ParseFiles(std::initializer_list<std::string_view> filenames);

		bool AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet);
		bool AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet);
		void AddError(ParseError err);
		bool HasErrors() const;
		std::span<const ParseError> Errors() const;
		void PushScope(AstNode* node);
		void PopScope();

		CodeLocation Location() const noexcept;

		bool AddSymbol(Symbol sym) noexcept;
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const noexcept;
		std::vector<Symbol const*> GlobalSymbols() const noexcept;

		int GetPrecedence() const;

		std::unique_ptr<Expression> ParseExpression(int precedence = 0);
		std::unique_ptr<Statement> ParseVarValStatement(bool constant, SymbolFlags extraFlags = SymbolFlags::None);
		std::unique_ptr<FunctionDeclaration> ParseFunctionDeclaration(bool method = false, SymbolFlags extraFlags = SymbolFlags::None);
		std::unique_ptr<RepeatStatement> ParseRepeatStatement();
		std::unique_ptr<WhileStatement> ParseWhileStatement();
		std::unique_ptr<Statements> ParseBlock(std::vector<Parameter> const& args = {}, bool newscope = true);
		std::unique_ptr<Statement> ParseStatement(bool topLevel = false, bool errorIfNotFound = true);
		std::unique_ptr<ReturnStatement> ParseReturnStatement();
		std::unique_ptr<BreakOrContinueStatement> ParseBreakContinueStatement();
		std::unique_ptr<EnumDeclaration> ParseEnumDeclaration();
		std::unique_ptr<ForStatement> ParseForStatement();
		std::unique_ptr<ClassDeclaration> ParseClassDeclaration(ClassDeclaration const* parent = nullptr);
		std::unique_ptr<InterfaceDeclaration> ParseInterfaceDeclaration();
		std::unique_ptr<ForEachStatement> ParseForEachStatement();
		std::unique_ptr<Statements> ParseStatementsForMatch(bool newScope = false);
		std::unique_ptr<Statement> ParseUseStatement();

		Token Next();
		Token const& Peek() const;
		bool SkipTo(TokenType type);
		bool Match(TokenType type, bool consume = true, bool errorIfNotFound = false);
		bool Match(std::string_view lexeme, bool consume = true, bool errorIfNotFound = false);
		int AddConstSTring(std::string str);

	protected:
		virtual bool Init();
		virtual std::unique_ptr<Statements> DoParse();

	private:
		Tokenizer& m_Tokenizer;
		std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
		std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
		std::vector<ParseError> m_Errors;
		SymbolTable m_GlobalSymbols;
		std::stack<SymbolTable*> m_Symbols;
		std::string m_CurrentFile;
		std::vector<std::string> m_ConstStrings;
		int m_LoopCount{ 0 };
		int m_InClass{ 0 };
		bool m_Repl{ false };
	};
}
