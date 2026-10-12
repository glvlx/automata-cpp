#include <iostream>
#include <fstream>
#include <filesystem>
#include <sysexits.h>

#include "dfa_generator.hpp"

int main() {
    const std::filesystem::path source_path(__FILE__);
    const auto source_directory(source_path.parent_path());
    const auto correct_dfa_specs(source_directory/"correct_dfa_specs");
    const auto target_directory(source_directory/"generated_dfa");

    std::filesystem::create_directory(target_directory); // can throw

    for (auto const& dfa_spec_fsentry : std::filesystem::directory_iterator(correct_dfa_specs)) {
        std::string dfa_name = dfa_spec_fsentry.path().stem();
        std::ifstream is(dfa_spec_fsentry.path());
        
        auto target_path = target_directory/dfa_name;
        target_path.replace_extension("hpp");
        std::ofstream os(target_path);

        Automata::DFAGenerator dfagen(is);
        dfagen.deserialize(os, dfa_name);
    }

    return 0;
}