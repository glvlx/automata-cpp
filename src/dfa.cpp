#include "dfa.hpp"

#if 0
// DFAState
DFAState::DFAState(std::string name):
    name(name) {}

std::string DFAState::get_name() {
    return name;
}

void DFAState::set_name(std::string name) {
    this->name = name;
}

int DFAState::get_id() {
    return id;
}
#endif

// DFA
#if 0
DFA::DFA():
    states({ { invalid_state, std::make_shared<DFAState>(invalid_state) } }) {}

DFA::DFA(std::unordered_set<std::string> alphabet):
    states({ { invalid_state, std::make_shared<DFAState>(invalid_state) } }),
    alphabet(alphabet) {}
#endif

DFA::DFA(std::unordered_set<std::string> alphabet):
    states({ invalid_state }),
    alphabet(alphabet) {}

bool DFA::add_symbol(std::string symbol) {
    return alphabet.insert(symbol).second;
}

bool DFA::add_state(std::string state_name) {
    return states.insert(state_name).second;
}

bool DFA::add_transition(std::string symbol, std::string state_from, std::string state_to) {
    bool args_valid = alphabet.count(symbol) > 0 && states.count(state_from) > 0 && states.count(state_to) > 0;
    if (!args_valid) {
        return false;
    }

    transitions.insert({ state_from, std::unordered_map<std::string, std::string>()} );
    return transitions[state_from].insert({ symbol, state_to }).second;
}

bool DFA::set_start_state(std::string state) {
    bool args_valid = states.count(state) > 0;
    if (!args_valid) {
        return false;
    }

    start_state = state;
    return true;
}

bool DFA::add_end_state(std::string state) {
    bool args_valid = states.count(state) > 0;
    if (!args_valid) {
        return false;
    }

    return end_states.insert(state).second;
}

void DFA::reset() {
    alphabet.clear();
    states.clear();
    states.insert(invalid_state);
    transitions.clear();
    start_state = invalid_state;
    end_states.clear();
}

bool DFA::test(std::string word) {
#if 0
    // necessary?
    if (!verify()) {
        return false;
    }
#endif

    std::string state = start_state;
    std::string symbol;
    for (auto c : word) {
        symbol = {c};
        bool symbol_valid = alphabet.count(symbol) > 0;
        if (!symbol_valid) {
            return false;
        }

        bool transition_exists = transitions.count(state) > 0
            && transitions[state].count(symbol) > 0;
        if (!transition_exists) {
            return false;
        }

        state = transitions[state][symbol];
    }
    return end_states.count(state) > 0;
}

bool DFA::verify() {
    bool start_state_valid = states.count(start_state) > 0;
    bool end_states_valid = true;
    for (std::string state : end_states) {
        if (states.count(state) == 0) {
            end_states_valid = false;
            break;
        }
    }
    if (!start_state_valid || !end_states_valid) {
        return false;
    }

    for (std::string state : states) {
        if (state == invalid_state) {
            continue;
        }
        
        bool state_defined = transitions.count(state) > 0;
        if (!state_defined) {
            return false;
        }
        for (std::string symbol : alphabet) {
            bool symbol_defined = transitions[state].count(symbol) > 0;
            if (!symbol_defined) {
                return false;
            }
            bool next_state_valid = states.count(transitions[state][symbol]) > 0;
            if (!next_state_valid) {
                return false;
            }
        }
    }
    return true;
}