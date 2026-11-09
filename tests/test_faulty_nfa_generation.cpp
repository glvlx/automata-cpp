#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "fagen_status_flag.hpp"
#include "dfa_generator.hpp"
#include "nfa_generator.hpp"

const std::filesystem::path source_path(__FILE__);
const auto faulty_specs_path(source_path.parent_path()/"faulty_nfa_specs");

unsigned int try_build_nfa(std::string nfa_filename) {
    std::ifstream is(faulty_specs_path/nfa_filename);
    std::stringstream dummy_os;
    Automata::NFAGenerator nfa_generator(is);
    EXPECT_EQ(nfa_generator.get_status_flags() == nfa_generator.write_dfa_spec(dummy_os), true);
    return nfa_generator.get_status_flags();
}

TEST(FaultyNFAGenerationTest, InvalidTargetStateList) {
    EXPECT_NE(try_build_nfa("invalid_targets.nfa")
            & Automata::FAGenStatusFlag::WRONG_STATE_NAME, 0);
}