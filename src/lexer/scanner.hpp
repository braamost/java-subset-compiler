#pragma once
#include "automata.hpp"
#include <istream>
#include <optional>
#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string lexeme;
    int symbol = -1; // Identifier's symbol-table index, when applicable.
};

class Scanner {
public:
    // The DFA, rules, and input must outlive the scanner.
    Scanner(const Dfa& dfa, const std::vector<Rule>& rules, std::istream& input);
    std::optional<Token> next_token(); // No value means end of input.
    std::vector<std::string> symbols;

private:
    const Dfa& dfa_;
    const std::vector<Rule>& rules_;
    std::istream& input_;
};
