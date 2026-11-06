/**
 * Generate DFA specifications from correct NFA specifications
 */
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "nfa_generator.hpp"
#include "fagen_status_flag.hpp"

bool generate_dfa_specs(
    const std::filesystem::path nfa_spec_directory,
    const std::filesystem::path target_directory,
    std::string target_name_prefix
) {
    std::filesystem::create_directory(target_directory); // can throw

    for (auto const& nfa_spec_fsentry : std::filesystem::directory_iterator(nfa_spec_directory)) {
        std::string nfa_name = nfa_spec_fsentry.path().stem();
        std::ifstream is(nfa_spec_fsentry.path());
        
        auto target_path = target_directory/nfa_name;
        target_path.replace_extension("dfa");
        target_path.replace_filename(target_name_prefix + target_path.filename().string());
        std::ofstream os(target_path);
        if (!os) {
            std::cerr << "Error: cannot create " << target_path << std::endl;
        }

        Automata::NFAGenerator nfagen(is);
        bool nfa_successful = nfagen.write_dfa_spec(os) == Automata::FAGenStatusFlag::OK;
        if (!nfa_successful) {
            return false;
            std::cerr << "Error: cannot generate DFA specification for DFA \"" << nfa_name << "\" (as NFA)" << std::endl;
        }
    }

    return true;
}

int main() {
    const std::filesystem::path source_path(__FILE__);
    const auto source_directory(source_path.parent_path());

    bool correct_dfa_specs_successful = generate_dfa_specs(
        source_directory/"correct_dfa_specs",
        source_directory/"generated_dfa_specs",
        "fnfa_"
    );

    bool correct_nfa_specs_successful = generate_dfa_specs(
        source_directory/"correct_nfa_specs",
        source_directory/"generated_dfa_specs",
        "nfa_"
    );

    return !(
        correct_dfa_specs_successful &&
        correct_nfa_specs_successful
    );
}