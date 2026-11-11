#include "scanner.hpp"
#include <stdexcept>

Scanner::Scanner(const ScannerMachine& machine, std::istream& input, std::ostream& diagnostics)
    : machine_(machine), input_(input), diagnostics_(diagnostics) {}
std::optional<Token> Scanner::next_token() {
    throw std::runtime_error("Part 4: scanner is not implemented yet");
}
const std::vector<std::string>& Scanner::symbols() const { return symbols_; }
std::size_t Scanner::error_count() const { return errors_; }
