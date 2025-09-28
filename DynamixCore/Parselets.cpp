#include <memory>
#include <cassert>

#include "Parselets.h"
#include "Parser.h"
#include "AstNode.h"
#include "ParseError.h"
#include "SymbolTable.h"
#include "ArrayType.h"

using namespace Dynamix;
using namespace std;

PostfixOperatorParslet::PostfixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

BinaryOperatorParslet::BinaryOperatorParslet(int precedence, bool right) : m_Precedence(precedence), m_RightAssoc(right) {
}

PrefixOperatorParslet::PrefixOperatorParslet(int precedence) : m_Precedence(precedence) {
}

int BinaryOperatorParslet::Precedence() const noexcept {
	return m_Precedence;
}

int PrefixOperatorParslet::Precedence() const noexcept {
	return m_Precedence;
}

int PostfixOperatorParslet::Precedence() const noexcept {
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
	return make_unique<NameExpression>(move(name), move(ns));
}

unique_ptr<Expression> PrefixOperatorParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<UnaryExpression>(token, parser.ParseExpression(m_Precedence));
}

unique_ptr<Expression> LiteralParslet::Parse(Parser& parser, Token const& token) {
	return make_unique<LiteralExpression>(Value::FromToken(token));
}

unique_ptr<Expression> BinaryOperatorParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(m_Precedence - (m_RightAssoc ? 1 : 0));
	return make_unique<BinaryExpression>(move(left), token, move(right));
}

int GroupParslet::Precedence() const noexcept {
	return 1000;
}

unique_ptr<Expression> GroupParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen);
	return expr;
}

unique_ptr<Expression> AssignParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto right = parser.ParseExpression(Precedence() - 1);
	if (left->NodeType() == AstNodeType::Name)
		return make_unique<AssignExpression>(reinterpret_cast<NameExpression const*>(left.get())->Name(), move(right), token);

	if (left->NodeType() == AstNodeType::ArrayAccess) {
		return make_unique<AssignArrayIndexExpression>(move(left), move(right), token);
	}
	if (left->NodeType() == AstNodeType::GetMember) {
		return make_unique<AssignFieldExpression>(move(left), move(right), token);
	}

	parser.AddError(ParseError(ParseErrorType::InvalidLhs, left->Location(), "Illegal Left handl side of an assignment"));
	return nullptr;
}

int AssignParslet::Precedence() const noexcept {
	return 2;
}

InvokeFunctionParslet::InvokeFunctionParslet() : PostfixOperatorParslet(1200) {
}

unique_ptr<Expression> InvokeFunctionParslet::Parse(Parser& parser, unique_ptr<Expression> left, Token const& token) {
	auto next = parser.Peek();
	vector<unique_ptr<Expression>> args;
	while (next.Type != TokenType::CloseParen) {
		auto arg = parser.ParseExpression();
		if (!arg)
			return nullptr;
		args.push_back(move(arg));
		if (!parser.Match(TokenType::Comma) && !parser.Match(TokenType::CloseParen, false))
			parser.AddError(ParseError(ParseErrorType::CommaExpected, next, "Expected , or )"));
		next = parser.Peek();
	}
	parser.Match(TokenType::CloseParen, true, true);
	return make_unique<InvokeFunctionExpression>(move(left), move(args));
}

unique_ptr<Expression> IfThenElseParslet::Parse(Parser& parser, Token const& token) {
	auto cond = parser.ParseExpression();
	auto then = parser.ParseBlock();
	unique_ptr<Statement> elseExpr;
	if (parser.Match(TokenType::Else))
		elseExpr = parser.ParseBlock();
	return make_unique<IfThenElseExpression>(move(cond), move(then), move(elseExpr));
}

unique_ptr<Expression> AnonymousFunctionParslet::Parse(Parser& parser, Token const& token) {
	assert(token.Type == TokenType::Fn);
	parser.Match(TokenType::OpenParen, true, true);

	// parse args
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
	return make_unique<AnonymousFunctionExpression>(move(args), make_unique<ExpressionStatement>(move(block), false));
}

int AnonymousFunctionParslet::Precedence() const {
	return 2000;
}

unique_ptr<Expression> ArrayExpressionParslet::Parse(Parser& parser, Token const& token) {
	auto array = make_unique<ArrayExpression>();
	while (parser.Peek().Type != TokenType::CloseBracket) {
		auto expr = parser.ParseExpression();
		array->Add(move(expr));
		if (!parser.Match(TokenType::Comma) && parser.Peek().Type != TokenType::CloseBracket)
			parser.AddError(ParseError(ParseErrorType::Expected, CodeLocation::FromToken(parser.Peek()), "Expected: ,"));
	}
	parser.Next();
	return array;
}

unique_ptr<Expression> GetMemberParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	assert(token.Type == TokenType::Dot || token.Type == TokenType::DoubleColon);
	auto next = parser.Next();
	return std::make_unique<GetMemberExpression>(move(left), next.Lexeme, token);
}

unique_ptr<Expression> ArrayAccessParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	assert(token.Type == TokenType::OpenBracket);
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseBracket, true, true);
	return make_unique<AccessArrayExpression>(move(left), move(expr));
}

unique_ptr<Expression> NewOperatorParslet::Parse(Parser& parser, Token const& token) {
	assert(token.Type == TokenType::New);
	auto ident = parser.Next();
	if (ident.Type != TokenType::Identifier) {
		parser.AddError(ParseError(ParseErrorType::IllegalExpression, CodeLocation::FromToken(ident), "Class name expected after 'new'"));
		return nullptr;
	}
	while (parser.Peek().Type == TokenType::DoubleColon) {
		ident.Lexeme += "::";
		parser.Next();
		if (!parser.Match(TokenType::Identifier, false, true))
			break;
		ident.Lexeme += parser.Next().Lexeme;
	}

	vector<unique_ptr<Expression>> args;
	// allow initializers without open/close parens (means default ctor)
	if (parser.Peek().Type != TokenType::OpenBrace) {
		parser.Match(TokenType::OpenParen, true, true);
		auto next = parser.Peek();
		while (next.Type != TokenType::CloseParen) {
			auto param = parser.ParseExpression();
			args.push_back(move(param));
			if (!parser.Match(TokenType::Comma) && !parser.Match(TokenType::CloseParen, false)) {
				parser.AddError(ParseError(ParseErrorType::CommaExpected, next, "Expected , or )"));
				return nullptr;
			}
			next = parser.Peek();
		}
		parser.Match(TokenType::CloseParen, true, true);
	}

	vector<FieldInitializer> inits;
	if (parser.Peek().Type == TokenType::OpenBrace) {
		parser.Next();
		while (parser.Peek().Type != TokenType::CloseBrace) {
			parser.Match(TokenType::Dot, true, true);
			if (!parser.Match(TokenType::Identifier, false, true))
				break;
			auto field = parser.Next().Lexeme;
			parser.Match(TokenType::Assign, true, true);
			auto init = parser.ParseExpression();
			if (!init) {
				parser.AddError(ParseError(ParseErrorType::MissingInitExpression, parser.Location(), "Missing init expression"));
				parser.SkipTo(TokenType::CloseBrace);
				return nullptr;
			}
			inits.push_back(FieldInitializer{ move(field), move(init) });
			if (!parser.Match(TokenType::Comma) && parser.Peek().Type != TokenType::CloseBrace) {
				parser.AddError(ParseError(ParseErrorType::Expected, parser.Location(), "Expected: ',' or '}'"));
			}
		}
		parser.Next();
	}

	return make_unique<NewObjectExpression>(move(ident.Lexeme), move(args), move(inits));
}

unique_ptr<Expression> MatchParslet::Parse(Parser& parser, Token const& token) {
	auto expr = parser.ParseExpression();
	if (!expr)
		return nullptr;

	parser.Match(TokenType::OpenBrace, true, true);

	auto next = parser.Peek();
	std::vector<MatchCaseExpression> cases;
	auto match = make_unique<MatchExpression>(move(expr));
	while (next.Type != TokenType::CloseBrace) {
		switch (next.Type) {
			case TokenType::Case:
			{
				parser.Next();		// eat case
				std::vector<std::unique_ptr<Expression>> exprs;
				while (parser.Peek().Type != TokenType::Colon) {
					auto expr = parser.ParseExpression();
					if (!expr)
						break;
					exprs.push_back(move(expr));
					if (parser.Match(TokenType::Comma))
						continue;
				}
				assert(parser.Peek().Type == TokenType::Colon);
				parser.Next();		// eat colon
				auto action = parser.ParseStatementsForMatch(true);
				MatchCaseExpression mce(move(action));
				mce.SetCases(move(exprs));
				match->AddCase(move(mce));
				break;
			}

			case TokenType::Default:
			{
				parser.Next();		// eat default
				parser.Match(TokenType::Colon, true, true);
				auto action = parser.ParseStatementsForMatch(true);
				match->AddCase(MatchCaseExpression(move(action)));
				break;
			}

			default:
				parser.AddError(ParseError(ParseErrorType::UnexpectedToken, CodeLocation::FromToken(next), "Expected 'case' or 'default'"));
				parser.Next();
				break;
		}
		next = parser.Peek();
	}
	parser.Next();
	return match;
}

unique_ptr<Expression> RangeParslet::Parse(Parser& parser, std::unique_ptr<Expression> left, Token const& token) {
	assert(token.Type == TokenType::DotDot || token.Type == TokenType::DotDotInclusive);
	auto end = parser.ParseExpression();
	if (!end)
		return nullptr;

	return make_unique<RangeExpression>(move(left), move(end), token.Type == TokenType::DotDotInclusive);
}

unique_ptr<Expression> TypeOfParslet::Parse(Parser& parser, Token const& token) {
	assert(token.Type == TokenType::TypeOf);
	parser.Match(TokenType::OpenParen, true, true);
	auto expr = parser.ParseExpression();
	parser.Match(TokenType::CloseParen, true, true);
	return make_unique<UnaryExpression>(token, move(expr));
}
