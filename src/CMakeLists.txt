add_library(dfa ${CMAKE_CURRENT_SOURCE_DIR}/dfa.cpp)
target_link_libraries(dfa PRIVATE internal_headers)

add_library(nfa ${CMAKE_CURRENT_SOURCE_DIR}/nfa.cpp)
target_link_libraries(nfa PRIVATE internal_headers)