# Format
This document describes the format of input files expected by different units.

## DFA-Autogen
Each line has to be of one of the following forms:
- Transition: `[state_from] '[char_on]' > [state_to]`
  - `[char_on]` must be a single printable ASCII character or a literal whitespace
- Terminal state(s) definition: `+ [state_name] ...`
  - Multiple terminal definition lines are possible;
  the resulting set is a union of all encountered definitions
- Start state definition: `! [state_name]`
  - Each start state definition overwrites all previous definitions

and a state name may only contain alphanumeric characters and `_`.
Note that square brackets `[]` are not literal and only denote user text fields.

Furthermore, ignored are:
- Empty lines
- Comment lines starting with `//`

## NFA-Autogen
Follows the same rules, except:
- A transition now has the form: `[state_from] {'[char_on]' / _} > [state_to]...`
  - literal `_` instead of `'[char_on]'` represents an empty symbol (epsilon)
  - multiple target states are allowed in a single transition
  - if the same `state_from` and `char_on` are encountered on another line,
  the set is extended