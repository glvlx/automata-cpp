#ifndef DFA_nfa_accept_even_H__
#define DFA_nfa_accept_even_H__

namespace DFA_nfa_accept_even {

void init(Automata::DFA& dfa) {
    auto& q3455027185487056046 = dfa.get_initial_state(false);

    auto& q14917219127612967247 = dfa.new_state(true);

    dfa.add_transition(q3455027185487056046, '1', q3455027185487056046);
    dfa.add_transition(q3455027185487056046, '0', q14917219127612967247);
    dfa.add_transition(q14917219127612967247, '1', q3455027185487056046);
    dfa.add_transition(q14917219127612967247, '0', q14917219127612967247);
}

} // namespace DFA_nfa_accept_even

#endif
