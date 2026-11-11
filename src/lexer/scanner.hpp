#pragma once
#include "scanner_machine.hpp"
#include <cstddef>
#include <deque>
#include <iostream>
#include <istream>
#include <optional>
#include <string>
#include <vector>

struct Token {
    std::string type;
    std::string lexeme; // Numeric/operator attribute: preserve original spelling.
    int symbol = -1; // Identifier attribute: stable index into symbols().
    std::size_t line = 1;
    std::size_t column = 1;
};

// Parser-facing interface: nullopt means EOF, not a recoverable lexical error.
class IScanner {
public:
    virtual ~IScanner() = default;
    virtual std::optional<Token> next_token() = 0;
    virtual const std::vector<std::string>& symbols() const = 0;
    virtual std::size_t error_count() const = 0;
};

class Scanner : public IScanner {
public:
    // Machine/input/diagnostics must outlive the scanner. Input is read lazily.
    Scanner(const ScannerMachine& machine, std::istream& input,
            std::ostream& diagnostics = std::cerr);
    std::optional<Token> next_token() override;
    const std::vector<std::string>& symbols() const override;
    std::size_t error_count() const override;
private:
    int read_character();
    void unread(const std::string& text, std::size_t from);
    void advance(char character);
    const ScannerMachine& machine_;
    std::istream& input_;
    std::ostream& diagnostics_;
    std::deque<char> pending_;
    std::vector<std::string> symbols_;
    std::size_t line_ = 1;
    std::size_t column_ = 1;
    std::size_t errors_ = 0;
    bool previous_cr_ = false;
};
