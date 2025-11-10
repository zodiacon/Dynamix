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
		Operator	= 0x0300,
		Meta		= 0x0500,

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
		Each,
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
		Alias,

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
		QuestionDot,
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
		DotDotExclusive,
		DotDotInclusive,
		Ellipsis,
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

		std::string_view TypeToString() const noexcept;
		static std::string_view TypeToString(TokenType type);

		bool IsValid() const noexcept {
			return Type != TokenType::Invalid;
		}
		operator bool() const noexcept {
			return IsValid();
		}
		void Clear() noexcept {
			Type = TokenType::Invalid;
		}
	};
}

