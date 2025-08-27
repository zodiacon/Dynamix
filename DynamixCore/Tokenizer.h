#pragma once

#include <string_view>
#include <string>
#include <span>
#include "Token.h"
#include <unordered_map>

namespace Dynamix {
	class Tokenizer {
	public:
		bool Tokenize(std::string_view text, int line = 1);
		void SetCommentToEndOfLine(std::string_view chars);

		bool AddToken(std::string_view lexeme, TokenType type);
		bool AddTokens(std::initializer_list<std::pair<std::string_view, TokenType>> tokens);

		Token Next();
		Token Peek();

		std::string_view TokenTypeToString(TokenType type) const;

		operator bool() {
			return *m_Current != 0;
		}

	private:
		bool ProcessSingleLineComment();
		void EatWhitespace();
		Token ParseIdentifier();
		Token ParseNumber();
		Token ParseOperator();
		Token ParseString(bool raw);

		int m_Line;
		uint16_t m_Col{ 1 };
		std::unordered_map<std::string_view, TokenType> m_TokenTypes;
		std::unordered_map<TokenType, std::string_view> m_TokenTypesRev;
		std::string_view m_Text;
		const char* m_Current{ nullptr };
		std::string_view m_CommentToEndOfLine{ "//" };
	};
}

