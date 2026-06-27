#include "nfa.hpp"

#include <iostream>

namespace Automata {

/*
 * NFAState
 */

NFAState::NFAState(std::string name, bool is_invalid):
    is_invalid_(is_invalid), name_(name) {}

NFAState invalid_nfa_state{"invalid", true};
std::unordered_set<NFAState*> invalid_nfa_transition;

const std::unordered_set<NFAState*>& NFAState::transition(std::string symbol) const {
    auto next_or_null_it = transitions_.find(symbol);
    if (next_or_null_it == transitions_.cend()) {
        return invalid_nfa_transition;
    }
    
    return next_or_null_it->second;
}

bool operator==(const NFAState& lhs, const NFAState& rhs) {
    return lhs.id_ == rhs.id_;
}
bool operator!=(const NFAState& lhs, const NFAState& rhs) {
    return !(lhs == rhs);
}

/*
 * NFA
 */

NFA::NFA(std::unordered_set<std::string> alphabet):
    alphabet_(alphabet) {}

bool NFA::add_state(std::string state_name) {
    bool success = states_.emplace(std::make_pair(state_name, std::make_unique<NFAState>(state_name))).second;
    return success;
}

bool NFA::add_transition(std::string symbol, std::string state_from, std::string state_to) {
    bool args_valid = alphabet_.count(symbol) > 0 && states_.count(state_from) > 0 && states_.count(state_to) > 0;
    if (!args_valid) {
        return false;
    }

    NFAState* rstate_from = states_.at(state_from).get();
    NFAState* rstate_to = states_.at(state_to).get();
    bool success = rstate_from->transitions_.at(rstate_from->name_).emplace(rstate_to).second;
    return success;
}

bool NFA::set_start_state(std::string state_name) {
    bool args_valid = states_.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    start_state_ = states_.at(state_name).get();
    return true;
}

bool NFA::add_end_state(std::string state_name) {
    bool args_valid = states_.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    bool success = end_states_.emplace(states_.at(state_name).get()).second;
    return success;
}

void NFA::reset() {
    alphabet_.clear();
    states_.clear();
    start_state_ = &invalid_nfa_state;
    end_states_.clear();
}

bool NFA::test(std::string word) const {
    // TODO
    std::cerr << "Warning: NFA::test hasn't been implemented yet" << std::endl;
    return false;
}

bool NFA::verify() const {
    bool start_state_valid = states_.count(start_state_->name_) > 0;
    bool end_states_valid = true;
    for (const NFAState* rstate : end_states_) {
        std::string state_name = rstate->name_;
        if (states_.count(state_name) == 0) {
            end_states_valid = false;
            break;
        }
    }
    if (!start_state_valid || !end_states_valid) {
        return false;
    }

    for (auto &r_symbolstate_pair : states_) {
        const Automata::NFAState& state = *r_symbolstate_pair.second;

        for (auto & r_symbolset_pair : state.transitions_) {
            const std::unordered_set<NFAState*>& transition_set = r_symbolset_pair.second;
            for (const NFAState* next_state : transition_set) {
                if (states_.find(next_state->name_) == states_.end())
                    return false;
            }
        }
    }

    return true;
}

} // namespace Automata