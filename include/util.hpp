#ifndef UTIL_H_
#define UTIL_H_

#ifndef NDEBUG
#define DEBUG_PRINT true
#else
#define DEBUG_PRINT false
#endif

#define TAB "    "

#include <iostream>
#include <string>

void debug_print(std::string message, std::ostream& os=std::cout) {
#if DEBUG_PRINT
    os << "[dbg]: " << message << std::endl;
#endif
}

namespace Automata {

/**
 * Checks if the provided name is a valid C++ identifier
 */
bool is_valid_id(std::string dfa_name) {
    if (dfa_name.size() == 0 || isdigit(dfa_name.front())) {
        return false;
    }
    for (char c : dfa_name) {
        if (!(isalnum(c) || c == '_')) {
            return false;
        }
    }
    return true;
}

bool is_valid_state_name(std::string state_name) {
    if (state_name.size() == 0) {
        return false;
    }
    for (char c : state_name) {
        if (!(isalnum(c) || c == '_')) {
            return false;
        }
    }
    return true;
}

} // namespace Automata

#endif