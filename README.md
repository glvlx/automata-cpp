- [Description](#description)
- [Building the project](#building-the-project)
- [Usage](#usage)
  - [Libraries](#libraries)
    - [List of exported targets](#list-of-exported-targets)
  - [Apps](#apps)

## Description

This project provides:

- An implementation of [Deterministic Finite Automata](https://en.wikipedia.org/wiki/Deterministic_finite_automaton)
  - A way of serializing DFAs to specifications
- DFAGenerator class that generates a C++ header from a DFA specification. 
  This header provides a function that initializes a new DFA with a set of corresponding states and transitions.
- NFAGenerator class that generates a DFA specification from an NFA specification.

For specification file format, see [Format.md](Format.md)

## Building the project

CMake is used as this project's build system.
For example, if you wish to build the project into `build` directory:
```
$ cmake -B build
$ cmake --build build
```

Afterwards, installation can optionally be performed via:
```
$ cmake --install build [--prefix ...]
```

## Usage

### Libraries

Libraries are exported as CMake targets. Therefore, they can be included in one of the following ways:

- **FetchContent**: add the following to your CMakeLists.txt
  ```
  include(FetchContent)

  FetchContent_Declare(
    Automata
    GIT_REPOSITORY https://github.com/glvlx/automata-cpp.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(Automata)
  ```

- **add_subdirectory**: the repository can be cloned and added as a submodule, and its directory included in CMakeLists.txt as a subdirectory

- **find_package** (if the library has been installed):
  ```
  find_package(Automata REQUIRED)
  ```
  If a custom installation prefix has been selected, it must be supplied via `CMAKE_PREFIX_PATH`

#### List of exported targets
- Automata::headers
- Automata::exported_libs (includes all libraries below)
- Automata::dfa
- Automata::dfa_generator
- Automata::nfa_generator

### Apps

Additionally, several CLI applications are provided (`nfa2dfa`, `dfagen`). 
Information on their usage can be obtained by calling with `--help`.