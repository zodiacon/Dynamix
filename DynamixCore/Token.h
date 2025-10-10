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
		StringIndex = 0x20,
		End,
		Comment,

		Keyword		= 0x0100,
		Operator	= 0x1000,
		Meta		= 0x1200,

		If = Keyword,
		Else,
		Case,
		Default,
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
		This,
		Use,
		As,
		With,
		TypeOf,
		Public,
		Private,
		Module,
		Unuse,
		Empty,
		ReadOnly,

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
		DotDot,
		DotDotInclusive,
		StreamRight,
		StreamLeft,

		MetaInclude,
		MetaDefault,
	};

	struct CodeLocation {
		int Line;
		int Col;
		std::string FileName;
	};

	struct Token final {
		TokenType Type{ TokenType::Invalid };
		union {
			const char* Lexeme;
			long long Integer;
			double Real;
		};
		CodeLocation Location;

		std::string_view TypeToString() const;
		static std::string_view TypeToString(TokenType type);

		bool IsValid() const {
			return Type != TokenType::Invalid;
		}
		operator bool() const {
			return IsValid();
		}
		void Clear() {
			Type = TokenType::Invalid;
		}
	};
}

