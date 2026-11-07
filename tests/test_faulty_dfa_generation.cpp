#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "fagen_status_flag.hpp"
#include "dfa_generator.hpp"
#include "nfa_generator.hpp"

const std::filesystem::path source_path(__FILE__);
const auto faulty_specs_path(source_path.parent_path()/"faulty_dfa_specs");

unsigned int try_build_dfa(std::string dfa_filename, std::string dfa_name) {
    std::ifstream is(faulty_specs_path/dfa_filename);
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.get_status_flags() == dfa_generator.write(dummy_os, dfa_name), true);
    return dfa_generator.get_status_flags();
}

unsigned int try_build_nfa(std::string nfa_filename) {
    std::ifstream is(faulty_specs_path/nfa_filename);
    std::stringstream dummy_os;
    Automata::NFAGenerator nfa_generator(is);
    EXPECT_EQ(nfa_generator.get_status_flags() == nfa_generator.write_dfa_spec(dummy_os), true);
    return nfa_generator.get_status_flags();
}

/******** Incomplete specification ********/
TEST(FaultyDFAGenerationTest, EmptyFile) {
    EXPECT_NE(try_build_dfa("empty_file.dfa", "empty_file")
                & Automata::FAGenStatusFlag::NO_INIT, 0);
}
TEST(FaultyNFAGenerationTest, EmptyFile) {
    EXPECT_NE(try_build_nfa("empty_file.dfa")
                & Automata::FAGenStatusFlag::NO_INIT, 0);
}

TEST(FaultyDFAGenerationTest, NoStartState) {
    EXPECT_NE(try_build_dfa("no_start_state.dfa", "no_start_state") 
                & Automata::FAGenStatusFlag::NO_INIT, 0);
}
TEST(FaultyNFAGenerationTest, NoStartState) {
    EXPECT_NE(try_build_nfa("no_start_state.dfa") 
                & Automata::FAGenStatusFlag::NO_INIT, 0);
}

/************** Wrong format **************/
TEST(FaultyDFAGenerationTest, WrongStartStateLine) {
    EXPECT_NE(try_build_dfa("wrong_start_state_line.dfa", "wrong_start_state_line") 
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, WrongStartStateLine) {
    EXPECT_NE(try_build_nfa("wrong_start_state_line.dfa") 
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, WrongAcceptingStateLine) {
    EXPECT_NE(try_build_dfa("wrong_accepting_state_line.dfa", "wrong_accepting_state_line")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, WrongAcceptingStateLine) {
    EXPECT_NE(try_build_nfa("wrong_accepting_state_line.dfa")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingTransitionSymbol) {
    EXPECT_NE(try_build_dfa("missing_transition_symbol.dfa", "missing_transition_symbol")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, MissingTransitionSymbol) {
    EXPECT_NE(try_build_nfa("missing_transition_symbol.dfa")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingSourceStateName) {
    EXPECT_NE(try_build_dfa("missing_source_state_name.dfa", "missing_source_state")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, MissingSourceStateName) {
    EXPECT_NE(try_build_nfa("missing_source_state_name.dfa")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingTargetStateName) {
    EXPECT_NE(try_build_dfa("missing_target_state_name.dfa", "missing_target_state")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, MissingTargetStateName) {
    EXPECT_NE(try_build_nfa("missing_target_state_name.dfa")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, InvalidToken) {
    EXPECT_NE(try_build_dfa("invalid_token.dfa", "invalid_token")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}
TEST(FaultyNFAGenerationTest, InvalidToken) {
    EXPECT_NE(try_build_nfa("invalid_token.dfa")
                & Automata::FAGenStatusFlag::WRONG_FMT, 0);
}

/*********** Invalid state name ***********/
TEST(FaultyDFAGenerationTest, InvalidInitialStateName) {
    EXPECT_NE(try_build_dfa("invalid_initial_state_name.dfa", "invalid_state_name")
                & Automata::FAGenStatusFlag::WRONG_STATE_NAME, 0);
}
TEST(FaultyNFAGenerationTest, InvalidInitialStateName) {
    EXPECT_NE(try_build_nfa("invalid_initial_state_name.dfa")
                & Automata::FAGenStatusFlag::WRONG_STATE_NAME, 0);
}

TEST(FaultyDFAGenerationTest, InvalidTransitionStateName) {
    EXPECT_NE(try_build_dfa("invalid_transition_state_name.dfa", "invalid_state_name")
                & Automata::FAGenStatusFlag::WRONG_STATE_NAME, 0);
}
TEST(FaultyNFAGenerationTest, InvalidTransitionStateName) {
    EXPECT_NE(try_build_nfa("invalid_transition_state_name.dfa")
                & Automata::FAGenStatusFlag::WRONG_STATE_NAME, 0);
}

/************* Invalid symbol *************/
TEST(FaultyDFAGenerationTest, InvalidTransitionSymbol) {
    EXPECT_NE(try_build_dfa("invalid_transition_symbol.dfa", "invalid_transition_symbol")
                & Automata::FAGenStatusFlag::WRONG_SYMBOL, 0);
}
TEST(FaultyNFAGenerationTest, InvalidTransitionSymbol) {
    EXPECT_NE(try_build_nfa("invalid_transition_symbol.dfa")
                & Automata::FAGenStatusFlag::WRONG_SYMBOL, 0);
}

TEST(FaultyDFAGenerationTest, EmptyTransitionSymbol) {
    EXPECT_NE(try_build_dfa("empty_transition_symbol.dfa", "empty_transition_symbol")
                & Automata::FAGenStatusFlag::WRONG_SYMBOL, 0);
}
TEST(FaultyNFAGenerationTest, EmptyTransitionSymbol) {
    EXPECT_NE(try_build_nfa("empty_transition_symbol.dfa")
                & Automata::FAGenStatusFlag::WRONG_SYMBOL, 0);
}

/************ Invalid DFA name ************/
TEST(FaultyDFAGenerationTest, InvalidDFAName) {
    std::ifstream is(faulty_specs_path/"empty_file.dfa");
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.write(dummy_os, "wrong+name"), 
                Automata::FAGenStatusFlag::WRONG_NAME);
}