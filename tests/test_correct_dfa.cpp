#include <iostream>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "dfa_generator.hpp"

#include "generated_dfa/bin_11.hpp"
#include "generated_dfa/accept_empty.hpp"
#include "generated_dfa/accept_none.hpp"
#include "generated_dfa/accept_even.hpp"
#include "generated_dfa/rwth.hpp"

#define BOOL_TO_STR(x) ((x) ? "true" : "false")

void end_to_end(const std::vector<std::pair<std::string, bool>>& test_cases, void (*init_function)(Automata::AutoDFA&)) {
    Automata::AutoDFA sut;
    init_function(sut);

    for (const auto& [str, expected] : test_cases) {
        EXPECT_EQ(sut.check(str), expected) << "(str = \"" << str << "\")";
    }
}

TEST(CorrectDFATest, TestAcceptEmpty) {
    end_to_end({
        {"", true},
        {"0", false},
        {"1", false},
        {"01", false}
    }, DFA_accept_empty::init);
}

TEST(CorrectDFATest, TestAcceptNone) {
    end_to_end({
        {"", false},
        {"0", false},
        {"1", false},
        {"01", false}
    }, DFA_accept_none::init);
}

TEST(CorrectDFATest, TestBin11) {
    end_to_end({
        {"", false},
        {"0", false},
        {"1", true},
        {"11", true},
        {"111", true},
        {"011", false},
        {"110", false}
    }, DFA_bin_11::init);
}

TEST(CorrectDFATest, TestAcceptEven) {
    end_to_end({
        {"", false},
        {"0", true},
        {"1", false},
        {"01", false},
        {"11", false},
        {"000", true},
        {"111", false},
        {"011", false},
        {"010", true},
        {"110", true}
    }, DFA_accept_even::init);
}

TEST(CorrectDFATest, TestRWTH) {
    end_to_end({
        {"", false},
        {"RWTH", true},
        {"rwth", false},
        {"1234", false}
    }, DFA_rwth::init);
}