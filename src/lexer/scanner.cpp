#include "scanner.hpp"
#include <stdexcept>

Scanner::Scanner(const Dfa& dfa, const std::vector<Rule>& rules, std::istream& input)
    : dfa_(dfa), rules_(rules), input_(input) {}

std::optional<Token> Scanner::next_token() {
    // TODO: Longest match, rollback, whitespace, symbols, and error recovery.
    throw std::runtime_error("Scanning is not implemented yet");
}
