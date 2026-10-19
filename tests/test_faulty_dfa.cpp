#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <gtest/gtest.h>

#include "dfa_generator.hpp"
#include "fagen_status_code.hpp"

const std::filesystem::path source_path(__FILE__);
const auto faulty_specs_path(source_path.parent_path()/"faulty_dfa_specs");

Automata::FAGenStatusCode try_build(std::string dfa_filename, std::string dfa_name) {
    std::ifstream is(faulty_specs_path/dfa_filename);
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.get_status_code() == dfa_generator.deserialize(dummy_os, dfa_name), true);
    return dfa_generator.get_status_code();
}

/******** Incomplete specification ********/
TEST(FaultyDFATest, TestNoStartState) {
    EXPECT_EQ(try_build("no_start_state.dfa", "no_start_state"), Automata::FAGenStatusCode::NO_INIT);
}

/************** Wrong format **************/
TEST(FaultyDFATest, WrongStartStateLine) {
    EXPECT_EQ(try_build("wrong_start_state_line.dfa", "wrong_start_state_line"), Automata::FAGenStatusCode::WRONG_FMT);
}

TEST(FaultyDFATest, EmptyTransitionSymbol) {
    EXPECT_EQ(try_build("empty_transition_symbol.dfa", "empty_transition_symbol"), Automata::FAGenStatusCode::WRONG_FMT);
}

TEST(FaultyDFATest, MissingTransitionSymbol) {
    EXPECT_EQ(try_build("missing_transition_symbol.dfa", "missing_transition_symbol"), Automata::FAGenStatusCode::WRONG_FMT);
}

TEST(FaultyDFATest, MissingSourceStateName) {
    EXPECT_EQ(try_build("missing_source_state_name.dfa", "missing_source_state"), Automata::FAGenStatusCode::WRONG_FMT);
}

TEST(FaultyDFATest, MissingTargetStateName) {
    EXPECT_EQ(try_build("missing_target_state_name.dfa", "missing_target_state"), Automata::FAGenStatusCode::WRONG_FMT);
}
