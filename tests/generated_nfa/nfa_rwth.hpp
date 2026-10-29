#ifndef DFA_nfa_rwth_H__
#define DFA_nfa_rwth_H__

namespace DFA_nfa_rwth {

void init(Automata::DFA& dfa) {
    auto& q3455027185487056046 = dfa.get_initial_state(false);

    auto& q17214108958978852434 = dfa.new_state(false);
    auto& q13612557235615985865 = dfa.new_state(false);
    auto& q2818565491802442532 = dfa.new_state(false);
    auto& q1057967041962763294 = dfa.new_state(true);

    dfa.add_transition(q3455027185487056046, 'R', q17214108958978852434);
    dfa.add_transition(q13612557235615985865, 'H', q1057967041962763294);
    dfa.add_transition(q17214108958978852434, 'W', q2818565491802442532);
    dfa.add_transition(q2818565491802442532, 'T', q13612557235615985865);
}

} // namespace DFA_nfa_rwth

#endif
