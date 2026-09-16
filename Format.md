# Format
This document describes the format of input files expected by different units.

## DFA-Autogen
Each line has to be of one of the following forms:
- Transition: `[state_from] '[char_on]' > [state_to]`
- Terminal state(s) definition: `+ [state_name] ...`
  - Multiple terminal definition lines are possible;
  the resulting set is a union of all encountered definitions
- Start state definition: `! [state_name]`
  - Each start state definition overwrites all previous definitions

and a state name may contain only alphanumeric characters and `_`.
Note that square brackets `[]` are not literal and only denote user text fields.

Furthermore, ignored are:
- Empty lines
- Comment lines starting with `//`
