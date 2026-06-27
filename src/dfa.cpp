#include "dfa.hpp"

#include <iostream>

namespace Automata {

/*
 * DFAState
 */

DFAState::DFAState(std::string name, bool is_invalid):
    is_invalid_(is_invalid), name_(name)  {}

DFAState invalid_dfa_state{"invalid", true};

const DFAState* DFAState::transition(std::string symbol) const {
    auto next_or_null_it = transitions_.find(symbol);
    if (next_or_null_it == transitions_.cend()) {
        return &invalid_dfa_state;
    }
    
    return next_or_null_it->second;
}

bool operator==(const DFAState& lhs, const DFAState& rhs) {
    return lhs.id_ == rhs.id_;
}
bool operator!=(const DFAState& lhs, const DFAState& rhs) {
    return !(lhs == rhs);
}

/*
 * DFA
 */

DFA::DFA(std::unordered_set<std::string> alphabet): 
    alphabet_(alphabet) {}

bool DFA::add_state(std::string state_name) {
    bool success = states_.emplace(std::make_pair(state_name, std::make_unique<DFAState>(state_name))).second;
    return success;
}

bool DFA::add_transition(std::string symbol, std::string state_from, std::string state_to) {
    bool args_valid = alphabet_.count(symbol) > 0 && states_.count(state_from) > 0 && states_.count(state_to) > 0;
    if (!args_valid) {
        return false;
    }

    DFAState* rstate_from = states_.at(state_from).get();
    DFAState* rstate_to = states_.at(state_to).get();
    bool success = rstate_from->transitions_.emplace(std::make_pair(symbol, rstate_to)).second;
    return success;
}

bool DFA::set_start_state(std::string state_name) {
    bool args_valid = states_.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    start_state_ = states_.at(state_name).get();
    return true;
}

bool DFA::add_end_state(std::string state_name) {
    bool args_valid = states_.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    bool success = end_states_.emplace(states_.at(state_name).get()).second;
    return success;
}

void DFA::reset() {
    alphabet_.clear();
    states_.clear();
    start_state_ = &invalid_dfa_state;
    end_states_.clear();
}

bool DFA::test(std::string word) const {
#if 0
    // necessary?
    if (!verify()) {
        return false;
    }
#endif

    const DFAState* pstate = start_state_;
    std::string symbol;
    for (auto c : word) {
        symbol = c;
        bool symbol_valid = alphabet_.count(symbol) > 0;
        if (!symbol_valid) {
            return false;
        }

        pstate = pstate->transition(symbol); // guaranteed to exist (kind of)
        bool transition_exists = *pstate != invalid_dfa_state;
        if (!transition_exists) {
            return false;
        }
    }
    return end_states_.count(pstate) > 0;
}

bool DFA::verify() const {
    bool start_state_valid = states_.count(start_state_->name_) > 0;
    bool end_states_valid = true;
    for (const DFAState* rstate : end_states_) {
        std::string state_name = rstate->name_;
        if (states_.count(state_name) == 0) {
            end_states_valid = false;
            break;
        }
    }
    if (!start_state_valid || !end_states_valid) {
        return false;
    }
    return true;
}

void DFA::minimize() {
    // TODO
    std::cerr << "Warning: DFA minimization hasn't been implemented yet" << std::endl;
}

} // namespace DFA