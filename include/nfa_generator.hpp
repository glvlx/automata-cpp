#ifndef NFA_GENERATOR_H_
#define NFA_GENERATOR_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <vector>
#include <queue>
#include <functional>

#include "compound_state.hpp"
#include "fa_token.hpp"
#include "fagen_status_flag.hpp"
#include "util.hpp"

#define EPSILON_TOKEN "_"

namespace Automata {

/**
 * Does not create an actual NFA;
 * instead, automatically creates a DFA using power set construction
 */
class NFAGenerator final {
    std::istream& is_;
    int line_no_ = 1;
    bool valid_ = true;
    unsigned int status_flags_ = FAGenStatusFlag::OK;
    std::unordered_map<std::string, bool> states_; // bool indicates if the state is accepting
    std::unordered_map<CompoundState, bool> compound_states_;
    std::string start_state_name_;
    CompoundState compound_start_state_;
    std::unordered_map<std::string, 
        std::unordered_map<std::string, std::unordered_set<std::string>>> transitions_;
    std::unordered_map<CompoundState, 
        std::unordered_map<std::string, CompoundState>> compound_transitions_;
    std::unordered_map<std::string, std::unordered_set<std::string>> eps_path_from, eps_path_to;
    
    TokenType get_token_type(std::string tok);

    struct Token {
        TokenType type_;
        std::string token_;
    };

    void fmt_error(int line_no, std::string line, std::string reason = "");

    void state_name_error(int line_no, std::string line, std::string name);

    void sym_error(int line_no, std::string line, std::string symbol);

    bool parse_input();

    /**
     * @pre valid eps_path_from and eps_path_pre (from parse_input())
     */
    void eliminate_epsilon();

public:

    explicit NFAGenerator(std::istream& is);

    /* copying doesn't make sense */
    NFAGenerator(const NFAGenerator&) = delete;

    /**
     * Writes a corresponding DFA specification to an output stream
     * 
     * @param os target output stream
     * 
     * @return whether the operation was successful
     */
    unsigned int write_dfa_spec(std::ostream& os);
};

} // namespace Automata

#endif