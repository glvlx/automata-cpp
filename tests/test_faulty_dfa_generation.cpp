#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <gtest/gtest.h>

#include "dfa_generator.hpp"
#include "fagen_status_code.hpp"

const std::filesystem::path source_path(__FILE__);
const auto faulty_specs_path(source_path.parent_path()/"faulty_dfa_specs");

unsigned int try_build(std::string dfa_filename, std::string dfa_name) {
    std::ifstream is(faulty_specs_path/dfa_filename);
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.get_status_codes() == dfa_generator.deserialize(dummy_os, dfa_name), true);
    return dfa_generator.get_status_codes();
}

/******** Incomplete specification ********/
TEST(FaultyDFAGenerationTest, EmptyFile) {
    EXPECT_NE(try_build("empty_file.dfa", "empty_file")
                & Automata::FAGenStatusCode::NO_INIT, 0);
}

TEST(FaultyDFAGenerationTest, NoStartState) {
    EXPECT_NE(try_build("no_start_state.dfa", "no_start_state") 
                & Automata::FAGenStatusCode::NO_INIT, 0);
}

/************** Wrong format **************/
TEST(FaultyDFAGenerationTest, WrongStartStateLine) {
    EXPECT_NE(try_build("wrong_start_state_line.dfa", "wrong_start_state_line") 
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, WrongAcceptingStateLine) {
    EXPECT_NE(try_build("wrong_accepting_state_line.dfa", "wrong_accepting_state_line")
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingTransitionSymbol) {
    EXPECT_NE(try_build("missing_transition_symbol.dfa", "missing_transition_symbol")
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingSourceStateName) {
    EXPECT_NE(try_build("missing_source_state_name.dfa", "missing_source_state")
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, MissingTargetStateName) {
    EXPECT_NE(try_build("missing_target_state_name.dfa", "missing_target_state")
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

TEST(FaultyDFAGenerationTest, InvalidToken) {
    EXPECT_NE(try_build("invalid_token.dfa", "invalid_token")
                & Automata::FAGenStatusCode::WRONG_FMT, 0);
}

/*********** Invalid state name ***********/
TEST(FaultyDFAGenerationTest, InvalidInitialStateName) {
    EXPECT_NE(try_build("invalid_initial_state_name.dfa", "invalid_state_name")
                & Automata::FAGenStatusCode::WRONG_STATE_NAME, 0);
}

TEST(FaultyDFAGenerationTest, InvalidTransitionStateName) {
    EXPECT_NE(try_build("invalid_transition_state_name.dfa", "invalid_state_name")
                & Automata::FAGenStatusCode::WRONG_STATE_NAME, 0);
}

/************* Invalid symbol *************/
TEST(FaultyDFAGenerationTest, InvalidTransitionSymbol) {
    EXPECT_NE(try_build("invalid_transition_symbol.dfa", "invalid_transition_symbol")
                & Automata::FAGenStatusCode::WRONG_SYMBOL, 0);
}

TEST(FaultyDFAGenerationTest, EmptyTransitionSymbol) {
    EXPECT_NE(try_build("empty_transition_symbol.dfa", "empty_transition_symbol")
                & Automata::FAGenStatusCode::WRONG_SYMBOL, 0);
}
