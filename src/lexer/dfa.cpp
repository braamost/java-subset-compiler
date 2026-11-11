#include "automata.hpp"
#include <stdexcept>

Dfa DfaBuilder::build(const Nfa&, const std::vector<Rule>&) {
    throw std::runtime_error("Part 3: DFA construction is not implemented yet");
}
Dfa DfaBuilder::minimize(const Dfa&) {
    throw std::runtime_error("Part 3: DFA minimization is not implemented yet");
}
void DfaBuilder::write_table(const Dfa&, std::ostream&) {
    throw std::runtime_error("Part 3: transition table output is not implemented yet");
}
