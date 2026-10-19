#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <gtest/gtest.h>

#include "dfa_generator.hpp"
#include "fagen_status_code.hpp"

const std::filesystem::path source_path(__FILE__);
const auto faulty_specs_path(source_path.parent_path()/"faulty_dfa_specs");

TEST(FaultyDFATest, TestNoStartState) {
    std::ifstream is(faulty_specs_path/"no_start_state.dfa");
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.get_status_code(), Automata::FAGenStatusCode::NO_INIT);
    EXPECT_EQ(dfa_generator.deserialize(dummy_os, "name"), Automata::FAGenStatusCode::NO_INIT);
}

TEST(FaultyDFATest, WrongStartStateLine) {
    std::ifstream is(faulty_specs_path/"wrong_start_state_line.dfa");
    std::stringstream dummy_os;
    Automata::DFAGenerator dfa_generator(is);
    EXPECT_EQ(dfa_generator.get_status_code(), Automata::FAGenStatusCode::WRONG_FMT);
    EXPECT_EQ(dfa_generator.deserialize(dummy_os, "name"), Automata::FAGenStatusCode::WRONG_FMT);
}
