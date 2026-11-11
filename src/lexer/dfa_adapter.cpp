#include "dfa_adapter.hpp"
#include <stdexcept>

DfaAdapter::DfaAdapter(const Dfa& dfa, const std::vector<Rule>& rules)
    : dfa_(dfa), rules_(rules) {
    throw std::runtime_error("Part 4: DFA adapter is not implemented yet");
}
int DfaAdapter::start_state() const { return dfa_.start; }
int DfaAdapter::next_state(int, char) const {
    throw std::runtime_error("Part 4: DFA adapter is not implemented yet");
}
std::optional<std::string> DfaAdapter::token_type(int) const {
    throw std::runtime_error("Part 4: DFA adapter is not implemented yet");
}
