#ifndef DFA_H_
#define DFA_H_

#include <string>
#include <list>
#include <unordered_map>

namespace Automata {

class DFA {
private:
    int max_id_ = 0;
public:
    class DFAState {
        inline static int max_id_ = 0;
    public:
        DFA* const parent_; 
        const int id_;
        bool is_accepting_ = false;
        DFAState(DFA* const parent, bool is_accepting = false);
    };

private:
    /* Invariant: initial state is always at the front of states_ */
    std::list<DFAState> states_ {
        DFAState(this)
    };
    std::unordered_map<DFAState*, std::unordered_map<char, DFAState*>> transitions_ = {
        {&states_.front(), {}}
    };
    DFAState* state_ = &states_.front();

public:
    /**
     * @param make_accepting If true, makes the state accepting, otherwise doesn't change this property.
     * 
     * @return A reference to the current initial state (necessary for adding transitions)
     */
    DFAState& get_initial_state(bool make_accepting = false);

    /**
     * Creates a new state with provided parameters.
     * 
     * @return A reference to the new state (necessary for adding transitions)
     */
    DFAState& new_state(bool is_accepting = false, bool make_start_state = false);

    /**
     * Adds a transition. Can construct necessary entries automatically.
     * 
     * @return Whether a new transition has been created or not (= an old one replaced)
     */
    bool add_transition(DFAState& from, char on, DFAState& to);

    /**
     * Tries to carry out a single transition from the DFA's current state.
     * 
     * @return Whether a step could be performed
     */
    bool step(char symbol);

    /**
     * Tests a string with the DFA.
     * The DFA is set to its initial state beforehand.
     * 
     * @return Whether the string is in the DFA's language.
     */
    bool test(std::string in);

    /**
     * Writes a specification corresponding to the DFA to an ostream.
     * 
     * @return Whether serialization was successful
     */
    bool serialize(std::ostream& os);
};

} // namespace Automata

#endif