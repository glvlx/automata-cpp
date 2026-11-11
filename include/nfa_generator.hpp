#ifndef NFA_GENERATOR_H_
#define NFA_GENERATOR_H_

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <functional>

#include "compound_state.hpp"
#include "fa_token.hpp"
#include "fagen_status_flag.hpp"

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
    
    TokenType get_token_type(std::string tok) const noexcept;

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

    /**
     * @pre epsilon transitions eliminated
     */
    void construct_power_set();

public:
    /**
     * Reads an NFA specification and prepares an internal representation 
     * to generate a DFA specification.
     */
    explicit NFAGenerator(std::istream& is);

    /* copying doesn't make sense */
    NFAGenerator(const NFAGenerator&) = delete;

    /**
     * @return a bit field containing all status flags
     */
    inline unsigned int get_status_flags() const noexcept { return status_flags_; }

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