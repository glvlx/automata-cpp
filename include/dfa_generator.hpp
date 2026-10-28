#ifndef DFA_GENERATOR_H_
#define DFA_GENERATOR_H_

#include <iostream>
#include <unordered_map>
#include <string>

#include "fa_token.hpp"
#include "fagen_status_code.hpp"

namespace Automata {

/**
 * Reads a DFA specification from an istream and generates corresponding C++ code.
 * An instance shall not be copied or mutated. Parsing multiple specifications
 * requires multiple instances of this class.
 */
class DFAGenerator {
    std::istream& is_;
    std::ostream& error_os_;
    int line_no_ = 1;
    bool valid_ = true;
    unsigned int status_flags_ = FAGenStatusFlag::OK;
    std::unordered_map<std::string, bool> states_; // bool indicates if the state is accepting
    std::string start_state_name_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions_;
    
    /**
     * Returns token type (for input parsing).
     * Recognizes:
     * - `!`: START_MARKER
     * - `+`: ACCEPT_MARKER
     * - `>`: RARROW (denotes rhs of transitions)
     * - `//.*` and following symbols: COMMENT
     * - `[0-9a-zA-Z_]*`: STATE_NAME
     * - `'.'`: SYMBOL
     * otherwise INVALID.
     */
    TokenType get_token_type(std::string tok);

    struct Token {
        TokenType type_;
        std::string token_;
    };

    /* Functions displaying errors and setting parser state to invalid. */

    void fmt_error(int line_no, std::string line, std::string reason = "");

    void state_name_error(int line_no, std::string line, std::string sname);

    void sym_error(int line_no, std::string line, std::string symbol);

public:
    /**
     * Reads a DFA specification and prepares an internal representation to generate code.
     */
    DFAGenerator(std::istream& is, std::ostream& error_os = std::cerr);

    /**
     * @return a bit field containing all status flags
     */
    inline unsigned int get_status_flags() { return status_flags_; }

    /**
     * Writes C++ code corresponding to the specification to an ostream.
     * 
     * @return Status codes
     */
    unsigned int write(std::ostream& os, std::string dfa_name);
};

} // namespace Automata

#endif