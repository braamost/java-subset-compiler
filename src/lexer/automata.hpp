#pragma once
#include "rules.hpp"
#include <map>
#include <ostream>
#include <string>
#include <vector>

struct NfaState {
    std::map<char, std::vector<int>> transitions;
    std::vector<int> epsilon;
    int rule = -1; // Index into the rules vector; -1 means nonaccepting.
};

struct Nfa {
    std::vector<NfaState> states;
    int start = 0;
};

struct DfaState {
    std::map<char, int> transitions; // Missing transition means failure.
    int rule = -1;
};

struct Dfa {
    std::vector<DfaState> states;
    int start = 0;
};

Nfa build_nfa(const std::vector<Rule>& rules);
Dfa build_dfa(const Nfa& nfa, const std::vector<Rule>& rules);
Dfa minimize_dfa(const Dfa& dfa);
void write_table(const Dfa& dfa, std::ostream& output);
