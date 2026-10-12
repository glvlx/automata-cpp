#include <iostream>
#include <utility>
#include "dfa_generator.hpp"

#include "generated_dfa/bin_11.hpp"

#define BOOL_TO_STR(x) ((x) ? "true" : "false")

int main() {
    std::pair<std::string, bool> test_cases[] {
        {"", false},
        {"0", false},
        {"1", true},
        {"11", true},
        {"111", true},
        {"011", false},
        {"110", false}
    };

    const int total_count = sizeof(test_cases) / sizeof(*test_cases);
    int passed_count = total_count;

    Automata::AutoDFA dfa_bin_11;
    DFA_bin_11::init(dfa_bin_11);

    for (const auto& [str, expected] : test_cases) {
        if (dfa_bin_11.check(str) != expected) {
            passed_count--;
            std::cout << "bin_11: expected " << BOOL_TO_STR(expected) << " on \"" << str 
                      << "\", but DFA returned " << BOOL_TO_STR(!expected) << std::endl;
        }
    }

    return passed_count != total_count;
}