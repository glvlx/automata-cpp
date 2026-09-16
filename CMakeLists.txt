cmake_minimum_required(VERSION 3.25)
project(Automata VERSION 0.1 LANGUAGES CXX)

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

add_library(dfa src/dfa.cpp)
target_include_directories(dfa PRIVATE include)

add_library(nfa src/nfa.cpp)
target_include_directories(nfa PRIVATE include)

add_executable(playground main.cpp)
target_include_directories(playground PRIVATE include)
target_link_libraries(playground PRIVATE dfa nfa)
target_compile_options(playground PRIVATE cxx_std_17)