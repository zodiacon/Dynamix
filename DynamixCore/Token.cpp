#include "Token.h"
#include "EnumClassBitwise.h"

std::string_view Dynamix::Token::TypeToString() const noexcept {
	switch (Type) {
		case TokenType::Invalid: return "Invalid";
		case TokenType::Error: return "Error";
		case TokenType::Integer: return "Integer";
		case TokenType::Real: return "Real";
		case TokenType::String: return "String";
		case TokenType::Identifier: return "Identifier";
		case TokenType::Operator: return "Operator";
		case TokenType::Comment: return "Comment";
		case TokenType::End: return "End";
		case TokenType::Assign: return "=";
		case TokenType::Assign_Add: return "+=";
		case TokenType::Assign_Sub: return "-=";
		case TokenType::Assign_Mul: return "*=";
		case TokenType::Assign_Div: return "/=";
		case TokenType::Assign_Mod: return "%=";
		case TokenType::Assign_And: return "&=";
		case TokenType::Assign_Or: return "|=";
		case TokenType::Assign_Xor: return "^=";
		case TokenType::BitwiseAnd: return "&";
		case TokenType::BitwiseOr: return "|";
		case TokenType::BitwiseXor: return "^";
		case TokenType::BitwiseNot: return "~";
		case TokenType::Semicolon: return ";";
		case TokenType::Colon: return ":";
		case TokenType::Equal: return "==";
		case TokenType::NotEqual: return "!=";
		case TokenType::GreaterThan: return ">";
		case TokenType::LessThan: return "<";
		case TokenType::GreaterThanOrEqual: return ">=";
		case TokenType::LessThanOrEqual: return "<=";
		case TokenType::DotDotExclusive: return "..<";
		case TokenType::DotDot: return "..";
		case TokenType::Ellipsis: return "...";
		case TokenType::DotDotInclusive: return "..=";
		case TokenType::StreamRight: return ">>";
		case TokenType::StreamLeft: return "<<";
	}
	if ((Type & TokenType::Keyword) == TokenType::Keyword)
		return "Keyword";
	if ((Type & TokenType::Operator) == TokenType::Operator)
		return "Operator";
	return "(Unknown)";
}

std::string_view Dynamix::Token::TypeToString(TokenType type) {
	return (Token{ type }).TypeToString();
}

