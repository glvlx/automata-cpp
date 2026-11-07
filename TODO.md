## Global:
- [ ] make headers and libraries accessible outside the project
- [ ] (?) provide common base for parsing

## util:
- [ ] (?) rework DEBUG_PRINT
  - could be nice to be able to disable and enable messages at runtime

## NFAGenerator:
- [x] fix epsilon path calculation in parse_input()
- [x] bug: initial state not marked as accepting despite an epsilon-transition to an accepting state
- [x] extend error detection like in DFAGenerator

## CLI:
- [ ] add support for stdin mode (instead of file in)