#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "dfa.hpp"

TEST(DFATest, InitialState) {
    /* depends on DFA::test */
    Automata::DFA dfa_a, dfa_na;

    auto& start_a = dfa_a.get_initial_state(true);
    EXPECT_TRUE(dfa_a.test(""));
    
    EXPECT_EQ(&dfa_a.get_initial_state(false), &start_a);

    auto& start_na = dfa_na.get_initial_state();
    EXPECT_FALSE(dfa_na.test(""));

    EXPECT_EQ(&dfa_na.get_initial_state(true), &start_na);
    EXPECT_TRUE(dfa_na.test(""));
}

TEST(DFATest, NewState) {
    /* depends on DFA::{get_initial_state, add_transition, test} */
    Automata::DFA dfa;

    auto& s0 = dfa.get_initial_state();
    auto& s1 = dfa.new_state(true);
    EXPECT_EQ(&dfa.get_initial_state(), &s0);

    dfa.add_transition(s0, 'a', s1);
    EXPECT_TRUE(dfa.test("a"));

    auto& s2 = dfa.new_state(true, true);
    EXPECT_EQ(&dfa.get_initial_state(), &s2);
    EXPECT_NE(&s2, &s0);
    EXPECT_TRUE(dfa.test(""));
}

TEST(DFATest, AddTransition) {
    /* depends on DFA::{get_initial_state, new_state, test} */
    Automata::DFA dfa;

    auto& s0 = dfa.get_initial_state();
    auto& s1 = dfa.new_state(true);
    EXPECT_FALSE(dfa.test("a"));

    dfa.add_transition(s0, 'a', s1);
    EXPECT_TRUE(dfa.test("a"));
}

TEST(DFATest, Serialize) {
    /* depends on DFA::{get_initial_state, new_state} */
    Automata::DFA dfa;

    /* from DFATest.AddTransition */
    auto& s0 = dfa.get_initial_state();
    auto& s1 = dfa.new_state(true);
    dfa.add_transition(s0, 'a', s1);

    std::stringstream s;
    EXPECT_TRUE(dfa.serialize(s));

    std::string line;
    std::string s0_name, s1_name;
    while (std::getline(s, line)) {
        std::vector<std::string> tokens;

        std::stringstream ss(line);
        std::string tok;
        while (ss >> tok) {
            tokens.push_back(tok);
        }

        switch(tokens[0][0]) {
            case '!': {
                EXPECT_EQ(tokens.size(), 2);
                EXPECT_EQ(tokens[0], "!");
                EXPECT_EQ(tokens[1][0], 'q');

                if (s0_name.empty()) {
                    s0_name = tokens[1];
                } else {
                    EXPECT_EQ(tokens[1], s0_name);
                }
                break;
            }
            case '+': {
                EXPECT_EQ(tokens.size(), 2);
                EXPECT_EQ(tokens[0], "+");
                EXPECT_EQ(tokens[1][0], 'q');

                if (s1_name.empty()) {
                    s1_name = tokens[1];
                } else {
                    EXPECT_EQ(tokens[1], s1_name);
                }
                break;
            }
            case 'q': {
                EXPECT_EQ(tokens.size(), 4);
                EXPECT_EQ(tokens[1], "'a'");
                EXPECT_EQ(tokens[2], ">");
                EXPECT_EQ(tokens[3][0], 'q');

                if (s0_name.empty()) {
                    s0_name = tokens[0];
                } else {
                    EXPECT_EQ(tokens[0], s0_name);
                }
                if (s1_name.empty()) {
                    s1_name = tokens[3];
                } else {
                    EXPECT_EQ(tokens[3], s1_name);
                }

                break;
            }
            default:
                std::cout << "Error in DFATest.Serialize: invalid specification line format" << std::endl; 
                FAIL();
        }
    }
}
