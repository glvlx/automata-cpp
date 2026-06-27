#ifndef NFA_H_
#define NFA_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>

namespace Automata {

class NFAState {
private:
    static inline int next_id_ = 0; // so that invalid_nfa_state.id_ == 0

public:
    const int id_ = next_id_++;
    bool is_invalid_ = false;
    std::string name_;
    std::unordered_map<std::string, std::unordered_set<NFAState*>> transitions_;

    NFAState() = delete;
    NFAState(std::string name, bool is_invalid=false);
    ~NFAState() = default;

    /**
     * If transition exists, returns a reference to the list of possible next states;
     * otherwise, returns a reference to the shared global empty list `invalid_nfa_transition`
     */
    const std::unordered_set<NFAState*>& transition(std::string symbol) const;
};

extern NFAState invalid_nfa_state;
extern std::unordered_set<NFAState*> invalid_nfa_transition;

bool operator==(const NFAState& lhs, const NFAState& rhs);
bool operator!=(const NFAState& lhs, const NFAState& rhs);

class NFA {
private:
    std::unordered_map<std::string, std::unique_ptr<NFAState>> states_;
    const NFAState* start_state_ = &invalid_nfa_state;
    std::unordered_set<const NFAState*> end_states_;

    // const NFAState* transition(std::string symbol, const NFAState* state);

public:
    std::unordered_set<std::string> alphabet_ = {};

    NFA(std::unordered_set<std::string> alphabet = {});
    ~NFA() = default;
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