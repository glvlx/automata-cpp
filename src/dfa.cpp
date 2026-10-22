#include <iostream>

#include "dfa.hpp"

namespace Automata {

DFA::DFAState& DFA::get_start_state(bool is_accepting) {
    std::stringstream dbgs;
    dbgs << "new reference to " << &states_.front();
    DEBUG_PRINT(dbgs.str());

    if (is_accepting) {
        states_.front().is_accepting_ = true;
    }

    return states_.front();
}

DFA::DFAState& DFA::new_state(bool is_accepting, bool make_start_state) {
    if (make_start_state) {
        states_.emplace_front(this, is_accepting);
    } else {
        states_.emplace_back(this, is_accepting);
    }
    DFAState& state = (make_start_state) ? states_.front() : states_.back();
    
    std::stringstream dbgs;
    dbgs << "new reference to " << &state;
    DEBUG_PRINT(dbgs.str());

    transitions_.emplace(&state, 
                            std::unordered_map<char, DFAState*>());
    return state;
}

/**
 * Adds a transition. Can construct necessary entries automatically.
 * 
 * @return Whether a new transition has been created or not (= an old one replaced)
 */
bool DFA::add_transition(DFAState& from, char on, DFAState& to) {
    auto [it_from, inserted_from] = transitions_.try_emplace(&from, 
        std::unordered_map<char, DFAState*>()); // checks if key exists; if not, copies {}
    auto [it_sym, inserted_sym] = it_from->second.insert_or_assign(on, &to);
    if (!inserted_sym) {
        std::cerr << "Warning: a transition has been replaced" << std::endl;
    }
    return inserted_sym;
}

bool DFA::step(char symbol) {
    auto next_state_it = transitions_[state_].find(symbol);
    if (next_state_it == transitions_[state_].end()) {
        return false;
    } else {
        state_ = next_state_it->second;
        return true;
    }
}

bool DFA::check(std::string in) {
    state_ = &get_start_state();

    for (char s : in) {
        if (!step(s)) {
            return false;
        }
    }
    return state_->is_accepting_;
}

bool DFA::serialize(std::ostream& os) {
    DFAState& start_state = get_start_state();

    /* Start state */
    os << "! q" << start_state.id_ << std::endl;

    /* Terminal states */
    bool terminal_present = false;
    for (auto& state : states_) {
        if (!state.is_accepting_) {
            continue;
        }
        // else: state accepting
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

} // namespace Automata