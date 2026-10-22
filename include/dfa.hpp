#ifndef DFA_H_
#define DFA_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <sstream>
#include <list>

#include "util.hpp"

namespace Automata {

/**
 * 
 */
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
        DFAState(DFA* const parent, bool is_accepting = false)
        : parent_(parent)
        , id_(parent->max_id_++)
        , is_accepting_(is_accepting) {
            std::stringstream dbgs;
            dbgs << "new state at " << this;
            DEBUG_PRINT(dbgs.str());
        }
    };

private:
    std::list<DFAState> states_ {
        DFAState(this)
    };
    std::unordered_map<DFAState*, std::unordered_map<char, DFAState*>> transitions_ = {
        {&states_.front(), {}}
    };
    DFAState* state_ = &states_.front();

public:
    DFAState& get_start_state(bool is_accepting = false);

    DFAState& new_state(bool is_accepting = false, bool make_start_state = false);

    /**
     * Adds a transition. Can construct necessary entries automatically.
     * 
     * @return Whether a new transition has been created or not (= an old one replaced)
     */
    bool add_transition(DFAState& from, char on, DFAState& to);

    bool step(char symbol);

    bool check(std::string in);

    bool serialize(std::ostream& os);
};

} // namespace Automata

#endif