#pragma once

#include <string>
#include <string_view>

namespace Dynamix {
	enum class TokenType {
		Invalid,	
		Integer = 1,	
		Real = 2,
		String = 4,	
		Identifier = 8,
		Error = 0x10,	
		End,
		Comment,

		Keyword		= 0x0100,
		Operator	= 0x1000,

		Keyword_If = Keyword,
		Keyword_Else,
		Keyword_Fn,
		Keyword_Return,
		Keyword_While,
		Keyword_Null,
		Keyword_Var,
		Keyword_Val,
		Keyword_Const,
		Keyword_Repeat,
		Keyword_Interface,
		Keyword_Class,
		Keyword_Object,
		Keyword_True,
		Keyword_False,
		Keyword_For,
		Keyword_ForEach,
		Keyword_Do,
		Keyword_New,
		Keyword_In,
		Keyword_Break,
		Keyword_BreakOut,
		Keyword_Continue,
		Keyword_And,
		Keyword_Or,
		Keyword_Not,
		Keyword_Enum,
		Keyword_Struct,
		Keyword_Native,

		Operator_Plus = Operator,
		Operator_Minus,
		Operator_Mul,
		Operator_Div,
		Operator_Mod,
		Operator_Neg,
		Operator_Power,
		Operator_And,
		Operator_Or,
		Operator_Xor,
		Operator_Not,
		Operator_OpenParen,
		Operator_CloseParen,
		Operator_OpenBrace,
		Operator_CloseBrace,
		Operator_OpenBracket,
		Operator_CloseBracket,
		Operator_Dot,
		Operator_ScopeRes,
		Operator_Comma,
		Operator_GoesTo,
		Operator_Assign,
		Operator_Assign_Add,
		Operator_Assign_Sub,
		Operator_Assign_Mul,
		Operator_Assign_Div,
		Operator_Assign_Mod,
		Operator_Assign_And,
		Operator_Assign_Or,
		Operator_Assign_Xor,
		Operator_Assign_Power,
		Operator_BitwiseAnd,
		Operator_BitwiseOr,
		Operator_BitwiseXor,
		Operator_BitwiseNot,
		Operator_Semicolon,
		Operator_Colon,
		Operator_Equal,
		Operator_NotEqual,
		Operator_GreaterThan,
		Operator_LessThan,
		Operator_GreaterThanOrEqual,
		Operator_LessThanOrEqual,

	};

	struct Token final {
		TokenType Type;
		std::string Lexeme;
		int Line;
		int Col;
		union {
			long long iValue;
			double rValue;
		};
		std::string_view TypeToString() const;
	};
}

