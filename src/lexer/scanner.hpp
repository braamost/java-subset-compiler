#pragma once
#include "automata.hpp"
#include <cstddef>
#include <deque>
#include <istream>
#include <optional>
#include <string>
#include <vector>

struct ScannedToken {
    std::string type;
    std::string lexeme;
    int symbol = -1; // Identifier index in Scanner::symbols().
    std::size_t offset = 0;
};

// Pull interface for the parser. nullopt means end of input.
class Scanner {
public:
    Scanner(const Dfa& dfa, const std::vector<Rule>& rules, std::istream& input);
    std::optional<ScannedToken> nextToken();
    const std::vector<std::string>& symbols() const { return symbols_; }
    const std::vector<std::size_t>& errorOffsets() const { return error_offsets_; }
private:
    int readCharacter();
    void unread(const std::string& text, std::size_t from);
    const Dfa& dfa_;
    const std::vector<Rule>& rules_;
    std::istream& input_;
    std::deque<char> pending_;
    std::vector<std::string> symbols_;
    std::vector<std::size_t> error_offsets_;
    std::size_t offset_ = 0;
};
