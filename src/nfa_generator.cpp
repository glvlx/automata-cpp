#include "nfa_generator.hpp"

#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>

#include "fa_token.hpp"
#include "fagen_status_flag.hpp"
#include "util.hpp"
#include "compound_state.hpp"

namespace Automata {

TokenType NFAGenerator::get_token_type(std::string tok) const noexcept {
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
    for (const auto& c : tok) {
        if (!(std::isalnum(c) || c == '_')) {
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

    /* SYMBOL_SET: any number of characters enclosed in [] */
    if (tok.front() == '[') {
        if (tok.back() == ']') {
            return TokenType::SYMBOL_SET;
        } 
        return TokenType::SYMBOL_SET_START;
    } else if (tok.back() == ']') {
        return TokenType::SYMBOL_SET_END;
    }

    return TokenType::INVALID;
}
void NFAGenerator::fmt_error(int line_no, std::string line, std::string reason) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_FMT;
    std::cerr << "Invalid NFA specification: wrong format"
                << (reason.size() > 0 ? " - " + reason : "") << "\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

void NFAGenerator::state_name_error(int line_no, std::string line, std::string sname) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_STATE_NAME;
    std::cerr << "Invalid NFA specification: wrong state name \"" << sname << "\"\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

void NFAGenerator::sym_error(int line_no, std::string line, std::string symbol) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_SYMBOL;
    std::cerr << "Invalid NFA specification: wrong symbol " << symbol << "\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

bool NFAGenerator::parse_input() {
    /* split input into lines */
    std::string line;
    while (std::getline(is_, line)) {
        line_no_++;

        std::stringstream aux;
        std::vector<Token> tokens;

        /* first pass: separate special characters with whitespaces and verify characters */
        for (const auto& c : line) {
            if (c == '!' || c == '+' || c == '>') {
                aux << ' ' << c << ' ';
            } else {
                aux << c;
            }
        }

        /* second pass: split tokens */
        std::string tok;
        while (aux >> tok) {
            /* special case: whitespace character, next "token" necessary */
            if (tok == "'") { /* for SYMBOL */
                aux >> tok;
                if (tok == "'") {
                    tokens.push_back({
                        TokenType::SYMBOL,
                        "' '"
                    });
                } else {
                    fmt_error(line_no_, line, "invalid sequence [' " + tok + "]");
                }
                continue;
            }
            TokenType toktype = get_token_type(tok);
            /* special case: SYMBOL_SET with whitespace */
            if (toktype == TokenType::SYMBOL_SET_START) {
                std::string symbol_group = tok + " ";
                while (aux && toktype != TokenType::SYMBOL_SET_END) {
                    aux >> tok;
                    toktype = get_token_type(tok);
                    symbol_group += tok;
                }
                if (toktype == TokenType::SYMBOL_SET_END) {
                    tokens.push_back({
                        TokenType::SYMBOL_SET,
                        symbol_group
                    });
                } else {
                    fmt_error(line_no_, line, "symbol group not terminated");
                } 
                continue;
            } 
            if (toktype == TokenType::SYMBOL_SET_END) {
                fmt_error(line_no_, line, "unmatched symbol group terminator ']'");
                continue;
            }

            /* regular case */
            if (toktype == TokenType::COMMENT) {
                break;
            } else if (toktype == TokenType::INVALID) {
                fmt_error(line_no_, line, "invalid token [" + tok + "]");
            }
            tokens.push_back({
                toktype, 
                tok
            });
        }

        #if DEBUG_PRINT_ENABLED
        // print tokens
        std::cout << "{";
        for (const auto& token : tokens) {
            std::cout << "[" << token.type_ << ": " << token.token_ << "], ";
        }
        std::cout << "}" << std::endl;
        #endif

        /* determine line type & save relevant information */
        if (tokens.size() == 0) {
            /* empty line */
            continue;
        } else if (tokens.front().type_ == TokenType::START_MARKER) {
            if (tokens.size() > 2) {
                fmt_error(line_no_, line, "more than one start state specified");
                continue;
            }
            /* start state declaration */
            std::string state_name = tokens.back().token_;
            if (!is_valid_state_name(state_name)) {
                state_name_error(line_no_, line, state_name);
                continue;
            }
            states_.try_emplace(state_name, false);
            start_state_name_ = tokens.back().token_;
        } else if (tokens.front().type_ == TokenType::ACCEPT_MARKER) {
            if (tokens.size() < 2) {
                fmt_error(line_no_, line, "no accepting states specified");
                continue;
            }
            /* accepting state declaration */
            auto it = tokens.begin();
            for (++it; it != tokens.end(); it++) {
                Token token = *it;
                std::string state_name = token.token_;
                if (!is_valid_state_name(state_name)) {
                    state_name_error(line_no_, line, state_name);
                    continue;
                }
                states_.insert_or_assign(state_name, true);
                for (const auto& pre_state : eps_path_to[state_name]) {
                    states_[pre_state] = true;
                }
            }
        } else if (tokens.front().type_ == TokenType::STATE_NAME) {
            if (!(tokens.size() >= 4
                && tokens[0].type_ == TokenType::STATE_NAME
                && (tokens[1].type_ == TokenType::SYMBOL
                || tokens[1].type_ == TokenType::EPSILON
                || tokens[1].type_ == TokenType::SYMBOL_SET)
                /* epsilon transition logic handled in codegen */
                && tokens[2].type_ == TokenType::RARROW
                /* check all the remaining tokens later */
            )) {
                /* additional error detection */
                if (tokens.size() >= 4) {
                    if (tokens[0].type_ != TokenType::STATE_NAME) {
                        state_name_error(line_no_, line, tokens[0].token_);
                    }
                    if (!(tokens[1].type_ == TokenType::SYMBOL
                    || tokens[1].type_ == TokenType::EPSILON
                    || tokens[1].type_ == TokenType::SYMBOL_SET)) {
                        sym_error(line_no_, line, tokens[1].token_);
                    }
                }
                fmt_error(line_no_, line, "wrong transition format");
            }
            std::string state_from = tokens[0].token_;
            if (!is_valid_state_name(state_from)) {
                state_name_error(line_no_, line, state_from);
            }

            if (tokens.size() < 4) {
                continue;
            }

            auto& char_token = tokens[1];
            std::string char_on = tokens[1].token_;

            states_.try_emplace(state_from, false);
            auto to_it = tokens.begin() + 3;
            for (; to_it != tokens.end(); to_it++) {
                Token token_to = *to_it;
                if (token_to.type_ != TokenType::STATE_NAME) {
                    state_name_error(line_no_, line, token_to.token_);
                    fmt_error(line_no_, line, "transition targets must be valid state names");
                    break;
                }
                std::string state_to = token_to.token_;
                /* do not check the state name (checked in parser) */

                /* handle symbol groups */
                std::unordered_set<char> symbols_on;
                if (char_token.type_ == TokenType::SYMBOL_SET) {
                    /* discard [ ] */
                    for (auto cit = char_on.cbegin() + 1; cit != char_on.cend() - 1; cit++) {
                        char c = *cit;
                        char next = (cit + 1 == char_on.cend()) ? '\0' : *(cit + 1);
                        char next2 = (cit + 2 == char_on.cend()) ? '\0' : *(cit + 2);
                        char next3 = (cit + 3 == char_on.cend()) ? '\0' : *(cit + 3);
                        if (c == '\\') { // escaped characters
                            if (!next) {
                                fmt_error(line_no_, line, "unterminated escaping symbol \\ in symbol group");
                                break;
                            }
                            symbols_on.insert(next);
                            cit++;
                        } else if (next == '-') { // ranges
                            if (!next2) {
                                fmt_error(line_no_, line, "unterminated range in symbol group");
                                break;
                            } else if (next2 == '-') {
                                fmt_error(line_no_, line, "wrong range in symbol group: " + c + next + next2);
                                cit += 2;
                            } else {
                                char range_start = c;
                                char range_end = next2;
                                if (next2 == '\\') {
                                    if (!next3) {
                                        fmt_error(line_no_, line, "unterminated escaping symbol \\ in symbol group");
                                        break;
                                    }
                                    range_end = next3;
                                    cit++;
                                }
                                if (range_start > range_end) {
                                    std::cout << "[line " << line_no_ << "] Warning: range " << range_start 
                                              << "-" << range_end << " is empty" << std::endl;
                                }
                                for (char x = range_start; x <= range_end; x++) {
                                    symbols_on.insert(x);
                                }
                                cit += 2;
                            }
                        } else { // single characters
                            symbols_on.insert(c);
                        }
                    }
                } else if (char_token.type_ == TokenType::SYMBOL) {
                    symbols_on.insert(char_on[1]);
                }

                if (!valid_) continue;

                states_.try_emplace(state_to, false);

                if (char_on == EPSILON_TOKEN) {
                    eps_path_from[state_from].insert(state_to);
                    eps_path_to[state_to].insert(state_from);
                    states_[state_from] |= states_[state_to];

                    for (const auto& pre_state : eps_path_to[state_from]) {
                        eps_path_from[pre_state].insert(state_to);
                        eps_path_to[state_to].insert(pre_state);
                        states_[pre_state] |= states_[state_to];

                        for (const auto& post_state : eps_path_from[state_to]) {
                            eps_path_from[pre_state].insert(post_state);
                            eps_path_to[post_state].insert(pre_state);
                            states_[pre_state] |= states_[post_state];
                        }
                    }
                    for (const auto& post_state : eps_path_from[state_to]) {
                        eps_path_from[state_from].insert(post_state);
                        eps_path_to[post_state].insert(state_from);
                        states_[state_from] |= states_[post_state];
                    }
                    
                    continue;
                }
                // non-epsilon transition

                for (const auto& sym : symbols_on) {
                    std::string escaped_sym = "\'";
                    escaped_sym += std::string(1, sym) + "\'";

                    auto [it, inserted] = transitions_.try_emplace(state_from, 
                        std::unordered_map<std::string, std::unordered_set<std::string>>());
                    auto& map = it->second;
                    auto [map_it, map_inserted] = map.try_emplace(escaped_sym, 
                        std::unordered_set<std::string>());
                    auto& set = map_it->second;
                    set.insert(state_to);
                }
            }
        } else {
            fmt_error(line_no_, line, "unknown line format");
        }
    }
    if (!is_valid_state_name(start_state_name_)) {
        valid_ = false;
        status_flags_ |= FAGenStatusFlag::NO_INIT;
        std::cerr << "Invalid NFA specification: no initial state" << std::endl;
    }

    return valid_;
}

void NFAGenerator::eliminate_epsilon() {
    DEBUG_PRINT("new transitions after epsilon elimination:");

    /* Eliminate epsilon-transitions */
    for (const auto& [state_from, map] : transitions_) {
        /* right extension */
        for (const auto& [char_on, target_set] : map) {
            for (const auto& state_to : target_set) {
                for (const auto& post_state: eps_path_from[state_to]) {
                    transitions_[state_from][char_on].insert(post_state);
                    DEBUG_PRINT("[add] " + state_from + " " + char_on + " > " + post_state);
                }
            }
        }

        /* left extension */
        for (const auto& pre_state : eps_path_to[state_from]) {
            for (const auto& [char_on, target_set] : map) {
                for (const auto& state_to : target_set) {
                    transitions_[pre_state][char_on].insert(state_to);
                    DEBUG_PRINT("[add] " + pre_state + " " + char_on + " > " + state_to);
                }
            }
        }
    }
}

void NFAGenerator::construct_power_set() {
    /* Generate compound states*/
    std::queue<CompoundState*> q_pstate;
    compound_start_state_.add_state(start_state_name_);
    q_pstate.push(&compound_start_state_);

    while (!q_pstate.empty()) {
        CompoundState s_from = *q_pstate.front();
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
}

NFAGenerator::NFAGenerator(std::istream& is)
        : is_(is) {
    parse_input();
    
    /* No further error detection */
    if (!valid_) {
        return;
    }

    eliminate_epsilon();

    construct_power_set();
}

unsigned int NFAGenerator::write_dfa_spec(std::ostream& os) {
    /* No further error detection */
    if (!valid_ || status_flags_ != FAGenStatusFlag::OK) {
        return status_flags_;
    }

    /* Generate output */
    os << "! " << compound_start_state_.hash() << std::endl;
    bool accepting_states_met = false;
    for (const auto& [state, is_accepting] : compound_states_) {
        if (is_accepting) {
            if (!accepting_states_met) {
                accepting_states_met = true;
                os << "+";
            }
            os << ' ' << state.hash();
        }
    }
    if (accepting_states_met) {
        os << std::endl;
    }
    for (const auto& [state_from, transitions] : compound_transitions_) {
        for (const auto& [char_on, state_to] : transitions) {
            os << state_from.hash() << ' ' << char_on << " > " << state_to.hash() << std::endl;
        }
    }
    return FAGenStatusFlag::OK;
}

} // namespace Automata