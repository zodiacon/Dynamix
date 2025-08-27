#pragma once

#include <string>
#include <string_view>

namespace Dynamix {
	enum class TokenType : uint16_t {
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

		If = Keyword,
		Else,
		Fn,
		Return,
		While,
		Null,
		Var,
		Val,
		Const,
		Repeat,
		Interface,
		Class,
		Object,
		True,
		False,
		For,
		ForEach,
		Do,
		New,
		In,
		Break,
		BreakOut,
		Continue,
		And,
		Or,
		Not,
		Enum,
		Struct,
		Match,
		Native,

		Plus = Operator,
		Minus,
		Mul,
		Div,
		Mod,
		Neg,
		Power,
		Xor,
		OpenParen,
		CloseParen,
		OpenBrace,
		CloseBrace,
		OpenBracket,
		CloseBracket,
		Dot,
		DoubleColon,
		Comma,
		GoesTo,
		Assign,
		Assign_Add,
		Assign_Sub,
		Assign_Mul,
		Assign_Div,
		Assign_Mod,
		Assign_And,
		Assign_Or,
		Assign_Xor,
		Assign_Power,
		BitwiseAnd,
		BitwiseOr,
		BitwiseXor,
		BitwiseNot,
		Semicolon,
		Colon,
		Equal,
		NotEqual,
		GreaterThan,
		LessThan,
		GreaterThanOrEqual,
		LessThanOrEqual,

	};

	struct Token final {
		TokenType Type;
		std::string Lexeme;
		int Line;
		uint16_t Col;
		union {
			long long iValue;
			double rValue;
		};
		std::string_view TypeToString() const;
		static std::string_view TypeToString(TokenType type);
	};
}

