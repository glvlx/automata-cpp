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

#include "fa_token.hpp"
#include "util.hpp"

/**
 * Does not create an actual NFA;
 * instead, automatically creates a DFA using power set construction
 */

namespace Automata {

const std::string EPSILON_TOKEN = "_";

class CompoundState {
    std::unordered_set<std::string> simple_states_;
    std::size_t hash_ = 0;

    void update_hash(const std::string& new_state) {
        /* needs to be order-independent because unordered_set */
        hash_ += std::hash<std::string>{}(new_state);
    }

public:
    CompoundState() = default;
    explicit CompoundState(const std::unordered_set<std::string>& simple_states):
            simple_states_(simple_states) {
        for (const auto& state : simple_states) {
            update_hash(state);
        }
    }
    
    void add_state(const std::string& state) {
        auto [it, inserted] = simple_states_.insert(state);
        if (inserted) {
            update_hash(state);
        }
    }

    void add_states(const std::unordered_set<std::string>& states) {
        for (const auto& state : states) {
            add_state(state);
        }
    }

    const std::unordered_set<std::string>& get_primary_states() const noexcept {
        return simple_states_;
    }

    bool operator==(const CompoundState& other) const {
        return simple_states_ == other.simple_states_;
    }

    std::size_t hash() const noexcept {
        return hash_;
    }
};

} // namespace Automata

template <>
struct std::hash<Automata::CompoundState> {
    std::size_t operator()(const Automata::CompoundState& s) const noexcept {
        return s.hash();
    }
};

namespace Automata {

class NFAGenerator {

    std::istream& is_;
    int line_no_ = 1;
    bool valid_ = true;
    std::unordered_map<std::string, bool> states_; // bool indicates if the state is accepted
    std::unordered_map<CompoundState, bool> compound_states_;
    std::unordered_map<std::string, CompoundState> part_of_compound_;
    std::string start_state_name_;
    CompoundState compound_start_state_;
    std::unordered_map<std::string, 
        std::unordered_map<std::string, std::unordered_set<std::string>>> transitions_;
    std::unordered_map<CompoundState, std::unordered_map<std::string, CompoundState>> compound_transitions_;
    std::unordered_map<std::string, std::unordered_set<std::string>> eps_path_from, eps_path_to;
    
    TokenType get_token_type(std::string tok) {
        if (tok.size() == 0) {
            return TokenType::INVALID;
        } else if (tok == "!") {
            return TokenType::START_MARKER;
        } else if (tok == "+") {
            return TokenType::ACCEPT_MARKER;
        } else if (tok == EPSILON_TOKEN) {
            return TokenType::EPSILON;  
        } else if (tok == ">") {
            return TokenType::RARROW;
        } else if (tok.size() > 1 && tok[0] == '/' && tok[1] == '/') {
            return TokenType::COMMENT;
        }
        
        /* STATE_NAME */
        bool is_state_name = true;
        for (auto c : tok) {
            if (!(isalnum(c) || c == '_')) {
                is_state_name = false;
                break;
            }
        }
        if (is_state_name) {
            return TokenType::STATE_NAME;
        }

        /* SYMBOL: one singular character in single quotes or literal `_` */
        if (tok.size() == 3 && tok[0] == '\'' && tok[2] == '\'') {
            return TokenType::SYMBOL;
        }

        return TokenType::INVALID;
    }

    struct Token {
        TokenType type_;
        std::string token_;
    };

    void fmt_error(int line_no, std::string line, std::string reason = "") {
        valid_ = false;
        std::cerr << "Cannot deserialize NFA: wrong format"
                  << (reason.size() > 0 ? " - " + reason : "") << "\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

    void name_error(int line_no, std::string line, std::string name) {
        valid_ = false;
        std::cerr << "Cannot deserialize NFA: wrong state name \"" << name << "\"\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

    void sym_error(int line_no, std::string line, std::string symbol) {
        valid_ = false;
        std::cerr << "Cannot deserialize NFA: wrong symbol " << symbol << "\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

public:
    NFAGenerator(std::istream& is)
    : is_(is) {}

    bool make_dfa_spec(std::ostream& os) {
        /* split input into lines */
        std::string line;
        while (std::getline(is_, line)) {
            std::stringstream aux;
            std::vector<Token> tokens;

            /* first pass: separate special characters with whitespaces and verify characters */
            for (auto c : line) {
                if (c == '!' || c == '+' || c == '>') {
                    aux << ' ' << c << ' ';
                } else {
                    aux << c;
                }
                /* should not be necessary
                else {
                    std::cerr << "Cannot deserialize a DFA: unknown character '" << c << "'" << std::endl;
                    return false;
                }
                */ 
                
            }

            /* second pass: split tokens */
            std::string tok;
            while (aux >> tok) {
                TokenType toktype = get_token_type(tok);
                if (toktype == TokenType::EMPTY) {
                    throw std::runtime_error("Warning: empty token found");
                } else if (toktype == TokenType::COMMENT) {
                    break;
                } else if (toktype == TokenType::INVALID) {
                    std::cerr << "Cannot deserialize a DFA: invalid token [" << tok << "]" << std::endl;
                }
                tokens.push_back({
                    toktype, 
                    tok
                });
            }

            #if DEBUG_PRINT
            // print tokens
            std::cout << "{";
            for (auto token : tokens) {
                std::cout << "[" << token.type_ << ": " << token.token_ << "], ";
            }
            std::cout << "}" << std::endl;
            #endif

            /* determine line type & save relevant information */
            if (tokens.size() == 0) {
                continue;
            } else if (tokens.front().type_ == TokenType::START_MARKER) {
                if (tokens.size() > 2) {
                    fmt_error(line_no_, line, "more than one start state specified");
                    continue;
                }
                /* Start state declaration */
                std::string state_name = tokens.back().token_;
                if (!is_valid_state_name(state_name)) {
                    name_error(line_no_, line, state_name);
                    continue;
                }
                states_.try_emplace(state_name, false);
                start_state_name_ = tokens.back().token_;
            } else if (tokens.front().type_ == TokenType::ACCEPT_MARKER) {
                if (tokens.size() < 2) {
                    fmt_error(line_no_, line, "no accepting states specified");
                    continue;
                }
                /* Accepted state declaration; state name(s) present */
                auto it = tokens.begin();
                for (++it; it != tokens.end(); it++) {
                    Token token = *it;
                    std::string state_name = token.token_;
                    if (!is_valid_state_name(state_name)) {
                        name_error(line_no_, line, state_name);
                        continue;
                    }
                    states_.insert_or_assign(state_name, true);
                }
            } else if (tokens.front().type_ == TokenType::STATE_NAME) {
                if (!(tokens.size() >= 4
                   && tokens[0].type_ == TokenType::STATE_NAME
                   && (tokens[1].type_ == TokenType::SYMBOL
                    || tokens[1].type_ == TokenType::EPSILON)
                   /* epsilon transition logic handled in codegen */
                   && tokens[2].type_ == TokenType::RARROW
                   && tokens[3].type_ == TokenType::STATE_NAME
                   /* check all the remaining tokens later */
                )) {
                    fmt_error(line_no_, line, "wrong transition format");
                }
                std::string state_from = tokens[0].token_;
                if (!is_valid_state_name(state_from)) {
                    name_error(line_no_, line, state_from);
                }
                std::string char_on = tokens[1].token_;

                states_.try_emplace(state_from, false);
                auto to_it = tokens.begin() + 3;
                for (; to_it != tokens.end(); to_it++) {
                    Token token_to = *to_it;
                    if (token_to.type_ != TokenType::STATE_NAME) {
                        fmt_error(line_no_, line, "transition targets must be valid state names");
                        break;
                    }
                    std::string state_to = token_to.token_;
                    /* do not check the state name */
                    if (!valid_) continue;

                    states_.try_emplace(state_to, false);


                    if (char_on == EPSILON_TOKEN) {
                        for (auto pre_state : eps_path_to[state_from]) {
                            for (auto post_state : eps_path_from[state_to]) {
                                eps_path_from[state_from].insert(post_state);
                                eps_path_from[pre_state].insert(state_to);
                                eps_path_from[pre_state].insert(post_state);
                                
                                eps_path_to[state_to].insert(pre_state);
                                eps_path_to[post_state].insert(state_from);
                                eps_path_to[post_state].insert(pre_state);
                            }
                        }
                        eps_path_from[state_from].insert(state_to);
                        eps_path_to[state_to].insert(state_from);
                        continue;
                    }
                    // non-epsilon transition
                    auto [it, inserted] = transitions_.try_emplace(state_from, 
                        std::unordered_map<std::string, std::unordered_set<std::string>>());
                    auto& map = it->second;
                    auto [map_it, map_inserted] = map.try_emplace(char_on, 
                        std::unordered_set<std::string>());
                    auto& set = map_it->second;
                    set.insert(state_to);
                }
            } else {
                fmt_error(line_no_, line, "unknown line format");
            }

            line_no_++;
        }
        if (!is_valid_state_name(start_state_name_)) {
            valid_ = false;
            std::cerr << "Cannot deserialize NFA: no initial state" << std::endl;
        }

        /* No further error detection */
        if (!valid_) {
            return false;
        }
        
        debug_print("new transitions after epsilon elimination:");

        /* Eliminate epsilon-transitions */
        for (const auto& [state_from, map] : transitions_) {
            /* right extension */
            for (const auto& [char_on, target_set] : map) {
                for (const auto& state_to : target_set) {
                    for (const auto& post_state: eps_path_from[state_to]) {
                        transitions_[state_from][char_on].insert(post_state);
                        debug_print("[add] " + state_from + " " + char_on + " > " + post_state);
                    }
                }
            }

            /* left extension */
            for (const auto& pre_state : eps_path_to[state_from]) {
                for (const auto& [char_on, target_set] : map) {
                    for (const auto& state_to : target_set) {
                        transitions_[pre_state][char_on].insert(state_to);
                        debug_print("[add] " + pre_state + " " + char_on + " > " + state_to);
                    }
                }
            }
        }
        debug_print("");

        /* Generate compound states*/
        std::queue<CompoundState*> q_pstate;
        compound_start_state_.add_state(start_state_name_);
        q_pstate.push(&compound_start_state_);

        CompoundState s_from;
        while (!q_pstate.empty()) {
            s_from = *q_pstate.front();
            q_pstate.pop();

            auto [it, inserted] = compound_states_.try_emplace(s_from, false);
            if (!inserted) {
                continue;
            }
            for (const auto& prim_s : s_from.get_primary_states()) {
                if (states_[prim_s]) {
                    compound_states_[s_from] = true;
                }
                for (const auto& [char_on, set_to] : transitions_[prim_s]) {
                    auto [it, inserted] = compound_transitions_[s_from].try_emplace(char_on, CompoundState(set_to));
                    if (!inserted) {
                        compound_transitions_[s_from][char_on].add_states(set_to);
                    }
                }
            }
            for (auto& [char_on, state_to] : compound_transitions_[s_from]) {
                if (compound_states_.find(state_to) == compound_states_.end()) {
                    q_pstate.push(&state_to);
                }
            }
        }

        /* Generate output */
        os << "! " << compound_start_state_.hash() << std::endl;
        bool accepted_states_met = false;
        for (const auto& [state, is_accepted] : compound_states_) {
            if (is_accepted) {
                if (!accepted_states_met) {
                    accepted_states_met = true;
                    os << "+";
                }
                os << ' ' << state.hash();
            }
        }
        if (accepted_states_met) {
            os << std::endl;
        }
        for (const auto& [state_from, transitions] : compound_transitions_) {
            for (const auto& [char_on, state_to] : transitions) {
                os << state_from.hash() << ' ' << char_on << " > " << state_to.hash() << std::endl;
            }
        }
        return valid_;
    }
};

} // namespace Automata

#endif