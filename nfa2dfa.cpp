#include <iostream>
#include <fstream>
#include <string>
#include <sysexits.h>

#include "nfa_generator.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cout << "Usage: nfa2dfa input [output]" << std::endl;
        return EX_USAGE;
    } 
    std::string ifname(argv[1]);
    std::string ofname;
    if (argc == 3) {
        ofname = argv[2];
    } else {
        ofname = "_autodfa_" + ifname;
    }
    std::ifstream is(ifname);
    std::ofstream os(ofname);

    if (!is) {
        std::cout << "Error: cannot open \"" << ifname << "\"" << std::endl;
        return EX_NOINPUT;
    }
    if (!os) {
        std::cout << "Error: cannot create \"" << ofname << "\"" << std::endl;
        return EX_CANTCREAT;
    }

    Automata::NFAGenerator nfagen(is);
    nfagen.make_dfa_spec(os);

    return 0;
}