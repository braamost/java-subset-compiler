#pragma once
#include "scanner.hpp"
#include <filesystem>

// Write PREFIX.tokens, PREFIX.attributes, PREFIX.symbols. Return 1 if lexical
// errors were recovered, 0 otherwise; throw on file/input errors.
int scan_file(const ScannerMachine& machine, const std::filesystem::path& source,
              const std::filesystem::path& prefix);
