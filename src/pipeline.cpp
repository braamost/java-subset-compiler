#include "pipeline.hpp"
#include <stdexcept>

void generate_lexer(IRuleReader&, INfaBuilder&, IDfaBuilder&, ILexerEmitter&,
                    const std::filesystem::path&, const std::filesystem::path&) {
    throw std::runtime_error("Part 4: pipeline integration is not implemented yet");
}
