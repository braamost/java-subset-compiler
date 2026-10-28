#include "automata.hpp"
#include <stdexcept>

Dfa build_dfa(const Nfa&, const std::vector<Rule>&) {
    // TODO: Epsilon closure and subset construction; preserve rule priorities.
    throw std::runtime_error("DFA construction is not implemented yet");
}

Dfa minimize_dfa(const Dfa&) {
    // TODO: Merge equivalent states while preserving accepting rule identities.
    throw std::runtime_error("DFA minimization is not implemented yet");
}

void write_table(const Dfa&, std::ostream&) {
    // TODO: Output start state, transitions, and accepting rule indices.
    throw std::runtime_error("Transition table output is not implemented yet");
}
