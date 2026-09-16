#include <iostream>
#include <fstream>

#define DEBUG_PRINT true
#include "dfa-autogen.hpp"

using Automata::AutoDFA;

int main() {
    // all binary numbers that start and end with 1

    AutoDFA a;

    auto& q0 = a.get_start_state();
    auto& q1 = a.new_state(true);
    auto& q2 = a.new_state();

    a.add_transition(q0, '1', q1);
    a.add_transition(q1, '0', q2);
    a.add_transition(q1, '1', q1);
    a.add_transition(q2, '0', q2);
    a.add_transition(q2, '1', q1);

    std::string dfa_fpath = "./bin_11.txt";
    std::ofstream dfa_file(dfa_fpath);
    bool result = a.serialize(dfa_file);
    if (result) {
        std::cout << "Correctly serialized to '" << dfa_fpath << "'" << std::endl;
    }
    dfa_file.close();
    if (!dfa_file) {
        std::cerr << "Failed to close '" << dfa_fpath << "'" << std::endl;
    }

    std::string in;
    std::cout << "Enter a binary string: ";
    std::cin >> in;
    bool is_correct = a.check(in);
    std::cout << in << " is " << (is_correct ? "correct" : "incorrect") << std::endl;


    return 0;
}