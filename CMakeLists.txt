cmake_minimum_required(VERSION 3.16)
project(java_subset_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(compiler
    src/main.cpp
    src/pipeline.cpp
    src/lexer/rules.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/scanner.cpp
    src/lexer/dfa_adapter.cpp
    src/lexer/emitter.cpp
    src/lexer/output.cpp
)
target_include_directories(compiler PRIVATE src/lexer src)

add_executable(test_nfa
    test_nfa.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/rules.cpp
    src/lexer/dfa_adapter.cpp
    src/lexer/emitter.cpp
    src/lexer/output.cpp
)
target_include_directories(test_nfa PRIVATE src/lexer src)
