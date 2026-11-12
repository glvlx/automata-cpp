# Format
This document describes the format of input files expected by different units.

## DFAGenerator
Each line has to be of one of the following forms:
- Transition: `[state_from] '[char_on]' > [state_to]`
  2. `[state_from] [[char_set]] > [state_to]` (literal `[]` around `[char_set]`)
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

## NFAGenerator
Follows the same rules, except:
- A transition now has one of the following forms: 
  1. `[state_from] '[char_on]' > [state_to]...`
  2. `[state_from] _ > [state_to]...`
    
      In this form, literal `_` represents an empty symbol (epsilon)
  3. `[state_from] [[symbol_set]] > [state_to]...`
      
      This form allows to transition on a set of symbols. 

      `[symbol_set]` must be a combination of printable ASCII characters including whitespace,
      character ranges separated with `-`, and characters escaped with `\`

      Example: `a-zA-Z_\-` represents all latin letters, underscore and literal `-`

  Furthermore,
  - multiple target states are allowed in a single transition
  - if the same `state_from` and `char_on` are encountered on another line,
  the set is extended