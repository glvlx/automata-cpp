#ifndef DFA_H_
#define DFA_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <functional>

namespace Automata {

class DFAState {
private:
    static inline int next_id = 0; // so that invalid_state.id == 0

public:
    const int id = next_id++;
    bool is_invalid = false;
    std::string name;
    std::unordered_map<std::string, DFAState*> transitions;

    DFAState() = delete;
    DFAState(std::string name, bool is_invalid=false);
    ~DFAState() = default;

    /**
     * If transition exists, returns a reference to the next state;
     * otherwise, returns a reference to the shared global invalid state
     */
    DFAState *transition(std::string symbol) const;
};

extern DFAState invalid_state;

bool operator==(const DFAState& lhs, const DFAState& rhs);
bool operator!=(const DFAState& lhs, const DFAState& rhs);

class DFA {
private:
    std::unordered_map<std::string, std::unique_ptr<DFAState>> states;
    const DFAState* start_state = &invalid_state;
    std::unordered_set<const DFAState*> end_states;

    // const DFAState* transition(std::string symbol, const DFAState* state);

public:
    std::unordered_set<std::string> alphabet = {};

    DFA(std::unordered_set<std::string> alphabet = {});
    ~DFA() = default;

    bool add_symbol(std::string symbol);
    bool add_state(std::string state_name);
    bool add_transition(std::string symbol, std::string state_from, std::string state_to);
    bool set_start_state(std::string state);
    bool add_end_state(std::string state);

    void reset();
    bool test(std::string word);
    bool verify();
};

} // namespace DFA

#endif