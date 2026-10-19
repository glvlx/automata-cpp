#ifndef FA_TOKEN_H_
#define FA_TOKEN_H_

#include <string>

namespace Automata {

enum class TokenType {
    START_MARKER,
    ACCEPT_MARKER,
    RARROW,
    STATE_NAME,
    SYMBOL,
    EPSILON,
    INVALID,
    COMMENT
};

constexpr std::string_view to_string(TokenType t) {
    switch(t) {
        case TokenType::START_MARKER:       return "START_MARKER";
        case TokenType::ACCEPT_MARKER:      return "ACCEPT_MARKER";
        case TokenType::RARROW:             return "RARROW";
        case TokenType::STATE_NAME:         return "STATE_NAME";
        case TokenType::SYMBOL:             return "SYMBOL";
        case TokenType::EPSILON:            return "EPSILON";
        case TokenType::INVALID:            return "INVALID";
        case TokenType::COMMENT:            return "COMMENT";
    }
}

std::ostream& operator<<(std::ostream& os, TokenType t) {
    return os << to_string(t);
}

} // namespace Automata

#endif