#ifndef DFA_AUTOGEN_H_
#define DFA_AUTOGEN_H_

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "util.hpp"
#include "fa_token.hpp"

namespace Automata {

class DFAGenerator {
    std::istream& is_;
    int line_no_ = 1;
    bool valid_ = true;
    std::unordered_map<std::string, bool> states_; // bool indicates if the state is accepted
    std::string start_state_name_;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions_;
    
    TokenType get_token_type(std::string tok) {
        if (tok.size() == 0) {
            return TokenType::INVALID;
        } else if (tok == "!") {
            return TokenType::START_MARKER;
        } else if (tok == "+") {
            return TokenType::ACCEPT_MARKER;
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

        /* SYMBOL, for now only one singular character in single quotes */
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
        std::cerr << "Cannot deserialize DFA: wrong format"
                  << (reason.size() > 0 ? " - " + reason : "") << "\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

    void name_error(int line_no, std::string line, std::string name) {
        valid_ = false;
        std::cerr << "Cannot deserialize DFA: wrong state name \"" << name << "\"\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

    void sym_error(int line_no, std::string line, std::string symbol) {
        valid_ = false;
        std::cerr << "Cannot deserialize DFA: wrong symbol " << symbol << "\n"
                  << "Line #" << line_no << ": " << line << std::endl;
    }

public:
    DFAGenerator(std::istream& is)
    : is_(is) {}

    bool deserialize(std::ostream& os, std::string dfa_name) {
        if (!is_valid_id(dfa_name)) {
            std::cerr << "Cannot deserialize DFA: DFA name \"" << dfa_name << "\" contains characters "
                      << "other than alphanumeric characters and underscore." << std::endl;
            return false;
        }

        os << "#ifndef DFA_" << dfa_name << "_H__\n"
           << "#define DFA_" << dfa_name << "_H__\n"
           << "\n"
           << "namespace DFA_" << dfa_name << " {\n"
           << "\n"
           << "void init(Automata::AutoDFA& dfa) {"
           << std::endl;

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
            // for now: just print tokens
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
                if (!(tokens.size() == 4
                   && tokens[0].type_ == TokenType::STATE_NAME
                   && tokens[1].type_ == TokenType::SYMBOL
                   && tokens[2].type_ == TokenType::RARROW
                   && tokens[3].type_ == TokenType::STATE_NAME
                )) {
                    fmt_error(line_no_, line, "wrong transition format");
                }
                std::string state_from = tokens[0].token_;
                std::string state_to = tokens[3].token_;
                if (!is_valid_state_name(state_from)) {
                    name_error(line_no_, line, state_from);
                }
                if (!is_valid_state_name(state_to)) {
                    name_error(line_no_, line, state_to);
                }

                std::string char_on = tokens[1].token_;

                if (!valid_) {
                    continue;
                }
                states_.try_emplace(state_from, false);
                states_.try_emplace(state_to, false);
                auto [it, inserted] = transitions_.try_emplace(state_from, 
                    std::unordered_map<std::string, std::string>());
                auto& map = it->second;
                map.insert_or_assign(char_on, state_to);
            } else {
                fmt_error(line_no_, line, "unknown line format");
            }

            line_no_++;
        }
        if (!is_valid_state_name(start_state_name_)) {
            valid_ = false;
            std::cerr << "Cannot deserialize DFA: no initial state" << std::endl;
        }

        /* Generate code */
        if (valid_) {
            /* Add start state */
            os << TAB << "auto& q" << start_state_name_ << " = dfa.get_start_state(" 
               << (states_.at(start_state_name_) ? "true" : "false") << ");" << std::endl;

            os << std::endl;
            
            /* Add all other states */
            for (auto [state_name, is_accepting] : states_) {
                if (state_name == start_state_name_) continue;

                os << TAB << "auto& q" << state_name << " = dfa.new_state(" 
                   << (is_accepting ? "true" : "false") << ");" << std::endl;
            }

            os << std::endl;

            /* Add transitions */
            for (auto [state_from, map] : transitions_) {
                for (auto [char_on, state_to] : map) {
                    os << TAB << "dfa.add_transition(" 
                       << "q" << state_from << ", "
                       << char_on << ", "
                       << "q" << state_to
                       << ");" << std::endl;
                }
            }
        }

        os << "}\n"
           << "\n"
           << "} // namespace DFA_" << dfa_name << "\n"
           << "\n"
           << "#endif" << std::endl;
        return valid_;
    }
};

class AutoDFA {
private:
    int max_id_ = 0;
public:
    class AutoDFAState {
        inline static int max_id_ = 0;
    public:
        AutoDFA* const parent_; 
        const int id_;
        bool is_accepted_ = false;
        AutoDFAState(AutoDFA* const parent, bool is_accepted = false)
        : parent_(parent)
        , id_(parent->max_id_++)
        , is_accepted_(is_accepted) {
            std::stringstream dbgs;
            dbgs << "new state at " << this;
            debug_print(dbgs.str());
        }
    };

private:
    std::list<AutoDFAState> states_ {
        AutoDFAState(this)
    };
    std::unordered_map<AutoDFAState*, std::unordered_map<char, AutoDFAState*>> transitions_ = {
        {&states_.front(), {}}
    };
    AutoDFAState* state_ = &states_.front();

public:
    AutoDFAState& get_start_state(bool is_accepted = false) {
        std::stringstream dbgs;
        dbgs << "new reference to " << &states_.front();
        debug_print(dbgs.str());

        if (is_accepted) {
            states_.front().is_accepted_ = true;
        }

        return states_.front();
    }

    AutoDFAState& new_state(bool is_accepted = false, bool make_start_state = false) {
        if (make_start_state) {
            states_.emplace_front(this, is_accepted);
        } else {
            states_.emplace_back(this, is_accepted);
        }
        AutoDFAState& state = (make_start_state) ? states_.front() : states_.back();
        
        std::stringstream dbgs;
        dbgs << "new reference to " << &state;
        debug_print(dbgs.str());

        transitions_.emplace(&state, 
                             std::unordered_map<char, AutoDFAState*>());
        return state;
    }

    /**
     * Adds a transition. Can construct necessary entries automatically.
     * 
     * @return Whether a new transition has been created or not (= an old one replaced)
     */
    bool add_transition(AutoDFAState& from, char on, AutoDFAState& to) {
        auto [it_from, inserted_from] = transitions_.try_emplace(&from, 
            std::unordered_map<char, AutoDFAState*>()); // checks if key exists; if not, copies {}
        auto [it_sym, inserted_sym] = it_from->second.insert_or_assign(on, &to);
        if (!inserted_sym) {
            std::cerr << "Warning: a transition has been replaced" << std::endl;
        }
        return inserted_sym;
    }

    bool step(char symbol) {
        auto next_state_it = transitions_[state_].find(symbol);
        if (next_state_it == transitions_[state_].end()) {
            return false;
        } else {
            state_ = next_state_it->second;
            return true;
        }
    }

    bool check(std::string in) {
        state_ = &get_start_state();

        for (char s : in) {
            if (!step(s)) {
                return false;
            }
        }
        return state_->is_accepted_;
    }

    bool serialize(std::ostream& os) {
        AutoDFAState& start_state = get_start_state();

        /* Start state */
        os << "! q" << start_state.id_ << std::endl;

        /* Terminal states */
        bool terminal_present = false;
        for (auto& state : states_) {
            if (!state.is_accepted_) {
                continue;
            }
            // else: state accepted
            if (!terminal_present) {
                // first occurrence
                terminal_present = true;
                os << "+ ";
            } 
            os << 'q' << state.id_ << ' ';
        }
        if (!terminal_present) {
            std::cerr << "Unable to serialize the DFA, reason: no terminal states";
            return false;
        } else {
            os << std::endl;
        }

        /* Transitions */
        for (auto [from, map] : transitions_) {
            for (auto [on, to] : map) {
                os << "q" << from->id_ << " '" << on << "' > q" << to->id_ << std::endl;
            }
        }

        return true;
    }
};

} // namespace Automata

#endif