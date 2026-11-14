#pragma once
#include <istream>
#include <string>
#include <vector>

struct Rule {
    std::string name;
    std::string expression;
    int priority; // Smaller wins; original index breaks a remaining tie.
};

// Return only token rules, with named definitions expanded inside parentheses.
// Keep vector indices stable through all stages. Preserve escaped literals and
// \L; support the handout's ranges/operators. Keywords must be reserved: resolve
// their priority against id here, not in the scanner. Do not emit empty tokens.
// Use priority 0 for keywords and 1 for other rules; ties follow vector order.
// Definitions may refer forward. Bare identifier-like words of two or more
// characters are definition references; unknown words are errors. Single
// characters are literals unless they name a definition; range endpoints stay
// literal. Write literal words as escaped or space-separated characters, or in
// keyword groups. Group entries are whitespace-separated literals. Malformed
// entries, undefined references and cycles throw runtime_error with line numbers.
// Regex syntax and empty-match validation belong to the NFA builder.
class IRuleReader {
public:
    virtual ~IRuleReader() = default;
    virtual std::vector<Rule> read(std::istream& input) = 0;
};

class RuleReader : public IRuleReader {
public:
    std::vector<Rule> read(std::istream& input) override;
};
