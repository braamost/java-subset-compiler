# Build and run the Phase 1 lexer

Requirements: CMake 3.16 or newer and a C++17 compiler.

From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

Run the lexer with the supplied assignment rules and test program:

```sh
./build/lexer src/lexer/test_files/phase1.rules build/phase1.table \
  src/lexer/test_files/phase1_program.java > build/phase1.tokens
```

Compare the output with the expected token classes:

```sh
diff -u src/lexer/test_files/phase1_expected.tokens build/phase1.tokens
```

No diff means the token classes match. The transition table is written to
`build/phase1.table`. Lexical errors are reported to standard error with their
zero-based byte offsets.

The lexer builds the rules and minimized DFA once per process. Pass more than one
source file after the table path to reuse that definition for those files. Parser
code can instead construct a `LexerDefinition` once and request a `Scanner` for
each source stream; call `Scanner::nextToken()` until it returns `std::nullopt`.
