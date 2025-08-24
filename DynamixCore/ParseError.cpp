#include "ParseError.h"

using namespace Dynamix;

ParseError::ParseError(ParseErrorType type, Token const& token, std::string desc) : m_Type(type), m_Location{ "", token.Line, token.Col }, m_Description(std::move(desc)) {
}

ParseError::ParseError(ParseErrorType type, CodeLocation location, std::string desc) : m_Type(type), m_Location(std::move(location)), m_Description(std::move(desc)) {
}
