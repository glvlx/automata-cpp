#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

#include "dfa.hpp"

#include "generated_dfa/bin_11.hpp"
#include "generated_dfa/accept_empty.hpp"
#include "generated_dfa/accept_none.hpp"
#include "generated_dfa/accept_even.hpp"
#include "generated_dfa/whitespace_symbol.hpp"
#include "generated_dfa/rwth.hpp"

#include "generated_nfa/nfa_bin_11.hpp"
#include "generated_nfa/nfa_accept_empty.hpp"
#include "generated_nfa/nfa_accept_none.hpp"
#include "generated_nfa/nfa_accept_even.hpp"
#include "generated_nfa/nfa_whitespace_symbol.hpp"
#include "generated_nfa/nfa_rwth.hpp"

#define BOOL_TO_STR(x) ((x) ? "true" : "false")

void end_to_end(const std::vector<std::pair<std::string, bool>>& test_cases, void (*init_function)(Automata::DFA&)) {
    Automata::DFA sut;
    init_function(sut);

    for (const auto& [str, expected] : test_cases) {
        EXPECT_EQ(sut.test(str), expected) << "(str = \"" << str << "\")";
    }
}

void test_AcceptEmpty(void (*init_function)(Automata::DFA&)) {
    end_to_end({
        {"", true},
        {"0", false},
        {"1", false},
        {"01", false}
    }, init_function);
}

TEST(CorrectDFAGenerationTest, AcceptEmpty) {
    test_AcceptEmpty(DFA_accept_empty::init);
}

TEST(CorrectNFAasDFATest, AcceptEmpty) {
    test_AcceptEmpty(DFA_nfa_accept_empty::init);
}

void test_AcceptNone(void (*init_function)(Automata::DFA&)) {
    end_to_end({
        {"", false},
        {"0", false},
        {"1", false},
        {"01", false}
    }, init_function);
}

TEST(CorrectDFAGenerationTest, AcceptNone) {
    test_AcceptNone(DFA_accept_none::init);
}

TEST(CorrectNFAasDFATest, AcceptNone) {
    test_AcceptNone(DFA_nfa_accept_none::init);
}

void test_Bin11(void (*init_function)(Automata::DFA&)) {
    end_to_end({
        {"", false},
        {"0", false},
        {"1", true},
        {"11", true},
        {"111", true},
        {"011", false},
        {"110", false}
    }, init_function);
}

TEST(CorrectDFAGenerationTest, Bin11) {
    test_Bin11(DFA_bin_11::init);
}

TEST(CorrectNFAasDFATest, Bin11) {
    test_Bin11(DFA_nfa_bin_11::init);
}

void test_AcceptEven(void (*init_function)(Automata::DFA&)) {
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
    }, init_function);
}

TEST(CorrectDFAGenerationTest, AcceptEven) {
    test_AcceptEven(DFA_accept_even::init);
}

TEST(CorrectNFAasDFATest, AcceptEven) {
    test_AcceptEven(DFA_nfa_accept_even::init);
}

void test_WhitespaceSymbol(void (*init_function)(Automata::DFA&)) {
    end_to_end({
        {"", false},
        {"12", false},
        {"1 2", true},
        {"1\t2", false},
        {"122", false}
    }, init_function);
}

TEST(CorrectDFAGenerationTest, WhitespaceSymbol) {
    test_WhitespaceSymbol(DFA_whitespace_symbol::init);
}

TEST(CorrectNFAasDFATest, WhitespaceSymbol) {
    test_WhitespaceSymbol(DFA_nfa_whitespace_symbol::init);
}

void test_AcceptRWTH(void (*init_function)(Automata::DFA&)) {
    end_to_end({
        {"", false},
        {"RWTH", true},
        {"rwth", false},
        {"1234", false}
    }, init_function);
}

TEST(CorrectDFAGenerationTest, AcceptRWTH) {
    test_AcceptRWTH(DFA_rwth::init);
}

TEST(CorrectNFAasDFATest, AcceptRWTH) {
    test_AcceptRWTH(DFA_nfa_rwth::init);
}