#ifndef UTIL_H_
#define UTIL_H_

#ifndef NDEBUG
#define DEBUG_PRINT_ENABLED true
#else
#define DEBUG_PRINT_ENABLED false
#endif

#define TAB "    "

#include <iostream>
#include <string>

/**
 * Prints a debug message to std::cout when building in debug mode
 */
#if DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(MSG) std::cout << "[dbg]: " << MSG << std::endl
#else
#define DEBUG_PRINT(MSG)
#endif

namespace Automata {

/**
 * Checks whether name is a valid C++ identifier
 */
bool is_valid_id(std::string name) {
    if (name.size() == 0 || isdigit(name.front())) {
        return false;
    }
    for (char c : name) {
        if (!(isalnum(c) || c == '_')) {
            return false;
        }
    }
    return true;
}

/**
 * Checks whether name is a valid state name
 * (an arbitrary sequence of ASCII digits, letters and _;
 * may also start with a digit)
 */
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