#ifndef DFA_nfa_whitespace_symbol_H__
#define DFA_nfa_whitespace_symbol_H__

namespace DFA_nfa_whitespace_symbol {

void init(Automata::DFA& dfa) {
    auto& q3455027185487056046 = dfa.get_initial_state(false);

    auto& q14917219127612967247 = dfa.new_state(false);
    auto& q6756326855279698475 = dfa.new_state(false);
    auto& q26431177340862227 = dfa.new_state(true);

    dfa.add_transition(q3455027185487056046, '1', q14917219127612967247);
    dfa.add_transition(q14917219127612967247, ' ', q6756326855279698475);
    dfa.add_transition(q6756326855279698475, '2', q26431177340862227);
}

} // namespace DFA_nfa_whitespace_symbol

#endif
