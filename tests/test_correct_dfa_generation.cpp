#include <iostream>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

#include "dfa_generator.hpp"

#include "generated_dfa/bin_11.hpp"
#include "generated_dfa/accept_empty.hpp"
#include "generated_dfa/accept_none.hpp"
#include "generated_dfa/accept_even.hpp"
#include "generated_dfa/whitespace_symbol.hpp"
#include "generated_dfa/rwth.hpp"

#define BOOL_TO_STR(x) ((x) ? "true" : "false")

void end_to_end(const std::vector<std::pair<std::string, bool>>& test_cases, void (*init_function)(Automata::DFA&)) {
    Automata::DFA sut;
    init_function(sut);

    for (const auto& [str, expected] : test_cases) {
        EXPECT_EQ(sut.check(str), expected) << "(str = \"" << str << "\")";
    }
}

TEST(CorrectDFAGenerationTest, AcceptEmpty) {
    end_to_end({
        {"", true},
        {"0", false},
        {"1", false},
        {"01", false}
    }, DFA_accept_empty::init);
}

TEST(CorrectDFAGenerationTest, AcceptNone) {
    end_to_end({
        {"", false},
        {"0", false},
        {"1", false},
        {"01", false}
    }, DFA_accept_none::init);
}

TEST(CorrectDFAGenerationTest, Bin11) {
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

TEST(CorrectDFAGenerationTest, AcceptEven) {
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

TEST(CorrectDFAGenerationTest, WhitespaceSymbol) {
    end_to_end({
        {"", false},
        {"12", false},
        {"1 2", true},
        {"1\t2", false},
        {"122", false}
    }, DFA_whitespace_symbol::init);
}

TEST(CorrectDFAGenerationTest, AcceptRWTH) {
    end_to_end({
        {"", false},
        {"RWTH", true},
        {"rwth", false},
        {"1234", false}
    }, DFA_rwth::init);
}