#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "dfa_generator.hpp"
#include "fagen_status_flag.hpp"

bool generate_dfa_headers(
    const std::filesystem::path spec_directory,
    const std::filesystem::path target_directory
) {
    std::filesystem::create_directory(target_directory); // can throw

    bool task_successful = true;

    for (auto const& dfa_spec_fsentry : std::filesystem::directory_iterator(spec_directory)) {
        std::string dfa_name = dfa_spec_fsentry.path().stem();
        std::ifstream is(dfa_spec_fsentry.path());
        
        auto target_path = target_directory/dfa_name;
        target_path.replace_extension("hpp");
        std::ofstream os(target_path);

        Automata::DFAGenerator dfagen(is);
        bool dfa_successful = dfagen.write(os, dfa_name) == Automata::FAGenStatusFlag::OK;
        if (!dfa_successful) {
            task_successful = false;
            std::cerr << "Error: cannot generate header for DFA \"" << dfa_name << "\"" << std::endl;
        }
    }

    return task_successful;
}

int main() {
    const std::filesystem::path source_path(__FILE__);
    const auto source_directory(source_path.parent_path());

    bool correct_dfa_specs_successful = generate_dfa_headers(
        source_directory/"correct_dfa_specs",
        source_directory/"generated_dfa"
    );
    bool correct_nfa_specs_successful = generate_dfa_headers(
        source_directory/"generated_dfa_specs",
        source_directory/"generated_nfa"
    );

    return !(
        correct_dfa_specs_successful &&
        correct_nfa_specs_successful
    );
}