# Phase 1: Four-student work division

Each student owns one substantial part of the pipeline and writes the corresponding
section of the required report. The split aims for comparable effort; help each other
with integration rather than treating these as isolated assignments.

| Student | Responsibility | Main files |
| --- | --- | --- |
| 1 | Rule-file parsing and definition expansion | `rules.hpp`, `rules.cpp` |
| 2 | Regular-expression parsing and NFA construction | `nfa.cpp`, NFA structures in `automata.hpp` |
| 3 | DFA construction and minimization | `dfa.cpp`, DFA structures in `automata.hpp` |
| 4 | Scanner, symbol table, and executable integration | `scanner.hpp`, `scanner.cpp`, `main.cpp` |

## Student 1: Rules and definitions

- Read regular definitions (`=`), token rules (`:`), keyword groups, and punctuation.
- Expand named definitions without corrupting literal characters or escaped symbols.
- Handle forward references or document a declaration-order restriction; detect
  undefined names and cycles.
- Assign rule priorities and report malformed rule-file entries.
- Work with Student 2 on how expanded expressions represent ranges, escapes, and epsilon.

Output: a `vector<Rule>` containing token names, expanded expressions, and priorities.

## Student 2: Regular expressions to NFA

- Parse precedence, concatenation, grouping, alternation, ranges, `*`, `+`, and `\L`.
- Use Thompson construction for each token expression.
- Combine the NFAs using a new start state with epsilon edges.
- Mark accepting states with rule indices.
- Reject token expressions that match the empty string to prevent endless scanning.

Output: one combined `Nfa`, consumed by Student 3.

## Student 3: NFA to minimized DFA

- Implement epsilon closure and subset construction.
- Select the highest-priority rule when a DFA state contains multiple accepting states.
- Minimize the DFA without merging states with different accepting rule identities.
- Implement `write_table()` for the reduced DFA, including the start state and accepting actions.

Output: a minimized `Dfa` and its transition table.

## Student 4: Scanner and integration

- Implement `next_token()` with longest-prefix matching and rollback.
- Handle whitespace, EOF, identifier attributes, symbol-table insertion, and error recovery.
- Print token classes and attributes in the required output formats.
- Connect input/output files and the generation pipeline in `main.cpp`.
- Produce the required generated-lexer artifact, coordinating its table format with Student 3.
  Confirm whether a reusable runtime plus emitted tables meets the requirement or
  whether standalone C++ source must be emitted.

Output: the runnable generator/scanner workflow, using the other students' modules.

## Working together

Agree on the headers before implementing. Students 2 and 3 can begin with small
hand-built rule/NFA inputs; Student 4 can begin with a hand-built DFA.

Check each part manually as it is developed. Everyone participates in the final
run using the handout's rules and program, checks token priorities and recovery,
and contributes their own algorithms/data structures to the report. Divide any
optional Lex/Flex bonus work separately.

Two handout ambiguities need a shared decision: keyword groups follow `id` in the
sample despite keywords being reserved, and the prose appears to allow `12E3`
while the sample numeric expression does not. Confirm the intended behavior and
record the assumptions in the submission report.
