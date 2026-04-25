#include <iostream>

#include <memory>
#include <unordered_map>

#include "dfa.hpp"

int main() {
    DFA a;
    // all binary numbers that start and end with 1
    a.add_symbol("0");
    a.add_symbol("1");

    a.add_state("q0");
    a.add_state("q1");
    a.add_state("q2");

    a.add_transition("0", "q0", a.invalid_state);
    a.add_transition("1", "q0", "q1");
    a.add_transition("0", "q1", "q2");
    a.add_transition("1", "q1", "q1");
    a.add_transition("0", "q2", "q2");
    a.add_transition("1", "q2", "q1");

    a.set_start_state("q0");
    a.add_end_state("q1");

    std::string input;
    for (;;) {
        std::cout << "Enter a (binary) word: ";
        std::cin >> input;
        std::cout << input << " is " << (a.test(input) ? "" : "not ") << "accepted." << std::endl;
    }

    return 0;
}