cmake_minimum_required(VERSION 3.25)
project(Automata VERSION 0.1 LANGUAGES CXX)

# GTest configuration from https://google.github.io/googletest/quickstart-cmake.html#set-up-a-project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Proxy for use of all headers and libraries in subdirectories (e.g. tests)
add_library(internal_headers INTERFACE)
target_include_directories(internal_headers INTERFACE include)

# Extensive warnings for gcc and clang
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    target_compile_options(internal_headers INTERFACE
                        -Wall -Wextra -Wpedantic -Wold-style-cast -Wcast-align
                        -Wunused -Wnon-virtual-dtor -Wconversion -Wsign-conversion
                        -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough)

    if(CMAKE_CXX_COMPILER_ID MATCHES "^GNU$")
        target_compile_options(internal_headers INTERFACE
                        -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
                        -Wlogical-op -Wnull-dereference -Wuseless-cast)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^Clang$")
        # target_compile_options(internal_headers INTERFACE -Wlifetime)
    endif()
endif()

# Libraries
add_subdirectory(src)

# Executables
add_executable(playground playground.cpp)
target_link_libraries(playground PRIVATE internal_headers dfa nfa dfa_generator)

add_executable(nfa2dfa nfa2dfa.cpp)
target_link_libraries(nfa2dfa PRIVATE internal_headers dfa compound_state)

add_executable(dfagen dfagen.cpp)
target_link_libraries(dfagen PRIVATE internal_headers dfa_generator)

# Tests
include(CTest)

if (BUILD_TESTING) # false if include(CTest) hasn't been called in the root file (i.e. this file has been included)
    add_subdirectory(tests)
endif()
