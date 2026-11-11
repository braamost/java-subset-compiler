#include "emitter.hpp"
#include <stdexcept>

void LexerEmitter::emit(const Dfa&, const std::vector<Rule>&, std::ostream&) {
    throw std::runtime_error("Part 4: lexer emission is not implemented yet");
}
