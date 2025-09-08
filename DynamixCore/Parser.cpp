#include "Parser.h"
#include "AstNode.h"
#include <format>
#include <fstream>
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace Dynamix;

Parser::Parser(Tokenizer& t) : m_Tokenizer(t) {
	m_Symbols.push(&m_GlobalSymbols);
}

bool Parser::Init() {
	if (!m_InfixParslets.empty())
		return true;

	if (!m_Tokenizer.AddTokens({
		{ "if", TokenType::If },
		{ "while", TokenType::While },
		{ "fn", TokenType::Fn },
		{ "else", TokenType::Else },
		{ "var", TokenType::Var },
		{ "val", TokenType::Val },
		{ "const", TokenType::Const },
		{ "true", TokenType::True },
		{ "false", TokenType::False },
		{ "for", TokenType::For },
		{ "repeat", TokenType::Repeat },
		{ "break", TokenType::Break },
		{ "continue", TokenType::Continue },
		{ "return", TokenType::Return },
		{ "do", TokenType::Do },
		{ "foreach", TokenType::ForEach },
		{ "new", TokenType::New },
		{ "in", TokenType::In },
		{ "interface", TokenType::Interface },
		{ "class", TokenType::Class },
		{ "object", TokenType::Object },
		{ "enum", TokenType::Enum },
		{ "struct", TokenType::Struct },
		{ "and", TokenType::And },
		{ "or", TokenType::Or },
		{ "breakout", TokenType::BreakOut },
		{ "match", TokenType::Match },
		{ "this", TokenType::This },

		{ "$include", TokenType::MetaInclude },
		{ "$default", TokenType::MetaDefault },

		{ "(", TokenType::OpenParen },
		{ ")", TokenType::CloseParen },
		{ "{", TokenType::OpenBrace },
		{ "}", TokenType::CloseBrace },
		{ ">", TokenType::GreaterThan },
		{ "<", TokenType::LessThan },
		{ ">=", TokenType::GreaterThanOrEqual },
		{ "<=", TokenType::LessThanOrEqual },
		{ "!=", TokenType::NotEqual },
		{ "==", TokenType::Equal },
		{ "=", TokenType::Assign },
		{ "=>", TokenType::GoesTo },
		{ ",", TokenType::Comma },
		{ "|", TokenType::BitwiseOr },
		{ "&", TokenType::BitwiseAnd },
		{ ";", TokenType::Semicolon },
		{ ":", TokenType::Colon },
		{ "[", TokenType::OpenBracket },
		{ "]", TokenType::CloseBracket },
		{ ".", TokenType::Dot },
		{ "::", TokenType::DoubleColon },
		{ "+", TokenType::Plus },
		{ "-", TokenType::Minus },
		{ "*", TokenType::Mul },
		{ "/", TokenType::Div },
		{ "%", TokenType::Mod },
		{ "+=", TokenType::Assign_Add },
		{ "-=", TokenType::Assign_Sub },
		{ "*=", TokenType::Assign_Mul },
		{ "/=", TokenType::Assign_Div },
		{ "%=", TokenType::Assign_Mod },
		{ "&=", TokenType::Assign_And },
		{ "|=", TokenType::Assign_Or },
		{ "^=", TokenType::Assign_Xor },
		}))
		return false;

	AddParslet(TokenType::And, make_unique<BinaryOperatorParslet>(80));
	AddParslet(TokenType::Or, make_unique<BinaryOperatorParslet>(70));
	AddParslet(TokenType::Not, make_unique<PrefixOperatorParslet>(90));
	AddParslet(TokenType::Plus, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Minus, make_unique<BinaryOperatorParslet>(100));
	AddParslet(TokenType::Mul, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Div, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Mod, make_unique<BinaryOperatorParslet>(200));
	AddParslet(TokenType::Minus, make_unique<PrefixOperatorParslet>(300));
	AddParslet(TokenType::Integer, make_unique<LiteralParslet>());
	AddParslet(TokenType::String, make_unique<LiteralParslet>());
	AddParslet(TokenType::True, make_unique<LiteralParslet>());
	AddParslet(TokenType::False, make_unique<LiteralParslet>());
	AddParslet(TokenType::Real, make_unique<LiteralParslet>());
	AddParslet(TokenType::Identifier, make_unique<NameParslet>());
	AddParslet(TokenType::This, make_unique<NameParslet>(true));
	AddParslet(TokenType::OpenParen, make_unique<GroupParslet>());
	AddParslet(TokenType::Power, make_unique<BinaryOperatorParslet>(350, true));
	AddParslet(TokenType::Assign, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_Add, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_Sub, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_Mul, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_Div, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_Mod, make_unique<AssignParslet>());
	AddParslet(TokenType::Assign_And, make_unique<AssignParslet>());

	AddParslet(TokenType::Equal, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::NotEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::LessThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThan, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::GreaterThanOrEqual, make_unique<BinaryOperatorParslet>(90));
	AddParslet(TokenType::OpenParen, make_unique<InvokeFunctionParslet>());
	AddParslet(TokenType::If, make_unique<IfThenElseParslet>());
	AddParslet(TokenType::BitwiseAnd, make_unique<BinaryOperatorParslet>(400));
	AddParslet(TokenType::BitwiseOr, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::BitwiseXor, make_unique<BinaryOperatorParslet>(390));
	AddParslet(TokenType::Fn, make_unique<AnonymousFunctionParslet>());
	AddParslet(TokenType::BitwiseNot, make_unique<PrefixOperatorParslet>(500));
	AddParslet(TokenType::OpenBracket, make_unique<ArrayExpressionParslet>());
	AddParslet(TokenType::Dot, make_unique<GetMemberParslet>());
	AddParslet(TokenType::OpenBracket, make_unique<ArrayAccessParslet>());
	AddParslet(TokenType::New, make_unique<NewOperatorParslet>());
	AddParslet(TokenType::Colon, make_unique<EnumValueParslet>());

	return true;
}

std::unique_ptr<Statements> Parser::Parse(string_view text, bool repl, int line) {
	m_Repl = repl;
	if (!m_Tokenizer.Tokenize(text, line))
		return nullptr;

	m_NextToken.Type = TokenType::Invalid;
	m_Errors.clear();
	return DoParse();
}

std::unique_ptr<Statements> Parser::ParseFile(std::string_view filename) {
	error_code ec;
	auto len = filesystem::file_size(filename, ec);
	if (ec.value())
		return nullptr;
	ifstream stm(filename.data());
	if (!stm.good())
		return nullptr;

	auto text = std::make_unique<char[]>(len);
	stm.read(text.get(), len);
	auto file = m_CurrentFile;
	m_CurrentFile = filename;
	auto node = Parse(text.get());
	m_CurrentFile = file;
	return node;
}

unique_ptr<Statements> Parser::ParseFiles(std::initializer_list<std::string_view> filenames) {
	auto stmts = make_unique<Statements>();
	for (auto& file : filenames) {
		auto ast = ParseFile(file);
		if (ast)
			stmts->Append(move(ast));
	}
	return stmts;
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
	auto next = m_Tokenizer.Next();
	m_NextToken.Type = TokenType::Invalid;
	return next;
}

Token const& Parser::Peek() const {
	if (m_NextToken.Type == TokenType::Invalid)
		m_NextToken = m_Tokenizer.Peek();
	return m_NextToken;
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
	AddError(ParseError(ParseErrorType::UnknownOperator, token, format("Unexpected token: {}", token.Lexeme)));
	Next();
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

unique_ptr<Statement> Parser::ParseVarConstStatement(bool constant, SymbolFlags extraFlags) {
	auto next = Next();		// eat var/val

	auto stmts = make_unique<Statements>();
	do {
		auto name = Next();		// variable name
		if (name.Type != TokenType::Identifier)
			AddError(ParseError(ParseErrorType::IdentifierExpected, name, "Identifier expected"));

		bool dup = false;
		{
			auto sym = FindSymbol(name.Lexeme, true);
			if (sym) {
				AddError(ParseError{ ParseErrorType::DuplicateDefinition, name, format("Symbol {} already defined in scope", name.Lexeme) });
				dup = true;
			}
		}
		unique_ptr<Expression> init;
		if (Match(TokenType::Assign)) {
			init = ParseExpression();
		}
		else if (constant) {
			AddError(ParseError(ParseErrorType::MissingInitExpression, Peek()));
		}

		if (!dup) {
			Symbol sym;
			sym.Name = name.Lexeme;
			sym.Type = SymbolType::Element;
			sym.Flags = extraFlags | (constant ? SymbolFlags::Const : SymbolFlags::None);
			AddSymbol(sym);
			stmts->Add(make_unique<VarValStatement>(name.Lexeme, constant, move(init)));
		}
		if (Match(TokenType::Semicolon))
			break;
	} while (Match(TokenType::Comma, true, true));

	return stmts->Count() == 0 ? nullptr : (stmts->Count() == 1 ? move(stmts->RemoveAt(0)) : move(stmts));
}

unique_ptr<FunctionDeclaration> Parser::ParseFunctionDeclaration(bool method, SymbolFlags extraFlags) {
	bool ctor = method && Peek().Type == TokenType::New;
	Token ident = Next();
	if (!ctor) {
		ident = Next();
		if (ident.Type != TokenType::Identifier)
			AddError(ParseError(ParseErrorType::IdentifierExpected, CodeLocation::FromToken(ident), "Expected: identifier"));
	}

	Match(TokenType::OpenParen, true, true);

	bool staticCtor = ctor && ((extraFlags & SymbolFlags::Static) == SymbolFlags::Static);
	int isInst = method && (extraFlags & SymbolFlags::Static) == SymbolFlags::None ? 1 : 0;

	// get list of arguments
	vector<Parameter> parameters;
	while (Peek().Type != TokenType::CloseParen) {
		auto param = Next();
		if (param.Type != TokenType::Identifier)
			AddError(ParseError{ ParseErrorType::IdentifierExpected, param });
		parameters.push_back(Parameter{ param.Lexeme });
		Match(TokenType::Comma);
	}

	Next();		// eat close paren
	if(staticCtor && !parameters.empty())
		AddError(ParseError(ParseErrorType::TooManyFunctionArgs, Peek(), "Class constructor cannot have any parameters"));
	if (parameters.size() > 63)
		AddError(ParseError(ParseErrorType::TooManyFunctionArgs, Peek(), "Too many parameters to function/method"));

	auto sym = FindSymbol(format("{}/{}", ident.Lexeme, parameters.size() + isInst), true);
	if (sym)
		AddError(ParseError(ParseErrorType::DuplicateDefinition, ident, format("Duplicate definition of '{}'", sym->Name) ));

	auto decl = make_unique<FunctionDeclaration>(move(ident.Lexeme), method, (extraFlags & SymbolFlags::Static) == SymbolFlags::Static);

	unique_ptr<Expression> body;
	if (Match(TokenType::GoesTo)) {
		body = ParseExpression();
		Match(TokenType::Semicolon, true, true);
	}
	else {
		body = ParseBlock(parameters);
	}

	auto params = parameters.size();
	decl->SetParameters(move(parameters));
	decl->SetBody(move(body));

	if (sym == nullptr) {
		Symbol sym;
		sym.Name = format("{}/{}", decl->Name(), decl->Parameters().size() + isInst);
		sym.Type = method ? SymbolType::Method : SymbolType::Function;
		sym.Flags = extraFlags;
		AddSymbol(sym);
	}
	return decl;
}

unique_ptr<RepeatStatement> Parser::ParseRepeatStatement() {
	Next();		// eat repeat keyword
	bool empty = Match(TokenType::OpenBrace, false);
	auto times = empty ? make_unique<LiteralExpression>(Token { TokenType::True }) : ParseExpression();
	if (!times)
		return nullptr;
	auto body = ParseBlock();
	if (!body)
		return nullptr;
	return make_unique<RepeatStatement>(move(times), move(body));
}

bool Parser::AddSymbol(Symbol sym) {
	return m_Symbols.top()->AddSymbol(move(sym));
}

Symbol const* Parser::FindSymbol(string const& name, bool localOnly) const {
	return m_Symbols.top()->FindSymbol(name, localOnly);
}

vector<Symbol const*> Parser::GlobalSymbols() const {
	return m_GlobalSymbols.EnumSymbols();
}

void Parser::PushScope(AstNode* node) {
	m_Symbols.push(node->Symbols());
}

void Parser::PopScope() {
	m_Symbols.pop();
}

unique_ptr<Statements> Parser::ParseBlock(vector<Parameter> const& args, bool newscope) {
	Match(TokenType::OpenBrace, true, true);

	auto block = make_unique<Statements>();
	if(newscope)
		PushScope(block.get());

	for (auto& arg : args) {
		Symbol sym;
		sym.Name = arg.Name;
		sym.Flags = SymbolFlags::None;
		sym.Type = SymbolType::Argument;
		AddSymbol(sym);
	}

	while (Peek().Type != TokenType::CloseBrace) {
		auto peek = Peek();
		auto stmt = ParseStatement();
		if (m_Errors.size() > 10)
			break;
		if (!stmt)
			continue;
		stmt->SetLocation(CodeLocation{ m_CurrentFile, peek.Line, peek.Col });
		stmt->SetParent(block.get());
		block->Add(move(stmt));
	}
	Match(TokenType::CloseBrace, true, true);
	if (newscope)
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
		case TokenType::Var: return ParseVarConstStatement(false);
		case TokenType::Val: return ParseVarConstStatement(true);
		case TokenType::Repeat:
			if (!topLevel)
				return ParseRepeatStatement();
			break;
		case TokenType::ForEach:
			if (!topLevel)
				return ParseForEachStatement();
			break;

		case TokenType::While:
			if (!topLevel)
				return ParseWhileStatement();
			break;

		case TokenType::Fn: return ParseFunctionDeclaration();
		case TokenType::Return:
			if (!topLevel)
				return ParseReturnStatement();
			break;
		case TokenType::Break:
		case TokenType::Continue:
		case TokenType::BreakOut:
			if (!topLevel)
				return ParseBreakContinueStatement();
			break;

		case TokenType::Class: return ParseClassDeclaration();
		case TokenType::For:
			if (!topLevel)
				return ParseForStatement();
			break;
		case TokenType::Enum: return ParseEnumDeclaration();
		case TokenType::OpenBrace:
			if (!topLevel)
				return ParseBlock();
			break;
		case TokenType::Semicolon:
			if (!topLevel) {
				Next();		// eat semicolon empty statement
				return ParseStatement();
			}
			break;
		default:
			if (!topLevel) {
				auto expr = ParseExpression();
				if (expr) {
					bool semi = Match(TokenType::Semicolon);
					return std::make_unique<ExpressionStatement>(move(expr), semi);
				}
			}
	}
	AddError(ParseError(ParseErrorType::InvalidStatement, peek));
	return nullptr;
}

unique_ptr<BreakOrContinueStatement> Parser::ParseBreakContinueStatement() {
	auto token = Next();		// eat keyword
	Match(TokenType::Semicolon, true, true);

	if (m_LoopCount == 0)
		AddError(ParseError(ParseErrorType::BreakContinueNoLoop, token));

	return make_unique<BreakOrContinueStatement>(token.Type);
}

unique_ptr<WhileStatement> Parser::ParseWhileStatement() {
	Next();	// eat "while"
	Match(TokenType::OpenParen, true, true);
	auto cond = ParseExpression();
	if (cond == nullptr)
		AddError(ParseError(ParseErrorType::ConditionExpressionExpected, Peek()));
	Match(TokenType::CloseParen, true, true);
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
		SkipTo(TokenType::CloseBrace);
		return nullptr;
	}
	auto sym = FindSymbol(name.Lexeme, true);
	if (sym) {
		AddError(ParseError(ParseErrorType::DuplicateDefinition, name, "Idenitifier already defined in current scope"));
	}

	unordered_map<string, long long> values;
	Match(TokenType::OpenBrace, true, true);

	long long current = 0;
	while (Peek().Type != TokenType::CloseBrace && Peek().Type != TokenType::Invalid) {
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
		if (Match(TokenType::Assign)) {
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
		Match(TokenType::Comma, true, Peek().Type != TokenType::CloseBrace);
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

unique_ptr<ForStatement> Parser::ParseForStatement() {
	Next();		// eat for
	bool open = Match(TokenType::OpenParen);

	auto stmt = make_unique<ForStatement>();
	PushScope(stmt.get());
	auto init = ParseStatement();
	auto whileExpr = ParseExpression();
	Match(TokenType::Semicolon, true, true);

	auto inc = ParseExpression();
	if(open)
		Match(TokenType::CloseParen, true, true);

	m_LoopCount++;
	auto body = ParseBlock({}, false);
	m_LoopCount--;
	PopScope();

	stmt->SetInit(move(init));
	stmt->SetWhile(move(whileExpr));
	stmt->SetBody(move(body));
	stmt->SetInc(move(inc));
	return stmt;
}

unique_ptr<ClassDeclaration> Parser::ParseClassDeclaration() {
	Next();		// eat class keyword
	auto name = Next();
	if (name.Type != TokenType::Identifier)
		AddError(ParseError(ParseErrorType::Expected, CodeLocation::FromToken(name), "Expected: identifier"));

	if (Match(TokenType::Colon)) {
		auto baseType = Next();
	}

	Match(TokenType::OpenBrace, true, true);
	auto decl = make_unique<ClassDeclaration>(move(name.Lexeme));
	PushScope(decl.get());
	vector<unique_ptr<FunctionDeclaration>> methods;
	vector<unique_ptr<Statement>> fields;
	auto extraFlags = SymbolFlags::None;

	while (Peek().Type != TokenType::CloseBrace) {
		bool val = false;
		switch (Peek().Type) {
			case TokenType::New:	// ctor
			case TokenType::Fn:
			{
				auto method = ParseFunctionDeclaration(true, extraFlags);
				if (method) {
					methods.push_back(move(method));
				}
				extraFlags = SymbolFlags::None;
				break;
			}

			case TokenType::Val:
				val = true;
				[[fallthrough]];
			case TokenType::Var:
			{
				auto stmt = ParseVarConstStatement(val, extraFlags);
				fields.push_back(move(stmt));
				extraFlags = SymbolFlags::None;
				break;
			}
			case TokenType::Class:
				Next();		// eat class keyword
				extraFlags = SymbolFlags::Static;
				break;

			default:
				AddError(ParseError(ParseErrorType::UnexpectedToken, CodeLocation::FromToken(Peek()), format("Unexpected token: '{}'", Peek().Lexeme)));
				Next();
				break;
		}
	}
	Next();		// eat close brance
	PopScope();
	decl->SetMethods(move(methods));
	decl->SetFields(move(fields));
	return move(decl);
}

unique_ptr<ForEachStatement> Parser::ParseForEachStatement() {
	Next();		// eat foreach
	bool openParen = Match(TokenType::OpenParen);	// optional
	auto ident = Next();
	if (ident.Type != TokenType::Identifier)
		AddError(ParseError(ParseErrorType::IdentifierExpected, CodeLocation::FromToken(ident), "Expected identifier after 'foreach'"));

	Match(TokenType::In, true, true);
	auto collection = ParseExpression();
	if (openParen)
		Match(TokenType::CloseParen, true, true);

	return make_unique<ForEachStatement>(ident.Lexeme, move(collection), ParseBlock());
}

unique_ptr<ReturnStatement> Parser::ParseReturnStatement() {
	Next();		// eat return keyword
	auto expr = ParseExpression();
	Match(TokenType::Semicolon, true, true);
	return make_unique<ReturnStatement>(move(expr));
}
