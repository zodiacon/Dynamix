#include "Tokenizer.h"
#include <assert.h>

using namespace std;
using namespace Dynamix;

bool Tokenizer::Tokenize(string_view text, int line) {
	m_Text = text;
	m_Line = line;
	m_Col = 1;
	m_Current = m_Text.data();
	return true;
}

bool Tokenizer::AddToken(string_view lexeme, TokenType type) {
	return m_TokenTypesRev.try_emplace(type, lexeme).second &&
	m_TokenTypes.try_emplace(lexeme, type).second;
}

int Tokenizer::AddTokens(initializer_list<pair<string_view, TokenType>> tokens) {
	auto count = 0;
	for (auto const& [str, type] : tokens) {
		count += AddToken(str, type) ? 1 : 0;
	}
	return count;
}

Token Tokenizer::Next() {
	EatWhitespace();
	auto ch = *m_Current;
	if(ch == 0)
		return Token{ TokenType::End };
	if (isalpha(ch) || ch == '_' || ch == '$') {
		return ParseIdentifier();
	}
	if (ch >= '0' && ch <= '9') {
		return ParseNumber();
	}
	if (ch == '\"') {
		return ParseString(false);
	}
	else if (ch == '@' && m_Current[1] == '\"') {
		m_Current++;
		return ParseString(true);
	}
	return ParseOperator();
}

Token Tokenizer::Peek() {
	auto current = m_Current;
	auto line = m_Line;
	auto col = m_Col;
	auto token = Next();
	m_Current = current;
	m_Line = line;
	m_Col = col;
	return token;
}

std::string_view Dynamix::Tokenizer::TokenTypeToString(TokenType type) const {
	return m_TokenTypesRev.find(type)->second;
}

bool Tokenizer::ProcessSingleLineComment() {
	auto current = m_Current;
	int i = 0;
	while (*current && *current++ == m_CommentToEndOfLine[i]) {
		if (++i == m_CommentToEndOfLine.length())
			break;
	}
	if (i == m_CommentToEndOfLine.length()) {
		//
		// move to next line
		//
		m_Current = current;
		while (*m_Current && *m_Current != '\n')
			m_Current++;
		m_Current++;
		m_Line++;
		m_Col = 1;
		return true;
	}
	return false;
}

void Tokenizer::EatWhitespace() {
	while (*m_Current && isspace(*m_Current)) {
		m_Col++;
		if (*m_Current == '\n') {
			m_Col = 1;
			m_Line++;
		}
		++m_Current;
	}
	if (ProcessSingleLineComment())
		EatWhitespace();
}

Token Tokenizer::ParseIdentifier() {
	std::string lexeme;
	while (*m_Current && !isspace(*m_Current) && !ispunct(*m_Current)) {
		if (ProcessSingleLineComment())
			break;
		lexeme += *m_Current++;
		m_Col++;
	}
	assert(!lexeme.empty());
	auto type = TokenType::Identifier;
	if (auto it = m_TokenTypes.find(lexeme); it != m_TokenTypes.end())
		type = it->second;
	return Token{ .Type = type, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length(), };
}

Token Tokenizer::ParseNumber() {
	char* pd, * pi;
	auto dvalue = strtod(m_Current, &pd);
	int base = 10;
	int startLen = 0;
	if (*m_Current == '0') {
		switch (m_Current[1]) {
			case 'x': case 'X': startLen = 2; base = 16; break;
			case 'b': case 'B': startLen = 2; base = 2; break;
			case 'o': case 'O': startLen = 2; base = 8; break;
			default: m_Current -= 2; break;
		}
		m_Current += 2;
	}
	auto ivalue = strtoll(m_Current, &pi, base);
	assert(pd && pi);
	auto type = pd > pi ? TokenType::Real : TokenType::Integer;
	auto len = int(type == TokenType::Real ? pd - m_Current : pi - m_Current);
	m_Col += (int)len + startLen;
	m_Current += len;
	auto token = Token{ .Type = type, .Lexeme = string(m_Current - len - startLen, m_Current), .Line = m_Line, .Col = m_Col - len };
	if (type == TokenType::Integer)
		token.iValue = ivalue;
	else
		token.rValue = dvalue;

	if (*m_Current == '\n') {
		m_Col = 1;
		m_Line++;
		m_Current++;
	}
	ProcessSingleLineComment();
	return token;
}

Token Tokenizer::ParseOperator() {
	string lexeme;
	while (*m_Current && ispunct(*m_Current)) {
		//
		// treat parenthesis as special so they are not combined with other operators
		//
		if (lexeme == "(" || lexeme == ")" || (!lexeme.empty() && (*m_Current == '(' || *m_Current == ')')))
			break;

		lexeme += *m_Current++;
		m_Col++;
	}
	auto temp = lexeme;
	if (temp.empty())
		return Token();

	auto type = TokenType::Invalid;
	do {
		auto it = m_TokenTypes.find(lexeme);
		if (it != m_TokenTypes.end()) {
			type = it->second;
			break;
		}
		lexeme = lexeme.substr(0, lexeme.length() - 1);
	} while (!lexeme.empty());

	if (type == TokenType::Invalid)
		return Token{ .Type = TokenType::Invalid, .Lexeme = temp, .Line = m_Line, .Col = m_Col - (int)temp.length() };

	m_Current -= (temp.length() - lexeme.length());
	if (lexeme.empty()) {
		lexeme = temp;
		return Token{ .Type = TokenType::Operator, .Lexeme = move(lexeme), .Line = m_Line, .Col = m_Col - (int)temp.length() };
	}
	return Token{ .Type = type, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length() };
}

Token Tokenizer::ParseString(bool raw) {
	string lexeme;
	while (*++m_Current && *m_Current != '\"') {
		if (!raw && *m_Current == '\\') {
			//
			// escape character
			//
			static const string escape("tnrba\\\""), actual("\t\n\r\b\a\\\"");
			assert(escape.length() == actual.length());
			if (auto index = escape.find(m_Current[1]); index != escape.npos) {
				m_Current++;
				lexeme += actual[index];
			}
			else {
				//
				// unknown escape sequence
				//
				Token token{ .Type = TokenType::Error, .Lexeme = "Unknown escape character", .Line = m_Line, .Col = m_Col };
				return token;
			}
			continue;
		}
		lexeme += *m_Current;
		m_Col++;
		if (*m_Current == '\n') {
			m_Col = 1;
			m_Line++;
			if (!raw) {
				Token token{ .Type = TokenType::Error, .Lexeme = "Missing closing quote", .Line = m_Line, .Col = m_Col };
				return token;
			}
		}
	}
	m_Current++;
	return Token{ .Type = TokenType::String, .Lexeme = lexeme, .Line = m_Line, .Col = m_Col - (int)lexeme.length(), };
}
