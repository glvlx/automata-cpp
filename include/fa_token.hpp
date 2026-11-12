/**
 * Defines token types for Finite Automata specification files.
 * Single-header for convenience of use.
 */
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
    SYMBOL_SET,
    SYMBOL_SET_START,
    SYMBOL_SET_END,
    EPSILON,
    INVALID,
    COMMENT
};

inline constexpr std::string_view to_string(TokenType t) noexcept {
    switch(t) {
        case TokenType::START_MARKER:       return "START_MARKER";
        case TokenType::ACCEPT_MARKER:      return "ACCEPT_MARKER";
        case TokenType::RARROW:             return "RARROW";
        case TokenType::STATE_NAME:         return "STATE_NAME";
        case TokenType::SYMBOL:             return "SYMBOL";
        case TokenType::SYMBOL_SET:         return "SYMBOL_SET";
        case TokenType::SYMBOL_SET_START:   return "SYMBOL_SET_START";
        case TokenType::SYMBOL_SET_END:     return "SYMBOL_SET_END";
        case TokenType::EPSILON:            return "EPSILON";
        case TokenType::INVALID:            return "INVALID";
        case TokenType::COMMENT:            return "COMMENT";
    }
}

inline std::ostream& operator<<(std::ostream& os, TokenType t) {
    return os << to_string(t);
}

} // namespace Automata

#endif