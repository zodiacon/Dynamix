#pragma once

#include <string_view>
#include <string>
#include <set>
#include <memory>
#include "Token.h"
#include <unordered_map>

namespace Dynamix {
	class Tokenizer {
	public:
		bool Tokenize(std::string_view text, int line = 1);
		bool TokenizeFile(std::string_view filename);

		void SetCommentToEndOfLine(std::string_view chars);

		bool AddToken(std::string_view lexeme, TokenType type);
		bool AddTokens(std::initializer_list<std::pair<std::string_view, TokenType>> tokens);

		Token Next();
		Token const& Peek();

		int Line() const noexcept {
			return m_Line;
		}

		int Column() const noexcept {
			return m_Col;
		}

		std::string const& FileName() const noexcept {
			return m_FileName;
		}

		std::string_view TokenTypeToString(TokenType type) const;

		operator bool() {
			return *m_Current != 0;
		}

		const char* AddLiteralString(std::string str) {
			if (auto it = m_LiteralStrings.find(str); it != m_LiteralStrings.end())
				return it->c_str();
			return m_LiteralStrings.emplace(str).first->c_str();
		}

	private:
		bool IsNextChars(std::string_view chars);
		bool ProcessSingleLineComment();
		void EatWhitespace();
		Token ParseIdentifier();
		Token ParseNumber();
		Token ParseOperator();
		Token ParseString(bool raw);

		std::unique_ptr<char[]> m_Text;
		Token m_Next;
		int m_Col{ 1 }, m_Line{ 1 };
		std::string m_FileName;
		std::unordered_map<std::string_view, TokenType> m_TokenTypes;
		std::unordered_map<TokenType, std::string_view> m_TokenTypesRev;
		const char* m_Current{ nullptr };
		std::string m_CommentToEndOfLine{ "//" };
		std::string m_MultiLineCommentStart{ "/*" }, m_MultiLineCommentEnd{ "*/" };
		int m_MultiLineCommentNesting;
		std::set<std::string> m_LiteralStrings;
	};
}

