cmake_minimum_required(VERSION 3.26...4.3)
project(Automata 
    VERSION 0.3
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# For common installation and export paths
include(GNUInstallDirs)

# Proxy for use of all headers and libraries in subdirectories (e.g. tests)
add_library(headers INTERFACE)
target_include_directories(headers INTERFACE 
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_library(debug_options INTERFACE)
# Extensive warnings for gcc and clang
if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang)$")
    target_compile_options(debug_options INTERFACE
                        -Wall -Wextra -Wpedantic -Wold-style-cast -Wcast-align
                        -Wunused -Wnon-virtual-dtor -Wconversion -Wsign-conversion
                        -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough)

    if(CMAKE_CXX_COMPILER_ID MATCHES "^GNU$")
        target_compile_options(debug_options INTERFACE
                        -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches
                        -Wlogical-op -Wnull-dereference -Wuseless-cast)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^Clang$")
        # target_compile_options(debug_options INTERFACE -Wlifetime)
    endif()
endif()

# Libraries
add_subdirectory(src)

# Executables
add_executable(playground playground.cpp)
target_link_libraries(playground PRIVATE debug_options headers dfa nfa dfa_generator)

add_executable(nfa2dfa nfa2dfa.cpp)
target_link_libraries(nfa2dfa PRIVATE debug_options headers dfa nfa_generator)

add_executable(dfagen dfagen.cpp)
target_link_libraries(dfagen PRIVATE debug_options headers dfa_generator)

# Aliases (for export)
add_library(Automata::headers ALIAS headers)

add_library(exported_libs INTERFACE)
target_link_libraries(exported_libs INTERFACE 
                        dfa 
                        dfa_generator 
                        nfa_generator
)
add_library(Automata::exported_libs ALIAS exported_libs)

add_library(Automata::dfa ALIAS dfa)
add_library(Automata::dfa_generator ALIAS dfa_generator)
add_library(Automata::nfa_generator ALIAS nfa_generator)

# Installation (https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html)
install(TARGETS exported_libs headers dfa dfa_generator nfa_generator 
    EXPORT AutomataTargets
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(TARGETS nfa2dfa dfagen
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(FILES 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/dfa.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/dfa_generator.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/nfa_generator.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/fagen_status_flag.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/fa_token.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/compound_state.hpp"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(EXPORT AutomataTargets
    FILE AutomataTargets.cmake
    NAMESPACE Automata::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Automata"
)

# Package configuration
include(CMakePackageConfigHelpers)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/AutomataConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Automata"
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/AutomataConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/AutomataConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/AutomataConfigVersion.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Automata"
)

# Exports
export(EXPORT AutomataTargets
       FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/AutomataTargets.cmake"
       NAMESPACE Automata::
)

# Tests
include(CTest)

if (BUILD_TESTING # false if include(CTest) hasn't been called in the root file (i.e. this file has been included)
    AND CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME) 
    # GTest configuration from https://google.github.io/googletest/quickstart-cmake.html#set-up-a-project
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/releases/download/v1.18.0/googletest-1.18.0.tar.gz
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_subdirectory(tests)
endif()
