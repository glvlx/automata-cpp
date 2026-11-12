#include <iostream>
#include <fstream>
#include <string>
#include <sysexits.h>
#include <argp.h>

#include "compound_state.hpp"
#include "nfa_generator.hpp"

struct Args {
    std::string ifname_;
    std::string ofname_;
};

argp_option argp_options[] = {
    {
        /*.name     =*/ "out",
        /*.key      =*/ 'o',
        /*.arg      =*/ "OUTPUT",
        /*.flags    =*/ 0, // OPTION_ARG_OPTIONAL means providing a value is optional
        /*.doc      =*/ "Name of the output file. If none is specified, \"_autodfa_[INPUT]\" is used.",
        /*.group    =*/ 0
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
            pargs->ifname_ = arg;
            break;
        case 'o':
            if (arg == nullptr) {
                pargs->ofname_ = "";
            } else {
                pargs->ofname_ = arg;
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
    /*.args_doc =*/ "INPUT",
    /*.doc      =*/ "Constructs a DFA specification from the provided NFA specification.\v"
        "Specification syntax:\n"
        "\n"
        "Each line has to be in one of the following forms:\n"
        "\n"
        "(1) Transition: STATE_FROM 'CHAR_ON' > STATE_TO [STATE_TO...]\n"
        "    or          STATE_FROM _ > STATE_TO [STATES_TO...]\n"
        "    or          STATE_FROM [CHAR_SET] > STATE_TO [STATE_TO...]\n"
        "where _ represents an empty symbol (epsilon),\n"
        "and CHAR_SET is a combination of printable ASCII characters including whitespace, "
        "character ranges separated with `-`, and characters escaped with `\\`\n"
        "(Example: CHAR_SET a-zA-Z_\\- represents all latin letters, underscore and a literal dash)\n"
        "\n"
        "(2) Accepting state(s) definition: + STATE_NAME [STATE_NAMES...]\n"
        "Multiple accepting state definition lines are possible, the resulting set"
        "is a union of all encountered definitions\n"
        "\n"
        "(3) Start state definition: ! STATE_NAME\n"
        "Each start state definition overwrites all previous definitions.\n"
        "\n"
        "Note that a start state definition must be present in each valid specification.",
    /* .children    =*/ nullptr,
    /* .help_filter =*/ nullptr,
    /* .argp_domain =*/ nullptr
};

int main(int argc, char* argv[]) {
    Args parsed_args;
    argp_parse(&argp_instance, argc, argv, 0, nullptr, &parsed_args);

    if (parsed_args.ofname_ == "") {
        parsed_args.ofname_ = "_autodfa_" + parsed_args.ifname_ + ".dfa";
    }

    std::ifstream is(parsed_args.ifname_);
    if (!is) {
        std::cout << "Error: cannot open \"" << parsed_args.ifname_ << "\"" << std::endl;
        return EX_NOINPUT;
    }

    std::ofstream os(parsed_args.ofname_);
    if (!os) {
        std::cout << "Error: cannot create \"" << parsed_args.ofname_ << "\"" << std::endl;
        return EX_CANTCREAT;
    }

    Automata::NFAGenerator nfagen(is);
    nfagen.write_dfa_spec(os);

    return 0;
}