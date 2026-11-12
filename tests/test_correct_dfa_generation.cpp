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

#include "generated_nfa/fnfa_bin_11.hpp"
#include "generated_nfa/fnfa_accept_empty.hpp"
#include "generated_nfa/fnfa_accept_none.hpp"
#include "generated_nfa/fnfa_accept_even.hpp"
#include "generated_nfa/fnfa_whitespace_symbol.hpp"
#include "generated_nfa/fnfa_rwth.hpp"

#include "generated_nfa/nfa_abstar.hpp"
#include "generated_nfa/nfa_10_star_or_01_star.hpp"
#include "generated_nfa/nfa_multisym.hpp"
#include "generated_nfa/nfa_c_id.hpp"

#define BOOL_TO_STR(x) ((x) ? "true" : "false")

void end_to_end(const std::vector<std::pair<std::string, bool>>& test_cases, void (*init_function)(Automata::DFA&)) {
    Automata::DFA sut;
    init_function(sut);

    for (const auto& [str, expected] : test_cases) {
        EXPECT_EQ(sut.test(str), expected) << "(str = \"" << str << "\")";
    }
}

/********** DFA as DFA + as NFA **********/
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
    test_AcceptEmpty(DFA_fnfa_accept_empty::init);
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
    test_AcceptNone(DFA_fnfa_accept_none::init);
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
    test_Bin11(DFA_fnfa_bin_11::init);
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
    test_AcceptEven(DFA_fnfa_accept_even::init);
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
    test_WhitespaceSymbol(DFA_fnfa_whitespace_symbol::init);
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
    test_AcceptRWTH(DFA_fnfa_rwth::init);
}

/********** DFA from real NFA **********/
TEST(CorrectNFAGenerationTest, Test_AB_star) {
    end_to_end({
        {"", true},
        {"ab", true},
        {"ababab", true},
        {"a", false},
        {"b", false},
        {"aab", false},
        {"bab", false},
        {"aba", false},
        {"abb", false}
    }, DFA_nfa_abstar::init);
}

TEST(CorrectNFAGenerationTest, Test_10_star_or_01_star) {
    end_to_end({
        {"", true},
        {"0", false},
        {"1", false},
        {"10", true},
        {"101010", true},
        {"01", true},
        {"0101", true},
        {"101", false},
        {"010", false},
        {"10101001", false},
        {"01011010", false}
    }, DFA_nfa_10_star_or_01_star::init);
}

TEST(CorrectNFAGenerationTest, Test_MultisymTransitions) {
    end_to_end({
        {"", false},
        {"a", true},
        {" ", true},
        {"-", true},
        {"-\\", false},
        {"aeb", true},
        {"e", false},
        {"ad", false}
    }, DFA_nfa_multisym::init);
}

TEST(CorrectNFAGenerationTest, Test_C_IDs) {
    end_to_end({
        {"", false},
        {"_", true},
        {"x", true},
        {"Y", true},
        {"1", false},
        {"x1", true},
        {"_1", true},
        {"_x01TestId_233", true},
        {"abc%108", false}
    }, DFA_nfa_c_id::init);
}