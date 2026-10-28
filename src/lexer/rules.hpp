#pragma once
#include <istream>
#include <string>
#include <vector>

struct Rule {
    std::string name;
    std::string expression;
    int priority; // Lower numbers win equal-length matches.
};

// Read definitions, token expressions, keywords, and punctuation.
// Expand definitions and preserve token priority in the returned rules.
std::vector<Rule> read_rules(std::istream& input);
