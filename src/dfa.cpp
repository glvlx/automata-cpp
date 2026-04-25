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