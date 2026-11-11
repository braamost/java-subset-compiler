#pragma once
#include "lexer/automata.hpp"
#include "lexer/emitter.hpp"
#include <filesystem>

// Inject interfaces so the CLI does not depend on algorithm implementations.
void generate_lexer(IRuleReader& reader, INfaBuilder& nfa_builder,
                    IDfaBuilder& dfa_builder, ILexerEmitter& emitter,
                    const std::filesystem::path& rules_file,
                    const std::filesystem::path& prefix);
