#pragma once
#include "automata.hpp"
#include "scanner_machine.hpp"

// Part 4 adapter. Exposes DFA data; does not construct or minimize automata.
// DFA/rules must remain alive and unchanged while the adapter is used.
class DfaAdapter : public ScannerMachine {
public:
    DfaAdapter(const Dfa& dfa, const std::vector<Rule>& rules);
    int start_state() const override;
    int next_state(int state, char character) const override;
    std::optional<std::string> token_type(int state) const override;
private:
    const Dfa& dfa_;
    const std::vector<Rule>& rules_;
    std::vector<bool> productive_;
};
