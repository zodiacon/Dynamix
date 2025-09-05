#pragma once

#include <unordered_map>
#include <memory>
#include <stack>
#include <string_view>
#include <vector>
#include <string>

#include "Tokenizer.h"
#include "ParseError.h"
#include "Parselets.h"
#include "AstNode.h"
#include "SymbolTable.h"

namespace Dynamix {
	class AstNode;

	class Parser {
	public:
		explicit Parser(Tokenizer& t, bool repl = false);
		virtual ~Parser() noexcept = default;
		virtual bool Init();

		std::unique_ptr<Statements> Parse(std::string_view text, int line = 1);
		std::unique_ptr<Statements> ParseFile(std::string_view filename);
		std::unique_ptr<Statements> ParseFiles(std::initializer_list<std::string_view> filenames);

		bool AddParslet(TokenType type, std::unique_ptr<InfixParslet> parslet);
		bool AddParslet(TokenType type, std::unique_ptr<PrefixParslet> parslet);
		void AddError(ParseError err);
		bool HasErrors() const;
		std::span<const ParseError> Errors() const;
		void PushScope(AstNode* node);
		void PopScope();

		bool AddSymbol(Symbol sym);
		Symbol const* FindSymbol(std::string const& name, bool localOnly = false) const;
		SymbolTable const* GetSymbolTable() const;
		std::vector<Symbol const*> GlobalSymbols() const;

		int GetPrecedence() const;

		std::unique_ptr<Expression> ParseExpression(int precedence = 0);
		std::unique_ptr<Statements> ParseVarConstStatement(bool constant);
		std::unique_ptr<FunctionDeclaration> ParseFunctionDeclaration(bool method = false);
		std::unique_ptr<RepeatStatement> ParseRepeatStatement();
		std::unique_ptr<WhileStatement> ParseWhileStatement();
		std::unique_ptr<Statements> ParseBlock(std::vector<Parameter> const& args = {});
		std::unique_ptr<Statement> ParseStatement(bool topLevel = false);
		std::unique_ptr<ReturnStatement> ParseReturnStatement();
		std::unique_ptr<BreakOrContinueStatement> ParseBreakContinueStatement();
		std::unique_ptr<EnumDeclaration> ParseEnumDeclaration();
		std::unique_ptr<ForStatement> ParseForStatement();
		std::unique_ptr<ClassDeclaration> ParseClassDeclaration();

		Token Next();
		Token const& Peek() const;
		bool SkipTo(TokenType type);
		bool Match(TokenType type, bool consume = true, bool errorIfNotFound = false);
		bool Match(std::string_view lexeme, bool consume = true, bool errorIfNotFound = false);

	protected:
		virtual std::unique_ptr<Statements> DoParse();

	private:
		Tokenizer& m_Tokenizer;
		mutable Token m_NextToken{ TokenType::Invalid };
		std::unordered_map<TokenType, std::unique_ptr<InfixParslet>> m_InfixParslets;
		std::unordered_map<TokenType, std::unique_ptr<PrefixParslet>> m_PrefixParslets;
		std::vector<ParseError> m_Errors;
		SymbolTable m_GlobalSymbols;
		std::stack<SymbolTable*> m_Symbols;
		std::stack<std::string> m_Namespaces;
		std::string m_CurrentFile;
		int m_LoopCount{ 0 };
		bool m_Repl;
	};
}


