#ifndef UTIL_H_
#define UTIL_H_

#ifndef DEBUG_PRINT
#define DEBUG_PRINT true
#endif

#include <iostream>
#include <string>

void debug_print(std::string message) {
#if DEBUG_PRINT
    std::cout << "[dbg]: " << message << std::endl;
#endif
}

#endif