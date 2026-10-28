#include "dfa_generator.hpp"

#include <vector>
#include <sstream>
#include <string>
#include <iostream>
#include <unordered_map>

#include "util.hpp"
#include "fa_token.hpp"
#include "fagen_status_code.hpp"

namespace Automata {

TokenType DFAGenerator::get_token_type(std::string tok) {
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
    if (is_valid_state_name(tok)) {
        return TokenType::STATE_NAME;
    }

    /* SYMBOL, for now only one singular character in single quotes. */
    if (tok.size() == 3 && tok[0] == '\'' && tok[2] == '\'') {
        return TokenType::SYMBOL;
    }

    return TokenType::INVALID;
}

void DFAGenerator::fmt_error(int line_no, std::string line, std::string reason) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_FMT;
    error_os_ << "Invalid DFA specification: wrong format"
                << (reason.size() > 0 ? " - " + reason : "") << "\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

void DFAGenerator::state_name_error(int line_no, std::string line, std::string sname) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_STATE_NAME;
    error_os_ << "Invalid DFA specification: wrong state name \"" << sname << "\"\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

void DFAGenerator::sym_error(int line_no, std::string line, std::string symbol) {
    valid_ = false;
    status_flags_ |= FAGenStatusFlag::WRONG_SYMBOL;
    error_os_ << "Invalid DFA specification: wrong symbol " << symbol << "\n"
                << "[Line #" << line_no << "]: " << line << std::endl;
}

DFAGenerator::DFAGenerator(std::istream& is, std::ostream& error_os)
    : is_(is)
    , error_os_(error_os) {
    /* split input into lines */
    std::string line;

    DEBUG_PRINT("tokens:");
    while (std::getline(is_, line)) {
        std::stringstream aux; // special symbols separated with spaces to make parsing easier
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
                error_os_ << "Invalid DFA specification: unknown character '" << c << "'" << std::endl;
                return false;
            }
            */ 
            
        }

        /* second pass: split tokens */
        std::string tok;
        while (aux >> tok) {
            /* special case: whitespace character, next "token" necessary */
            if (tok == "'") {
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
            /* regular case */
            TokenType toktype = get_token_type(tok);
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

        /* print tokens in debug mode */
        #if DEBUG_PRINT_ENABLED
        std::cout << "{";
        for (auto token : tokens) {
            std::cout << "[" << token.type_ << ": " << token.token_ << "], ";
        }
        std::cout << "\b\b}\n";
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
                state_name_error(line_no_, line, state_name);
                continue;
            }
            states_.try_emplace(state_name, false);
            start_state_name_ = tokens.back().token_;
        } else if (tokens.front().type_ == TokenType::ACCEPT_MARKER) {
            if (tokens.size() < 2) {
                fmt_error(line_no_, line, "no accepting states specified after +");
                continue;
            }
            /* Accepting state declaration; state name(s) present */
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
            if (!(tokens.size() == 4
                && tokens[0].type_ == TokenType::STATE_NAME
                && tokens[1].type_ == TokenType::SYMBOL
                && tokens[2].type_ == TokenType::RARROW
                && tokens[3].type_ == TokenType::STATE_NAME
            )) {
                /* additional error detection */
                if (tokens.size() == 4) {
                    if (tokens[0].type_ != TokenType::STATE_NAME) {
                        state_name_error(line_no_, line, tokens[0].token_);
                    }
                    if (tokens[1].type_ != TokenType::SYMBOL) {
                        sym_error(line_no_, line, tokens[1].token_);
                    }
                    if (tokens[3].type_ != TokenType::STATE_NAME) {
                        state_name_error(line_no_, line, tokens[3].token_);
                    }
                }
                fmt_error(line_no_, line, "wrong transition format");
                continue; // to the next line
            }
            std::string state_from = tokens[0].token_;
            std::string state_to = tokens[3].token_;

            /* currently should be always false: invalid names detected by get_token_type() */
            if (!is_valid_state_name(state_from)) {
                state_name_error(line_no_, line, state_from);
            }
            if (!is_valid_state_name(state_to)) {
                state_name_error(line_no_, line, state_to);
            }

            if (!valid_) {
                continue;
            }
            std::string char_on = tokens[1].token_;

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
    #if DEBUG_PRINT_ENABLED
    std::cout.flush();
    #endif
    if (!is_valid_state_name(start_state_name_)) {
        valid_ = false;
        status_flags_ |= FAGenStatusFlag::NO_INIT;
        error_os_ << "Invalid DFA specification: no initial state (with a valid name)" << std::endl;
    }
}

unsigned int DFAGenerator::write(std::ostream& os, std::string dfa_name) {
    if (!is_valid_id(dfa_name)) {
        error_os_ << "Cannot generate code: DFA name \"" << dfa_name << "\" contains characters "
                    << "other than alphanumeric characters and underscore." << std::endl;
        return FAGenStatusFlag::WRONG_NAME;
    }

    if (!valid_) {
        error_os_ << "Cannot generate code for '" << dfa_name 
                    << "': input specification invalid." << std::endl;
        return status_flags_;
    }

    os << "#ifndef DFA_" << dfa_name << "_H__\n"
        << "#define DFA_" << dfa_name << "_H__\n"
        << "\n"
        << "namespace DFA_" << dfa_name << " {\n"
        << "\n"
        << "void init(Automata::DFA& dfa) {"
        << std::endl;

    /* add start state */
    os << TAB << "auto& q" << start_state_name_ << " = dfa.get_initial_state(" 
        << (states_.at(start_state_name_) ? "true" : "false") << ");" << std::endl;

    os << std::endl;
    
    /* add all other states */
    for (auto [state_name, is_accepting] : states_) {
        if (state_name == start_state_name_) continue;

        os << TAB << "auto& q" << state_name << " = dfa.new_state(" 
            << (is_accepting ? "true" : "false") << ");" << std::endl;
    }

    os << std::endl;

    /* add transitions */
    for (auto [state_from, map] : transitions_) {
        for (auto [char_on, state_to] : map) {
            os << TAB << "dfa.add_transition(" 
                << "q" << state_from << ", "
                << char_on << ", "
                << "q" << state_to
                << ");" << std::endl;
        }
    }

    os << "}\n"
        << "\n"
        << "} // namespace DFA_" << dfa_name << "\n"
        << "\n"
        << "#endif" << std::endl;
    return FAGenStatusFlag::OK;
}

} // namespace Automata