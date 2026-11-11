#pragma once
#include "automata.hpp"

class ILexerEmitter {
public:
    virtual ~ILexerEmitter() = default;
    // Emit a self-contained C++17 lexer using the shared scanner runtime.
    virtual void emit(const Dfa& dfa, const std::vector<Rule>& rules,
                      std::ostream& output) = 0;
};
class LexerEmitter : public ILexerEmitter {
public:
    void emit(const Dfa& dfa, const std::vector<Rule>& rules,
              std::ostream& output) override;
};
