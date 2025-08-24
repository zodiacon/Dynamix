#include <memory>
#include <cassert>

#include "Parselets.h"
#include "Parser.h"
#include "AstNode.h"
#include "ParseError.h"
#include "SymbolTable.h"

using namespace Dynamix;
using namespace std;

PostfixOperatorParslet::PostfixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

//unique_ptr<Expression> PostfixOperatorParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
//	return make_unique<PostfixExpression>(move(left), token);
//}

BinaryOperatorParslet::BinaryOperatorParslet(int precedence, bool right) : m_Precedence(precedence), m_RightAssoc(right) {
}

PrefixOperatorParslet::PrefixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

int BinaryOperatorParslet::Precedence() const {
	return m_Precedence;
}

int PrefixOperatorParslet::Precedence() const {
	return m_Precedence;
}

int PostfixOperatorParslet::Precedence() const {
	return m_Precedence;
}

unique_ptr<Expression> NameParslet::Parse(Parser& parser, Token const& token) {
	string ns;
	//while (parser.Peek().Type == TokenType::ScopeRes) {
	//	ns += "::" + parser.Next().Lexeme;
	//	if (parser.Peek().Type != TokenType::Identifier) {
	//		parser.AddError(ParseError(ParseErrorType::IdentifierExpected, parser.Peek(), "Identifier expected after ::"));
	//		break;
	//	}
	//}
	//
	string name = token.Lexeme;
	while (parser.Peek().Type == TokenType::Dot) {
		parser.Next();
		if (parser.Peek().Type != TokenType::Identifier) {
			parser.AddError(ParseError(ParseErrorType::IdentifierExpected, parser.Peek(), "Identifier expected after ::"));
			break;
		}
		name += "." + parser.Next().Lexeme;
	}
	return make_unique<NameExpression>(name, ns);
}

unique_ptr<Expression> PrefixOperatorParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<UnaryExpression>(token, parser.ParseExpression(m_Precedence));
}

unique_ptr<Expression> LiteralParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<LiteralExpression>(token);
}

unique_ptr<Expression> BinaryOperatorParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(m_Precedence - (m_RightAssoc ? 1 : 0));
	return make_unique<BinaryExpression>(move(left), token, move(right));
}

int GroupParslet::Precedence() const {
	return 1000;
}

unique_ptr<Expression> GroupParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen);
	return expr;
}

unique_ptr<Expression> AssignParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	if (left->Type() != AstNodeType::Name)
		parser.AddError(ParseError(ParseErrorType::IllegalExpression, token, "Identifier expected"));

	auto right = parser.ParseExpression(Precedence() - 1);

	return make_unique<AssignExpression>(move(left), move(right), token);
}

int AssignParslet::Precedence() const {
	return 2;
}

InvokeFunctionParslet::InvokeFunctionParslet() : PostfixOperatorParslet(1200) {
}

unique_ptr<Expression> InvokeFunctionParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	//if (left->Type() == AstNodeType::Name)
	//	parser.AddError(ParseError(ParseErrorType::Syntax, token, "Identifier expected"));

	//auto nameExpr = reinterpret_cast<NameExpression*>(left.get());

	auto next = parser.Peek();
	vector<unique_ptr<Expression>> args;
	while (next.Type != TokenType::CloseParen) {
		auto param = parser.ParseExpression();
		args.push_back(move(param));
		if (!parser.Match(TokenType::Comma) && !parser.Match(TokenType::CloseParen, false))
			parser.AddError(ParseError(ParseErrorType::CommaExpected, next, "Expected , or )"));
		next = parser.Peek();
	}
	parser.Match(TokenType::CloseParen, true, true);
	return make_unique<InvokeFunctionExpression>(move(left), move(args));
}

unique_ptr<Expression> IfThenElseParslet::Parse(Parser& parser, Token const& token) {
	parser.Match(TokenType::OpenParen, true, true);
	auto cond = parser.ParseExpression();
	parser.Match(TokenType::CloseParen, true, true);
	auto then = parser.ParseBlock();
	unique_ptr<Statement> elseExpr;
	if (parser.Match(TokenType::Else))
		elseExpr = parser.ParseBlock();
	return make_unique<IfThenElseExpression>(move(cond), move(then), move(elseExpr));
}

unique_ptr<Expression> AnonymousFunctionParslet::Parse(Parser& parser, Token const& token) {
	assert(token.Type == TokenType::Fn);
	parser.Match(TokenType::OpenParen, true, true);

	//
	// parse args
	//
	vector<Parameter> args;
	while (parser.Peek().Type != TokenType::CloseParen) {
		auto arg = parser.Next();
		if (arg.Type != TokenType::Identifier)
			parser.AddError(ParseError(ParseErrorType::IdentifierExpected, arg));
		args.push_back(Parameter{ move(arg.Lexeme) });
		if (parser.Match(TokenType::Comma) || parser.Match(TokenType::CloseParen, false))
			continue;
		parser.AddError(ParseError(ParseErrorType::CommaOrCloseParenExpected, parser.Peek()));
	}
	parser.Next();		// eat close paren
	if (parser.Match(TokenType::GoesTo)) {
		auto expr = parser.ParseExpression();
		return make_unique<AnonymousFunctionExpression>(move(args), move(expr));
	}
	auto block = parser.ParseBlock(args);
	return make_unique<AnonymousFunctionExpression>(move(args), move(block));
}

int AnonymousFunctionParslet::Precedence() const {
	return 2000;
}
