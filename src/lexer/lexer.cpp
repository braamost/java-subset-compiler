#include "lexer.hpp"
#include "rules.hpp"

LexerDefinition::LexerDefinition(std::istream& rules_input) {
    RuleReader rule_reader;
    NfaBuilder nfa_builder;
    DfaBuilder dfa_builder;

    rules_ = rule_reader.read(rules_input);
    const Nfa nfa = nfa_builder.build(rules_);
    dfa_ = dfa_builder.minimize(dfa_builder.build(nfa, rules_));
}

Scanner LexerDefinition::scanner(std::istream& source) const {
    return Scanner(dfa_, rules_, source);
}

void LexerDefinition::writeTransitionTable(std::ostream& output) const {
    DfaBuilder{}.write_table(dfa_, output);
}
