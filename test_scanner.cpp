#include "src/lexer/scanner.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

static void check(const char* name, bool ok) {
    std::cout << (ok ? "PASS: " : "FAIL: ") << name << '\n';
    if (!ok) throw std::runtime_error(name);
}

int main() {
    // Hand-built DFA for identifiers [a-z]+ and '=' / '=='.
    Dfa dfa;
    dfa.start = 0;
    dfa.states.resize(5);
    dfa.states[0].transitions['a'] = 1;
    dfa.states[0].transitions['b'] = 1;
    dfa.states[0].transitions['='] = 2;
    dfa.states[1].transitions['a'] = 1;
    dfa.states[1].transitions['b'] = 1;
    dfa.states[1].rule = 0;
    dfa.states[2].rule = 1;
    dfa.states[2].transitions['='] = 3;
    dfa.states[3].rule = 2;
    dfa.states[3].transitions['='] = 4;
    dfa.states[4].rule = 2;

    const std::vector<Rule> rules = {
        {"id", "(a|b)+", 0}, {"assign", "=", 0}, {"relop", "==|===", 0}
    };
    std::istringstream input("ab === = ab?b");
    Scanner scanner(dfa, rules, input);
    std::vector<ScannedToken> tokens;
    while (auto token = scanner.nextToken()) tokens.push_back(*token);
    check("longest identifier", tokens.size() == 5 && tokens[0].lexeme == "ab");
    check("longest operator", tokens[1].type == "relop" && tokens[1].lexeme == "===");
    check("single operator", tokens[2].type == "assign" && tokens[2].lexeme == "=");
    check("identifier IDs reused", tokens[0].symbol == tokens[3].symbol);
    check("invalid input recovers", scanner.errorOffsets().size() == 1 && tokens.back().lexeme == "b");
    check("symbol table", scanner.symbols().size() == 2 && scanner.symbols()[0] == "ab");
    check("token offsets", tokens[0].offset == 0 && tokens[1].offset == 3);
    check("EOF stays EOF", !scanner.nextToken() && !scanner.nextToken());

    std::istringstream trailing_input("==");
    Scanner trailing_scanner(dfa, rules, trailing_input);
    const auto trailing = trailing_scanner.nextToken();
    check("accept at EOF", trailing && trailing->type == "relop" && trailing->lexeme == "==" &&
          !trailing_scanner.nextToken());

    std::istringstream invalid_input("??ab");
    Scanner invalid_scanner(dfa, rules, invalid_input);
    const auto after_errors = invalid_scanner.nextToken();
    check("multiple bad bytes recover", after_errors && after_errors->lexeme == "ab" &&
          invalid_scanner.errorOffsets() == std::vector<std::size_t>{0, 1});
    return 0;
}
