#pragma once
#include "rules.hpp"
#include <map>
#include <ostream>
#include <vector>

// State IDs are vector indices. All targets and accepting rule IDs must be valid.
struct NfaState {
    std::map<char, std::vector<int>> transitions;
    std::vector<int> epsilon;
    int rule = -1;
};
struct Nfa {
    std::vector<NfaState> states;
    int start = 0;
};
struct DfaState {
    std::map<char, int> transitions; // Missing edge means failure.
    int rule = -1; // Index into the unchanged rules vector; -1 is nonaccepting.
};
struct Dfa {
    std::vector<DfaState> states;
    int start = 0;
};

class INfaBuilder {
public:
    virtual ~INfaBuilder() = default;
    virtual Nfa build(const std::vector<Rule>& rules) = 0;
};
class NfaBuilder : public INfaBuilder {
public:
    Nfa build(const std::vector<Rule>& rules) override;
};

class IDfaBuilder {
public:
    virtual ~IDfaBuilder() = default;
    // Resolve accepting priorities during subset construction. Minimization must
    // preserve winning rule IDs. The start state must not accept an empty token.
    virtual Dfa build(const Nfa& nfa, const std::vector<Rule>& rules) = 0;
    virtual Dfa minimize(const Dfa& dfa) = 0;
    virtual void write_table(const Dfa& dfa, std::ostream& output) = 0;
};
class DfaBuilder : public IDfaBuilder {
public:
    Dfa build(const Nfa& nfa, const std::vector<Rule>& rules) override;
    Dfa minimize(const Dfa& dfa) override;
    void write_table(const Dfa& dfa, std::ostream& output) override;
};

void dump_nfa(const Nfa& nfa, std::ostream& out);
