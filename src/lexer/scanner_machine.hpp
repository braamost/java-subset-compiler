#pragma once
#include <optional>
#include <string>

// Scanner-facing abstraction. The scanner needs neither NFA construction nor
// minimization details. A missing transition returns -1; a nonaccepting state
// returns nullopt. Token priorities have already been resolved by the DFA builder.
class ScannerMachine {
public:
    virtual ~ScannerMachine() = default;
    virtual int start_state() const = 0;
    virtual int next_state(int state, char character) const = 0;
    virtual std::optional<std::string> token_type(int state) const = 0;
};
