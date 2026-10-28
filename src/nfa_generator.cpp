#include "nfa_generator.hpp"

namespace Automata {

TokenType NFAGenerator::get_token_type(std::string tok) {
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

void NFAGenerator::fmt_error(int line_no, std::string line, std::string reason) {
    valid_ = false;
    std::cerr << "Cannot deserialize NFA: wrong format"
                << (reason.size() > 0 ? " - " + reason : "") << "\n"
                << "Line #" << line_no << ": " << line << std::endl;
}

void NFAGenerator::state_name_error(int line_no, std::string line, std::string name) {
    valid_ = false;
    std::cerr << "Cannot deserialize NFA: wrong state name \"" << name << "\"\n"
                << "Line #" << line_no << ": " << line << std::endl;
}

void NFAGenerator::sym_error(int line_no, std::string line, std::string symbol) {
    valid_ = false;
    std::cerr << "Cannot deserialize NFA: wrong symbol " << symbol << "\n"
                << "Line #" << line_no << ": " << line << std::endl;
}

bool NFAGenerator::parse_input() {
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
        }

        /* second pass: split tokens */
        std::string tok;
        while (aux >> tok) {
            TokenType toktype = get_token_type(tok);
            if (toktype == TokenType::COMMENT) {
                break;
            } else if (toktype == TokenType::INVALID) {
                std::cerr << "Cannot parse the NFA: invalid token [" << tok << "]" << std::endl;
            }
            tokens.push_back({
                toktype, 
                tok
            });
        }

        #if DEBUG_PRINT_ENABLED
        // print tokens
        std::cout << "{";
        for (auto token : tokens) {
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
                state_name_error(line_no_, line, state_from);
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
                    for (const auto& pre_state : eps_path_to[state_from]) {
                        for (const auto& post_state : eps_path_from[state_to]) {
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

NFAGenerator::NFAGenerator(std::istream& is)
        : is_(is) {
    parse_input();
    
    /* No further error detection */
    if (!valid_) {
        return;
    }

    eliminate_epsilon();

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
}

bool NFAGenerator::write_dfa_spec(std::ostream& os) {
    /* No further error detection */
    if (!valid_) {
        return false;
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

} // namespace Automata