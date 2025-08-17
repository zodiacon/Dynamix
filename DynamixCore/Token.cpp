#include "Token.h"

std::string_view Dynamix::Token::TypeToString() const {
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
    }
    if ((int)Type & (int)TokenType::Keyword)
        return "Keyword";
    if ((int)Type & (int)TokenType::Operator)
        return "Operator";
    return "(Unknown)";
}
