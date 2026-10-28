#include <iostream>
#include <fstream>
#include <string>
#include <sysexits.h>
#include <argp.h>

#include "dfa_generator.hpp"
#include "fagen_status_flag.hpp"
#include "util.hpp"

struct Args {
    bool ifname_specified_ = false;
    bool ofname_specified_ = false;
    std::string dfaname_;
    std::string ifname_;
    std::string ofname_;
};

argp_option argp_options[] = {
    {   
        nullptr, 0, nullptr, 0, "Positional arguments:", 1
    },
    {
        /*.name     =*/ "DFANAME",
        /*.key      =*/ 0,
        /*.arg      =*/ nullptr,
        /*.flags    =*/ OPTION_DOC,
        /*.doc      =*/ "Suffix of the namespace where the init function is to be placed",
        /*.group    =*/ 1
    },
    {   
        nullptr, 0, nullptr, 0, "Options:", 2
    },
    {
        /*.name     =*/ "input",
        /*.key      =*/ 'i',
        /*.arg      =*/ "INPUT",
        /*.flags    =*/ 0, // OPTION_ARG_OPTIONAL means providing a value is optional
        /*.doc      =*/ "Name of the input file. If none is specified, \"[DFANAME]\" is used.",
        /*.group    =*/ 2
    },
    {
        /*.name     =*/ "output",
        /*.key      =*/ 'o',
        /*.arg      =*/ "OUTPUT",
        /*.flags    =*/ 0,
        /*.doc      =*/ "Name of the output file. If none is specified, \"_dfa_[INPUT].hpp\" is used.",
        /*.group    =*/ 2
    },
    { } // null terminator
};

error_t arg_parser(int key, char* arg, struct argp_state* state) {
    Args* pargs = static_cast<Args*>(state->input);
    switch (key) {
        case ARGP_KEY_ARG: // positional args
            if (state->arg_num >= 1) {
                argp_usage(state);
            }
            pargs->dfaname_ = arg;
            break;
        case 'i':
            if (arg) {
                pargs->ifname_ = arg;
                pargs->ifname_specified_ = true;
            }
            break;
        case 'o':
            if (arg) {
                pargs->ofname_ = arg;
                pargs->ofname_specified_ = true;
            }
            break;
        case ARGP_KEY_END: // check if positional provided
            if (state->arg_num < 1) {
                argp_usage(state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

argp argp_instance {
    /*.options  =*/ argp_options,
    /*.parser   =*/ arg_parser,
    /*.args_doc =*/ "DFANAME",
    /*.doc      =*/ "Generates a C++ header from the provided DFA specification.\n"
        "\n"
        "The generated header provides \"init(Automata::DFA&)\" inside namespace \"DFA_[DFANAME]\" "
        "that can be used to initialize a DFA according to the specification.\n"
        "\n"
        "The generated header itself does not include Automata::DFA."
        "\v"
        "Syntax:\n"
        "Each line has to be in one of the following forms:\n"
        "- Transition:\n"
        "  STATE_FROM 'CHAR_ON' > STATE_TO\n"
        "- Terminal state(s) definition:\n"
        "  + STATE_NAME [STATE_NAMES...]\n"
        "  - multiple terminal definition lines are possible, the resulting set "
        "is a union of all encountered definitions\n"
        "- Initial state definition:\n"
        "  ! STATE_NAME\n"
        "  - each start state definition overwrites all previous definitions.\n"
        "\n",
    /* .children    =*/ nullptr,
    /* .help_filter =*/ nullptr,
    /* .argp_domain =*/ nullptr
};

int main(int argc, char* argv[]) {
    Args parsed_args;
    argp_parse(&argp_instance, argc, argv, 0, nullptr, &parsed_args);

    if (!parsed_args.ofname_specified_) {
        parsed_args.ofname_ = "_dfa_" + parsed_args.ifname_ + ".hpp";
    }
    if (!parsed_args.ifname_specified_) {
        parsed_args.ifname_ = parsed_args.dfaname_;
    }

    std::string dfaname(parsed_args.dfaname_);
    if (!Automata::is_valid_id(dfaname)) {
        std::cout << "Error: DFA name \"" << dfaname << "\" is not a valid C++ identifier." << std::endl;
        return EX_CONFIG;
    }

    std::ifstream is(parsed_args.ifname_);
    if (!is) {
        std::cout << "Error: cannot open \"" << parsed_args.ifname_ << "\"." << std::endl;
        if (!parsed_args.ifname_specified_) {
            std::cout << "If specification file name is different than DFA name, pass it via -i." << std::endl;
        }
        return EX_NOINPUT;
    }

    std::ofstream os(parsed_args.ofname_);
    if (!os) {
        std::cout << "Error: cannot create \"" << parsed_args.ofname_ << "\"." << std::endl;
        return EX_CANTCREAT;
    }

    Automata::DFAGenerator dfagen(is);
    unsigned int flags = dfagen.write(os, dfaname);
    if (flags == Automata::FAGenStatusFlag::OK) {
        std::cout << "DFA \"" << dfaname << "\" successfully generated to \"" 
                  << parsed_args.ofname_ << "\"." << std::endl;
    } else {
        std::cout << "Generation of DFA \"" << dfaname << "\" failed." << std::endl;
    }

    return 0;
}