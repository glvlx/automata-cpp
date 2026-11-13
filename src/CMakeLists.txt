add_library(dfa ${CMAKE_CURRENT_SOURCE_DIR}/dfa.cpp)
target_link_libraries(dfa 
                        PUBLIC headers 
                        PRIVATE "$<BUILD_LOCAL_INTERFACE:_automata_compile_options>")

add_library(dfa_generator ${CMAKE_CURRENT_SOURCE_DIR}/dfa_generator.cpp)
target_link_libraries(dfa_generator 
                        PUBLIC headers dfa
                        PRIVATE "$<BUILD_LOCAL_INTERFACE:_automata_compile_options>")

add_library(nfa ${CMAKE_CURRENT_SOURCE_DIR}/nfa.cpp)
target_link_libraries(nfa 
                        PUBLIC headers
                        PRIVATE "$<BUILD_LOCAL_INTERFACE:_automata_compile_options>")

add_library(nfa_generator ${CMAKE_CURRENT_SOURCE_DIR}/nfa_generator.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/compound_state.cpp)
target_link_libraries(nfa_generator 
                        PUBLIC headers
                        PRIVATE "$<BUILD_LOCAL_INTERFACE:_automata_compile_options>")