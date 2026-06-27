#ifndef DFA_H_
#define DFA_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace Automata {

class DFAState {
private:
    static inline int next_id_ = 0; // so that invalid_dfa_state.id_ == 0

public:
    const int id_ = next_id_++;
    bool is_invalid_ = false;
    std::string name_;
    std::unordered_map<std::string, DFAState*> transitions_;

    DFAState() = delete;
    DFAState(std::string name, bool is_invalid=false);
    ~DFAState() = default;

    /**
     * If transition exists, returns a reference to the next state;
     * otherwise, returns a reference to the shared global invalid state
     */
    const DFAState* transition(std::string symbol) const;
};

extern DFAState invalid_dfa_state;

bool operator==(const DFAState& lhs, const DFAState& rhs);
bool operator!=(const DFAState& lhs, const DFAState& rhs);

class DFA {
private:
    std::unordered_map<std::string, std::unique_ptr<DFAState>> states_;
    const DFAState* start_state_ = &invalid_dfa_state;
    std::unordered_set<const DFAState*> end_states_;

    // const DFAState* transition(std::string symbol, const DFAState* state);

public:
    std::unordered_set<std::string> alphabet_ = {};

    DFA(std::unordered_set<std::string> alphabet = {});
    ~DFA() = default;

    bool add_state(std::string state_name);
    bool add_transition(std::string symbol, std::string state_from, std::string state_to);
    bool set_start_state(std::string state);
    bool add_end_state(std::string state);

    void reset();
    bool test(std::string word) const;
    bool verify() const;

    void minimize();
};

} // namespace DFA

#endif