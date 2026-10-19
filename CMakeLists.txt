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

# Extensive warnings for gcc and clang
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    add_compile_options(-Wall -Wextra -Wpedantic -Wold-style-cast -Wcast-align
                        -Wunused -Wnon-virtual-dtor -Wconversion -Wsign-conversion
                        -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough)

    if(CMAKE_CXX_COMPILER_ID MATCHES "^GNU$")
        add_compile_options(-Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
                            -Wlogical-op -Wnull-dereference -Wuseless-cast)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^Clang$")
        # add_compile_options(-Wlifetime)
    endif()
endif()

# Libraries
add_library(dfa src/dfa.cpp)
target_include_directories(dfa PRIVATE include)

add_library(nfa src/nfa.cpp)
target_include_directories(nfa PRIVATE include)

# Executables
add_executable(playground main.cpp)
target_include_directories(playground PRIVATE include)
target_link_libraries(playground PRIVATE dfa nfa)

add_executable(dfa-autogen autogen-driver.cpp)
target_include_directories(dfa-autogen PRIVATE include)

add_executable(nfa2dfa nfa2dfa.cpp)
target_include_directories(nfa2dfa PRIVATE include)

# Proxy for use of all headers and libraries in subdirectories (e.g. tests)
add_library(internal_headers INTERFACE)
target_include_directories(internal_headers INTERFACE include)

# Tests
include(CTest)

if (BUILD_TESTING) # false if include(CTest) hasn't been called in the root file (i.e. this file has been included)
    add_subdirectory(tests)
endif()
