/**
 * Provides some useful functions that are shared between different modules.
 * Single-header for convenience of use.
 */
#ifndef UTIL_H_
#define UTIL_H_

#ifndef NDEBUG
#define AUTOMATA_DEBUG_PRINT_ENABLED true
#define AUTOMATA_VDEBUG_PRINT_ENABLED false
#else
#define AUTOMATA_DEBUG_PRINT_ENABLED false
#define AUTOMATA_VDEBUG_PRINT_ENABLED false
#endif


#define TAB "    "

#include <iostream>
#include <string>

namespace Automata {

/**
 * Debug message printing
 */
struct dummy_stream {
    template <typename T>
    dummy_stream& operator<<(const T&) {
        return *this;
    }

    dummy_stream& flush() {
        return *this;
    }
};

#if AUTOMATA_DEBUG_PRINT_ENABLED
#define DEBUG_STREAM (std::cout)
#else
#define DEBUG_STREAM Automata::dummy_stream()
#endif

#if AUTOMATA_VDEBUG_PRINT_ENABLED // verbose debug
#define VDEBUG_STREAM (std::cout)
#else
#define VDEBUG_STREAM Automata::dummy_stream()
#endif

/**
 * Checks whether the argument is a valid C++ identifier
 */
inline bool is_valid_id(std::string name) noexcept {
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
 * Checks whether the argument is a valid state name
 * (an arbitrary sequence of ASCII digits, letters and _;
 * may also start with a digit)
 */
inline bool is_valid_state_name(std::string state_name) noexcept {
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