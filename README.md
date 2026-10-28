# Java Subset Compiler

A C++17 university project for the Programming Languages and Compiler course at
Alexandria University. The project develops a compiler for a subset of Java across
three phases, leading to JVM bytecode generation.

## Phase 1: Lexical Analyzer Generator

The first phase reads lexical rules, constructs an NFA, converts it to a DFA,
minimizes the DFA, and outputs its transition table and a lexical analyzer.
The scanner will recognize the longest matching token, resolve rule priorities,
store identifiers in a symbol table, and recover from invalid input.

The repository currently contains starter structures and function declarations.
The algorithms are not implemented yet.

## Build and run

Requires CMake 3.16 or newer and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
./build/compiler
```

The executable currently prints a starter message.

## Structure

```text
src/
  main.cpp           Program entry point
  lexer/             Phase 1 rules, automata, and scanner
  parser/            Reserved for a later phase
  codegen/           Reserved for a later phase
docs/
  phase1-work.md     Phase 1 division of work
```

See the [Phase 1 work division](docs/phase1-work.md) for the four-student plan.
