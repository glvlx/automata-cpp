#ifndef DFA_AUTOGEN_H_
#define DFA_AUTOGEN_H_

#include <unordered_map>
#include <list>
#include <string>
#include <sstream>

#include "util.hpp"

namespace Automata {
class AutoDFA {
public:
    class AutoDFAState {
        inline static int max_id_ = 0;
    public:
        const int id_ = max_id_++;
        bool is_accepted_ = false;
        AutoDFAState(bool is_accepted = false): is_accepted_(is_accepted) {
            std::stringstream dbgs;
            dbgs << "new state at " << this;
            debug_print(dbgs.str());
        }
    };

private:
    std::list<AutoDFAState> states_ {
        AutoDFAState()
    };
    std::unordered_map<AutoDFAState*, std::unordered_map<char, AutoDFAState*>> transitions_ = {
        {&states_.front(), {}}
    };
    AutoDFAState* state_ = &states_.front();

public:
    AutoDFAState& get_start_state() {
        std::stringstream dbgs;
        dbgs << "new reference to " << &states_.front();
        debug_print(dbgs.str());

        return states_.front();
    }

    AutoDFAState& new_state(bool is_accepted = false, bool make_start_state = false) {
        if (make_start_state) {
            states_.emplace_front(is_accepted);
        } else {
            states_.emplace_back(is_accepted);
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