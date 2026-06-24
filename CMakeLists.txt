cmake_minimum_required(VERSION 3.25)
project(Automata VERSION 0.1 LANGUAGES CXX)

add_library(dfa src/dfa.cpp)
target_include_directories(dfa PRIVATE include)

add_executable(playground main.cpp)
target_include_directories(playground PRIVATE include)
target_link_libraries(playground PRIVATE dfa)