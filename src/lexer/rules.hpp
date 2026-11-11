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
class IRuleReader {
public:
    virtual ~IRuleReader() = default;
    virtual std::vector<Rule> read(std::istream& input) = 0;
};

class RuleReader : public IRuleReader {
public:
    std::vector<Rule> read(std::istream& input) override;
};
