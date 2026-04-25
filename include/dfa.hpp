#ifndef DFA_H_
#define DFA_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>

#if 0
class DFAState {
private:
    static inline int next_id = 1;

    const int id = next_id++;
    std::string name;
public:
    DFAState() = delete;
    DFAState(std::string name);
    ~DFAState() = default;

    std::string get_name();
    void set_name(std::string name);
    int get_id();
};
#endif

class DFA {
public:
    const std::string invalid_state = "invalid";
private:
    std::unordered_set<std::string> alphabet = {};
    #if 0
    std::unordered_map<std::string, std::shared_ptr<DFAState>> states;
    std::weak_ptr<DFAState> transition(std::string, std::weak_ptr<DFAState>);
    std::weak_ptr<DFAState> start_state;
    std::unordered_set<std::shared_ptr<DFAState>> end_states;
    #endif
    std::set<std::string> states = { invalid_state };
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions = {};
    std::string start_state = invalid_state;
    std::unordered_set<std::string> end_states = {};

public:
    DFA() = default;
    DFA(std::unordered_set<std::string> alphabet);
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

#endif