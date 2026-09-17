#include <iostream>

#include <memory>
#include <unordered_map>
#include <cstdlib>

#include "dfa-autogen.hpp"
#include "_auto_bin_11.hpp"

using Automata::AutoDFA;

int main() {
    AutoDFA a;
    DFA_bin_11::init(a);

    std::string in;
    std::cout << "Enter a binary string: ";
    std::cin >> in;
    bool is_correct = a.check(in);
    std::cout << in << " is " << (is_correct ? "correct" : "incorrect") << std::endl;

    return 0;
}