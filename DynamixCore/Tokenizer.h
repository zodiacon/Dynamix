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

		Token Next() noexcept;
		Token const& Peek() noexcept;

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

		const char* AddLiteralString(std::string str) {
			if (auto it = m_LiteralStrings.find(str); it != m_LiteralStrings.end())
				return it->c_str();
			return m_LiteralStrings.emplace(str).first->c_str();
		}

	private:
		bool IsNextChars(std::string_view chars) noexcept;
		bool ProcessSingleLineComment() noexcept;
		void EatWhitespace() noexcept;
		Token ParseIdentifier();
		Token ParseNumber() noexcept;
		Token ParseOperator() noexcept;
		Token ParseString(bool raw);

		std::unique_ptr<char[]> m_Text;
		Token m_Next;
		int m_Col{ 1 };
		int m_Line{ 1 };
		std::string m_FileName;
		std::unordered_map<std::string_view, TokenType> m_TokenTypes;
		std::unordered_map<TokenType, std::string_view> m_TokenTypesRev;
		const char* m_Current{ nullptr };
		std::string m_CommentToEndOfLine{ "//" };
		std::string m_MultiLineCommentStart{ "/*" };
		std::string m_MultiLineCommentEnd{ "*/" };
		int m_MultiLineCommentNesting;
		std::set<std::string> m_LiteralStrings;
	};
}

