add_library(dfa ${CMAKE_CURRENT_SOURCE_DIR}/dfa.cpp)
target_link_libraries(dfa PRIVATE internal_headers)

add_library(dfa_generator ${CMAKE_CURRENT_SOURCE_DIR}/dfa_generator.cpp)
target_link_libraries(dfa_generator PRIVATE internal_headers dfa)

add_library(nfa ${CMAKE_CURRENT_SOURCE_DIR}/nfa.cpp)
target_link_libraries(nfa PRIVATE internal_headers)

add_library(compound_state ${CMAKE_CURRENT_SOURCE_DIR}/compound_state.cpp)
target_link_libraries(compound_state PRIVATE internal_headers)