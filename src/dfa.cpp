#include "dfa.hpp"

namespace Automata {

/*
 * DFAState
 */

DFAState::DFAState(std::string name, bool is_invalid):
    is_invalid(is_invalid), name(name)  {}

DFAState invalid_state{"invalid", true};

DFAState* DFAState::transition(std::string symbol) const {
    auto next_or_null_it = transitions.find(symbol);
    if (next_or_null_it == transitions.cend()) {
        return &invalid_state;
    }
    
    return next_or_null_it->second;
}

bool operator==(const DFAState& lhs, const DFAState& rhs) {
    return lhs.id == rhs.id;
}

bool operator!=(const DFAState& lhs, const DFAState& rhs) {
    return !(lhs == rhs);
}

/*
 * DFA
 */

DFA::DFA(std::unordered_set<std::string> alphabet): 
    alphabet(alphabet) {}

bool DFA::add_symbol(std::string symbol) {
    return alphabet.insert(symbol).second;
}

bool DFA::add_state(std::string state_name) {
    bool success = states.emplace(std::make_pair(state_name, std::make_unique<DFAState>(state_name))).second;
    return success;
}

bool DFA::add_transition(std::string symbol, std::string state_from, std::string state_to) {
    bool args_valid = alphabet.count(symbol) > 0 && states.count(state_from) > 0 && states.count(state_to) > 0;
    if (!args_valid) {
        return false;
    }

    DFAState* rstate_from = states.at(state_from).get();
    DFAState* rstate_to = states.at(state_to).get();
    bool success = rstate_from->transitions.emplace(std::make_pair(symbol, rstate_to)).second;
    return success;
}

bool DFA::set_start_state(std::string state_name) {
    bool args_valid = states.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    start_state = states.at(state_name).get();
    return true;
}

bool DFA::add_end_state(std::string state_name) {
    bool args_valid = states.count(state_name) > 0;
    if (!args_valid) {
        return false;
    }

    bool success = end_states.emplace(states.at(state_name).get()).second;
    return success;
}

void DFA::reset() {
    alphabet.clear();
    states.clear();
    start_state = &invalid_state;
    end_states.clear();
}

bool DFA::test(std::string word) {
#if 0
    // necessary?
    if (!verify()) {
        return false;
    }
#endif

    const DFAState* pstate = start_state;
    std::string symbol;
    for (auto c : word) {
        symbol = c;
        bool symbol_valid = alphabet.count(symbol) > 0;
        if (!symbol_valid) {
            return false;
        }

        pstate = pstate->transition(symbol); // guaranteed to exist (kind of)
        bool transition_exists = *pstate != invalid_state;
        if (!transition_exists) {
            return false;
        }
    }
    return end_states.count(pstate) > 0;
}

bool DFA::verify() {
    bool start_state_valid = states.count(start_state->name) > 0;
    bool end_states_valid = true;
    for (const DFAState* rstate : end_states) {
        std::string state_name = rstate->name;
        if (states.count(state_name) == 0) {
            end_states_valid = false;
            break;
        }
    }
    if (!start_state_valid || !end_states_valid) {
        return false;
    }
    return true;
}

} // namespace DFA