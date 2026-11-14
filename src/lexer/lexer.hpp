#pragma once
#include "automata.hpp"
#include "scanner.hpp"
#include <iosfwd>
#include <vector>

// A language's rules and minimized DFA, built once and reused for source files.
// Keep this object alive while any Scanner returned by scanner() is in use.
class LexerDefinition {
public:
    explicit LexerDefinition(std::istream& rules_input);
    Scanner scanner(std::istream& source) const;
    void writeTransitionTable(std::ostream& output) const;
    const std::vector<Rule>& rules() const { return rules_; }
    const Dfa& dfa() const { return dfa_; }
private:
    std::vector<Rule> rules_;
    Dfa dfa_;
};
