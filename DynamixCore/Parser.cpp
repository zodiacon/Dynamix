#include "Parser.h"
#include "AstNode.h"
#include <format>

using namespace std;
using namespace Dynamix;

Parser::Parser(Tokenizer& t, bool test) : m_Tokenizer(t), m_Repl(test) {
	Init();
}

void Parser::Init() {
	m_Tokenizer.AddTokens({
		{ "if", TokenType::Keyword_If },
		{ "while", TokenType::Keyword_While },
		{ "fn", TokenType::Keyword_Fn },
		{ "else", TokenType::Keyword_Else },
		{ "var", TokenType::Keyword_Var },
		{ "val", TokenType::Keyword_Val },
		{ "const", TokenType::Keyword_Const },
		{ "true", TokenType::Keyword_True },
		{ "false", TokenType::Keyword_False },
		{ "for", TokenType::Keyword_For },
		{ "repeat", TokenType::Keyword_Repeat },
		{ "break", TokenType::Keyword_Break },
		{ "continue", TokenType::Keyword_Continue },
		{ "return", TokenType::Keyword_Return },
		{ "do", TokenType::Keyword_Do },
		{ "foreach", TokenType::Keyword_ForEach },
		{ "new", TokenType::Keyword_New },
		{ "in", TokenType::Keyword_In },
		{ "interface", TokenType::Keyword_Interface },
		{ "class", TokenType::Keyword_Class },
		{ "object", TokenType::Keyword_Object },
		{ "enum", TokenType::Keyword_Enum },
		{ "struct", TokenType::Keyword_Struct },
		});

	m_Tokenizer.AddTokens({
		{ "(", TokenType::Operator_OpenParen },
		{ ")", TokenType::Operator_CloseParen },
		{ "{", TokenType::Operator_OpenBrace },
		{ "}", TokenType::Operator_CloseBrace },
		{ ">", TokenType::Operator_GreaterThan },
		{ "<", TokenType::Operator_LessThan },
		{ ">=", TokenType::Operator_GreaterThanOrEqual },
		{ "<=", TokenType::Operator_LessThanOrEqual },
		{ "!=", TokenType::Operator_NotEqual },
		{ "==", TokenType::Operator_Equal },
		{ "=", TokenType::Operator_Assign },
		{ ",", TokenType::Operator_Comma },
		{ "|", TokenType::Operator_BitwiseOr },
		{ "&", TokenType::Operator_BitwiseAnd },
		{ ";", TokenType::Operator_Semicolon },
		{ ":", TokenType::Operator_Colon },
		{ "[", TokenType::Operator_OpenBracket },
		{ "]", TokenType::Operator_CloseBracket },
		{ ".", TokenType::Operator_Dot },
		{ "::", TokenType::Operator_ScopeRes },
		{ "+", TokenType::Operator_Plus },
		{ "-", TokenType::Operator_Minus },
		{ "*", TokenType::Operator_Mul },
		{ "/", TokenType::Operator_Div },
		{ "%", TokenType::Operator_Mod },
		{ "+=", TokenType::Operator_Assign_Add },
		});

	AddParslet(TokenType::Keyword_And, make_unique<BinaryOperatorParslet>(80));
	AddParslet(TokenType::Keyword_Or, make_unique<BinaryOperatorParslet>(70));
	AddParslet(TokenType::Keyword_Not, make_unique<PrefixOperatorParslet>(90));
	AddParslet(TokenType::Operator_Plus, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Operator_Minus, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Operator_Mul, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Operator_Div, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Operator_Mod, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Operator_Minus, make_unique<PrefixOperatorParslet>(300));
	AddParslet(TokenType::Integer, make_unique<LiteralParslet>());
	AddParslet(TokenType::String, make_unique<LiteralParslet>());
	AddParslet(TokenType::Keyword_True, make_unique<LiteralParslet>());
	AddParslet(TokenType::Keyword_False, make_unique<LiteralParslet>());
	AddParslet(TokenType::Real, make_unique<LiteralParslet>());
	AddParslet(TokenType::Identifier, make_unique<NameParslet>());
	AddParslet(TokenType::Operator_OpenParen, make_unique<GroupParslet>());
	AddParslet(TokenType::Operator_Power, make_unique<BinaryOperatorParslet>(350, true));
	AddParslet(TokenType::Operator_Assign, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_Add, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_Sub, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_Mul, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_Div, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_Mod, make_unique<AssignParslet>());
	AddParslet(TokenType::Operator_Assign_And, make_unique<AssignParslet>());

	AddParslet(TokenType::Operator_Equal, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_NotEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_LessThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_LessThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_GreaterThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_GreaterThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::Operator_OpenParen, make_unique<InvokeFunctionParslet>());
	AddParslet(TokenType::Keyword_If, make_unique<IfThenElseParslet>());
	AddParslet(TokenType::Operator_BitwiseAnd, make_unique<BinaryOperatorParslet>(400));
	AddParslet(TokenType::Operator_BitwiseOr, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::Operator_BitwiseXor, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::Keyword_Fn, make_unique<AnonymousFunctionParslet>());
	AddParslet(TokenType::Operator_BitwiseNot, make_unique<PrefixOperatorParslet>(500));

	m_Symbols.push(&m_GlobalSymbols);
}

std::unique_ptr<Statements> Parser::Parse(string_view text, int line) {
	if (!m_Tokenizer.Tokenize(text, line))
		return nullptr;

	return DoParse();
}

unique_ptr<Statements> Parser::DoParse() {
	auto block = make_unique<Statements>();
	block->SetParentSymbols(m_Symbols.top());
	while (true) {
		auto stmt = ParseStatement(!m_Repl);
		if (stmt == nullptr)
			break;
		block->Add(move(stmt));
	}
	return HasErrors() ? nullptr : move(block);
}

Token Parser::Next() {
	return m_Tokenizer.Next();
}

Token Parser::Peek() const {
	return m_Tokenizer.Peek();
}

bool Parser::SkipTo(TokenType type) {
	auto next = Next();
	while (next.Type != type) {
		if (next.Type == TokenType::Invalid)
			return false;
	}
	return true;
}

bool Parser::Match(TokenType type, bool consume, bool errorIfNotFound) {
	auto next = Peek();
	auto found = next.Type == type;
	if (consume && found) {
		Next();
		return true;
	}
	if (errorIfNotFound)
		AddError(ParseError{ ParseErrorType::Expected, next, format("'{}' expected", m_Tokenizer.TokenTypeToString(type)) });
	return found;
}

unique_ptr<Expression> Parser::ParseExpression(int precedence) {
	auto token = Next();
	if (auto it = m_PrefixParslets.find(token.Type); it != m_PrefixParslets.end()) {
		auto left = it->second->Parse(*this, token);
		while (precedence < GetPrecedence()) {
			auto token = Next();
			if (token.Type == TokenType::Invalid)
				break;
			if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end()) {
				left = it->second->Parse(*this, move(left), token);
			}
		}
		return left;
	}
	AddError(ParseError{ ParseErrorType::UnknownOperator, token });
	return nullptr;
}

void Parser::AddError(ParseError err) {
	m_Errors.emplace_back(move(err));
}

bool Parser::HasErrors() const {
	return !m_Errors.empty();
}

span<const ParseError> Parser::Errors() const {
	return m_Errors;
}

int Parser::GetPrecedence() const {
	auto token = Peek();
	if (auto it = m_InfixParslets.find(token.Type); it != m_InfixParslets.end())
		return it->second->Precedence();
	return 0;
}

bool Parser::AddParslet(TokenType type, unique_ptr<InfixParslet> parslet) {
	return m_InfixParslets.insert({ type, move(parslet) }).second;
}

bool Parser::AddParslet(TokenType type, unique_ptr<PrefixParslet> parslet) {
	return m_PrefixParslets.insert({ type, move(parslet) }).second;
}

unique_ptr<VarValStatement> Parser::ParseVarConstStatement(bool constant) {
	auto next = Next();		// eat var or val
	auto name = Next();		// variable name
	if (name.Type != TokenType::Identifier)
		throw ParseError(ParseErrorType::IdentifierExpected, name);

	bool dup = false;
	{
		auto sym = FindSymbol(name.Lexeme, true);
		if (sym)
			AddError(ParseError{ ParseErrorType::DuplicateDefinition, name, format("Symbol {} already defined in scope", name.Lexeme) });
		dup = true;
	}
	unique_ptr<Expression> init;
	if (Match(TokenType::Operator_Assign)) {
		init = ParseExpression();
	}
	else if (constant) {
		AddError(ParseError(ParseErrorType::MissingInitExpression, Peek()));
	}

	Match(TokenType::Operator_Semicolon, true, true);
	if (!dup) {
		Symbol sym;
		sym.Name = name.Lexeme;
		sym.Type = SymbolType::Variable;
		sym.Flags = constant ? SymbolFlags::Const : SymbolFlags::None;
		AddSymbol(sym);
	}
	return make_unique<VarValStatement>(name.Lexeme, constant, move(init));
}

unique_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration() {
	Next();		// eat fn keyword
	auto ident = Next();
	if (ident.Type != TokenType::Identifier)
		AddError(ParseError{ ParseErrorType::IdentifierExpected, ident });

	Match(TokenType::Operator_OpenParen, true, true);

	//
	// get list of arguments
	//
	vector<Parameter> parameters;
	while (Peek().Type != TokenType::Operator_CloseParen) {
		auto param = Next();
		if (param.Type != TokenType::Identifier)
			AddError(ParseError{ ParseErrorType::IdentifierExpected, ident });
		parameters.push_back(Parameter{ param.Lexeme });
		Match(TokenType::Operator_Comma);
	}

	Next();		// eat close paren
	if (parameters.size() > 127)
		AddError(ParseError{ ParseErrorType::TooManyFunctionArgs, Peek() });

	auto sym = FindSymbol(format("{}/{}", ident.Lexeme, parameters.size()), true);
	if (sym)
		AddError(ParseError(ParseErrorType::DuplicateDefinition, ident));

	auto decl = make_unique<FunctionDeclaration>(move(ident.Lexeme));

	unique_ptr<Expression> body;
	if (Match(TokenType::Operator_GoesTo))
		body = ParseExpression();
	else
		body = ParseBlock(parameters);
	
	body->SetParent(decl.get());
	auto params = parameters.size();
	decl->Parameters(move(parameters));
	decl->Body(move(body));

	if (sym == nullptr) {
		Symbol sym;
		sym.Name = format("{}/{}", decl->Name(), params);
		sym.Type = SymbolType::Function;
		sym.Flags = SymbolFlags::None;
		sym.Ast = decl.get();
		AddSymbol(sym);
	}
	return decl;
}

bool Parser::AddSymbol(Symbol sym) {
	return m_Symbols.top()->AddSymbol(move(sym));
}

Symbol const* Parser::FindSymbol(string const& name, bool localOnly) const {
	return m_Symbols.top()->FindSymbol(name, localOnly);
}

void Parser::PushScope(AstNode* node) {
	m_Symbols.push(node->Symbols());
}

void Parser::PopScope() {
	m_Symbols.pop();
}

unique_ptr<Statements> Parser::ParseBlock(vector<Parameter> const& args) {
	Match(TokenType::Operator_OpenBrace, true, true);

	auto block = make_unique<Statements>();
	PushScope(block.get());

	for (auto& arg : args) {
		Symbol sym;
		sym.Name = arg.Name;
		sym.Flags = SymbolFlags::None;
		sym.Type = SymbolType::Argument;
		AddSymbol(sym);
	}

	while (Peek().Type != TokenType::Operator_CloseBrace) {
		auto stmt = ParseStatement();
		if (!stmt)
			break;
		stmt->SetParent(block.get());
		block->Add(move(stmt));
	}
	Match(TokenType::Operator_CloseBrace, true, true);
	PopScope();
	return block;
}

unique_ptr<Statement> Parser::ParseStatement(bool topLevel) {
	auto peek = Peek();
	if (peek.Type == TokenType::Error) {
		AddError(ParseError{ ParseErrorType::Syntax, peek });
		return nullptr;
	}
	if (peek.Type == TokenType::End)
		return nullptr;

	switch (peek.Type) {
		case TokenType::Keyword_Var: return ParseVarConstStatement(false);
		case TokenType::Keyword_Val: return ParseVarConstStatement(true);
			//case TokenType::Keyword_Repeat: return ParseRepeatStatement();
		case TokenType::Keyword_While:
			if (!topLevel)
				return ParseWhileStatement();
			break;

		case TokenType::Keyword_Fn: return ParseFunctionDeclaration();
		case TokenType::Keyword_Return:
			if (!topLevel)
				return ParseReturnStatement();
			break;
		case TokenType::Keyword_Break:
		case TokenType::Keyword_Continue:
			if (!topLevel)
				return ParseBreakContinueStatement(peek.Type == TokenType::Keyword_Continue);
			break;

			//case TokenType::Keyword_For: return ParseForStatement();
		case TokenType::Keyword_Enum: return ParseEnumDeclaration();
		case TokenType::Operator_OpenBrace:
			if (!topLevel)
				return ParseBlock();
			break;
		case TokenType::Operator_Semicolon:
			if (!topLevel) {
				Next();		// eat semicolon empty statement
				return ParseStatement();
			}
			break;
		default:
			if (!topLevel) {
				//auto it = m_PrefixParslets.find(peek.Type);
				//if (it != m_PrefixParslets.end())
				//	return std::make_unique<ExpressionStatement>(it->second->Parse(*this, peek));

				auto expr = ParseExpression();
				if (expr) {
					return std::make_unique<ExpressionStatement>(move(expr));
				}
			}
	}
	AddError(ParseError(ParseErrorType::InvalidStatement, peek));
	return nullptr;
}

unique_ptr<BreakOrContinueStatement> Parser::ParseBreakContinueStatement(bool cont) {
	Next();		// eat keyword
	if (!Match(TokenType::Operator_Semicolon))
		AddError(ParseError(ParseErrorType::SemicolonExpected, Peek()));
	if (m_LoopCount == 0)
		AddError(ParseError(ParseErrorType::BreakContinueNoLoop, Peek()));

	return make_unique<BreakOrContinueStatement>(cont);
}

unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
	Next();	// eat "while"
	auto cond = ParseExpression();
	if (cond == nullptr)
		AddError(ParseError(ParseErrorType::ConditionExpressionExpected, Peek()));
	m_LoopCount++;
	auto block = ParseBlock();
	m_LoopCount--;
	return make_unique<WhileStatement>(move(cond), move(block));
}

unique_ptr<EnumDeclaration> Parser::ParseEnumDeclaration() {
	Next();		// eat enum
	auto name = Next();
	if (name.Type != TokenType::Identifier) {
		AddError(ParseError(ParseErrorType::IdentifierExpected, name, "Expected identifier after 'enum'"));
		SkipTo(TokenType::Operator_CloseBrace);
		return nullptr;
	}
	auto sym = FindSymbol(name.Lexeme);
	if (sym) {
		AddError(ParseError(ParseErrorType::DuplicateDefinition, name, "Idenitifier already defined in current scope"));
	}

	unordered_map<string, long long> values;
	Match(TokenType::Operator_OpenBrace, true, true);

	long long current = 0;
	while (Peek().Type != TokenType::Operator_CloseBrace && Peek().Type != TokenType::Invalid) {
		auto next = Next();
		bool error = false;
		if (next.Type != TokenType::Identifier) {
			AddError(ParseError(ParseErrorType::IdentifierExpected, name, "Expected: identifier"));
			error = true;
		}
		if (values.find(next.Lexeme) != values.end()) {
			AddError(ParseError(ParseErrorType::DuplicateDefinition, name, format("Duplicate enum value '{}'", next.Lexeme)));
			error = true;
		}
		if (Match(TokenType::Operator_Assign)) {
			auto value = ParseExpression();
			if (value == nullptr || value->Type() != AstNodeType::Literal) {
				AddError(ParseError(ParseErrorType::IllegalExpression, Peek(), "Expression must be constant"));
				error = true;
			}
			else {
				auto literal = reinterpret_cast<LiteralExpression*>(value.get());
				if (literal->Literal().Type != TokenType::Integer) {
					AddError(ParseError(ParseErrorType::IllegalExpression, Peek(), "Expression must be an Integer"));
					error = true;
				}
				else
					current = strtoll(literal->Literal().Lexeme.c_str(), nullptr, 0);
			}
		}
		if (!error)
			values.insert({ move(next.Lexeme), current });
		current++;
		Match(TokenType::Operator_Comma, true, Peek().Type != TokenType::Operator_CloseBrace);
	}
	Next();		// consume close brace
	if (sym)
		return nullptr;

	auto decl = make_unique<EnumDeclaration>(move(name.Lexeme), move(values));
	{
		Symbol sym;
		sym.Name = decl->Name();
		sym.Type = SymbolType::Enum;
		sym.Flags = SymbolFlags::None;
		AddSymbol(sym);
	}

	return decl;
}

unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
	Next();		// eat return keyword
	auto expr = ParseExpression();
	Match(TokenType::Operator_Semicolon, true, true);
	return make_unique<ReturnStatement>(move(expr));
}
