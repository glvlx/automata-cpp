#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "nfa_generator.hpp"
#include "fagen_status_flag.hpp"

int main() {
    const std::filesystem::path source_path(__FILE__);
    const auto source_directory(source_path.parent_path());
    const auto correct_dfa_specs(source_directory/"correct_dfa_specs");
    const auto target_directory(source_directory/"generated_dfa_specs");

    std::filesystem::create_directory(target_directory); // can throw

    bool task_successful = true;

    for (auto const& dfa_spec_fsentry : std::filesystem::directory_iterator(correct_dfa_specs)) {
        std::string dfa_name = dfa_spec_fsentry.path().stem();
        std::ifstream is(dfa_spec_fsentry.path());
        
        auto target_path = target_directory/dfa_name;
        target_path.replace_extension("dfa");
        target_path.replace_filename("nfa_" + target_path.filename().string());
        std::ofstream os(target_path);
        if (!os) {
            std::cerr << "Error: cannot create " << target_path << std::endl;
        }

        Automata::NFAGenerator nfagen(is);
        bool nfa_successful = nfagen.write_dfa_spec(os) == Automata::FAGenStatusFlag::OK;
        if (!nfa_successful) {
            task_successful = false;
            std::cerr << "Error: cannot generate DFA specification for DFA \"" << dfa_name << "\" (as NFA)" << std::endl;
        }
    }

    return !task_successful;
}