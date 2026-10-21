add_library(test_headers INTERFACE)
target_link_libraries(test_headers INTERFACE internal_headers GTest::gtest_main)

add_executable(test_faulty_dfa test_faulty_dfa.cpp)
target_link_libraries(test_faulty_dfa PRIVATE test_headers)

# Builds the generator of correct DFA
add_executable(build_correct_dfa build_correct_dfa.cpp)
target_link_libraries(build_correct_dfa PRIVATE test_headers)
target_compile_definitions(build_correct_dfa PRIVATE NDEBUG)

set(GENERATED_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated_dfa)

# List of correct dfa specs so that they can be tracked
file(GLOB_RECURSE CORRECT_DFA_SPECS 
    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/correct_dfa_specs/*.dfa")

set(CORRECT_DFA_SPEC_STAMP "${CMAKE_CURRENT_BINARY_DIR}/correct_dfa_specs.stamp")

# Command tied to an output stamp (empty file generated on every run of the command)
# The command itself is only run on dependency (here ${CORRECT_DFA_SPECS}) update
add_custom_command(
    OUTPUT "${CORRECT_DFA_SPEC_STAMP}"
    COMMAND build_correct_dfa
    COMMAND ${CMAKE_COMMAND} -E touch "${CORRECT_DFA_SPEC_STAMP}"
    DEPENDS ${CORRECT_DFA_SPECS}
    COMMENT "Running build_correct_dfa..."
    VERBATIM
)
# Target that "interfaces" the stamp file, is meant to be depended on to invoke the command above
add_custom_target(generate_correct_dfa DEPENDS "${CORRECT_DFA_SPEC_STAMP}")

add_executable(test_correct_dfa test_correct_dfa.cpp)
add_dependencies(test_correct_dfa build_correct_dfa generate_correct_dfa)
target_link_libraries(test_correct_dfa PRIVATE test_headers)
target_include_directories(test_correct_dfa PRIVATE ${GENERATED_HEADER_DIR})

# Needs to be added as a separate test to setup the fixture (and use as a prerequisite)
add_test(
    NAME correct_dfa_setup
    COMMAND build_correct_dfa
)
set_tests_properties(correct_dfa_setup PROPERTIES
    FIXTURES_SETUP correct_dfa_env
)

include(GoogleTest)
gtest_discover_tests(test_correct_dfa
    PROPERTIES
        FIXTURES_REQUIRED correct_dfa_env
)
gtest_discover_tests(test_faulty_dfa)