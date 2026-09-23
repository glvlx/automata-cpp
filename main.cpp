#include <iostream>

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <cstdlib>
#include <fstream>

#include "dfa_autogen.hpp"
#include "_auto_bin_11.hpp"
#include "nfa_generator.hpp"

using Automata::AutoDFA, Automata::NFAGenerator, Automata::CompoundState, Automata::DFAGenerator;

int main() {
    #if 0
    AutoDFA a;
    DFA_bin_11::init(a);

    std::string in;
    std::cout << "Enter a binary string: ";
    std::cin >> in;
    bool is_correct = a.check(in);
    std::cout << in << " is " << (is_correct ? "correct" : "incorrect") << std::endl;
    #endif

    for (int i = 0; i < 3; i++) {
        CompoundState c(std::unordered_set<std::string>{"q1", "q2", "q3", "q4"});
        std::cout << i + 1 << ": hash = " << c.hash() << std::endl;
    }
    
    std::ifstream is("a_abstar.txt");
    NFAGenerator nfagen(is);
    std::ofstream dfas_o("_auto_a_abstar.dfa.txt");
    nfagen.make_dfa_spec(dfas_o);
    dfas_o.close();
    
    std::ifstream dfas_i("_auto_a_abstar.dfa.txt");
    DFAGenerator dfagen(dfas_i);
    std::ofstream dfacode("_auto_a_abstar.hpp");
    dfagen.deserialize(dfacode, "a_abstar");

    return 0;
}