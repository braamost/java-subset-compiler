cmake_minimum_required(VERSION 3.16)
project(java_subset_compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(compiler
    src/main.cpp
    src/lexer/rules.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/scanner.cpp
)
target_include_directories(compiler PRIVATE src/lexer src)

add_executable(test_nfa
    src/lexer/tests/test_nfa.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/rules.cpp
)
target_include_directories(test_nfa PRIVATE src/lexer src ${PROJECT_SOURCE_DIR})

add_executable(test_scanner
    src/lexer/tests/test_scanner.cpp
    src/lexer/scanner.cpp
)
target_include_directories(test_scanner PRIVATE src/lexer ${PROJECT_SOURCE_DIR})

add_executable(test_e2e
    src/lexer/tests/test_e2e.cpp
    src/lexer/lexer.cpp
    src/lexer/rules.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/scanner.cpp
)
target_include_directories(test_e2e PRIVATE src/lexer src ${PROJECT_SOURCE_DIR})

add_executable(lexer
    src/lexer/main.cpp
    src/lexer/lexer.cpp
    src/lexer/rules.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/scanner.cpp
)
target_include_directories(lexer PRIVATE src/lexer)

add_executable(test_dfa
    src/lexer/tests/test_dfa.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/rules.cpp
)
target_include_directories(test_dfa PRIVATE src/lexer src ${PROJECT_SOURCE_DIR})
target_include_directories(test_nfa PRIVATE src/lexer src)
enable_language(C)
find_package(FLEX REQUIRED)

FLEX_TARGET(BonusLexer
    ${CMAKE_CURRENT_SOURCE_DIR}/bonus_flex/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
)

add_executable(flex_scanner
    ${FLEX_BonusLexer_OUTPUTS}
    bonus_flex/flex_main.c
)

add_executable(test_flex_parity
    src/lexer/tests/test_flex_parity.cpp
    ${FLEX_BonusLexer_OUTPUTS}
    src/lexer/lexer.cpp
    src/lexer/rules.cpp
    src/lexer/nfa.cpp
    src/lexer/dfa.cpp
    src/lexer/scanner.cpp
)
target_include_directories(test_flex_parity PRIVATE src/lexer src ${PROJECT_SOURCE_DIR})
