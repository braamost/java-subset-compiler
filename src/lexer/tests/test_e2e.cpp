#include "src/lexer/lexer.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

int main() {
    const std::string rule_text = R"RULES(
letter = a-z | A-Z
digit = 0 - 9
id: letter (letter|digit)*
digits = digit+
{boolean int float}
num: digit+ | digit+ . digits ( \L | E digits)
relop: \=\= | !\= | > | >\= | < | <\=
assign: =
{ if else while }
[; , \( \) { }]
addop: \+ | \-
mulop: \* | /
)RULES";
    const std::string source =
        "int sum , count , pass , mnt; while (pass !=10)\n"
        "{\n"
        "pass = pass + 1 ;\n"
        "}\n";
    const std::vector<std::string> expected = {
        "int", "id", ",", "id", ",", "id", ",", "id", ";", "while",
        "(", "id", "relop", "num", ")", "{", "id", "assign", "id",
        "addop", "num", ";", "}"
    };

    try {
        std::istringstream rules_input(rule_text);
        LexerDefinition language(rules_input);
        std::istringstream source_input(source);
        Scanner scanner = language.scanner(source_input);

        std::vector<ScannedToken> tokens;
        while (auto token = scanner.nextToken()) tokens.push_back(*token);

        if (!scanner.errorOffsets().empty())
            throw std::runtime_error("sample program produced lexical errors");
        if (tokens.size() != expected.size())
            throw std::runtime_error("wrong token count: got " + std::to_string(tokens.size()) +
                                     ", expected " + std::to_string(expected.size()));
        for (std::size_t i = 0; i < expected.size(); ++i) {
            if (tokens[i].type != expected[i])
                throw std::runtime_error("token " + std::to_string(i) + ": got '" +
                                         tokens[i].type + "', expected '" + expected[i] + "'");
        }
        if (scanner.symbols() != std::vector<std::string>{"sum", "count", "pass", "mnt"})
            throw std::runtime_error("identifier symbol table did not match the sample");

        std::cout << "PASS: rules -> NFA -> DFA -> minimized DFA -> sample tokens ("
                  << tokens.size() << " tokens)\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
