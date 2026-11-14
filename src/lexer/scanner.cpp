#include "scanner.hpp"
#include <algorithm>
#include <stdexcept>
#include <utility>

namespace {
bool isWhitespace(int byte) {
    return byte == ' ' || byte == '\t' || byte == '\r' || byte == '\n' ||
           byte == '\f' || byte == '\v';
}
}

Scanner::Scanner(const Dfa& dfa, const std::vector<Rule>& rules, std::istream& input) : dfa_(dfa), rules_(rules), input_(input) {

    if (dfa_.states.empty() || dfa_.start < 0 ||
        static_cast<std::size_t>(dfa_.start) >= dfa_.states.size())
        throw std::invalid_argument("DFA has an invalid start state");

    if (dfa_.states[dfa_.start].rule != -1)
        throw std::invalid_argument("DFA accepts an empty token");

    for (const auto& state : dfa_.states) {
        if (state.rule < -1 || (state.rule >= 0 &&
            static_cast<std::size_t>(state.rule) >= rules_.size()))
            throw std::invalid_argument("DFA has an invalid accepting rule");
        for (const auto& edge : state.transitions)
            if (edge.second < 0 || static_cast<std::size_t>(edge.second) >= dfa_.states.size())
                throw std::invalid_argument("DFA has an invalid transition target");
    }
}

int Scanner::readCharacter() {
    if (!pending_.empty()) {
        const int byte = static_cast<unsigned char>(pending_.front());
        pending_.pop_front();
        return byte;
    }
    const int byte = input_.get();
    if (byte == std::char_traits<char>::eof() && (input_.bad() || !input_.eof()))
        throw std::runtime_error("Failed to read source input");
    return byte;
}

void Scanner::unread(const std::string& text, std::size_t from) {
    for (std::size_t index = text.size() - 1; index >= from; --index)
        pending_.push_front(text[index]);
}

std::optional<ScannedToken> Scanner::nextToken() {
    const int eof = std::char_traits<char>::eof();
    for (;;) {
        int first = readCharacter();
        while (first != eof && isWhitespace(first)) {
            ++offset_;
            first = readCharacter();
        }
        if (first == eof) return std::nullopt;

        int state = dfa_.start;
        std::string examined;
        std::size_t accepted_length = 0;
        int accepted_rule = -1;
        int byte = first;
        while (byte != eof) {
            examined.push_back(static_cast<char>(byte));
            const auto& transitions = dfa_.states[state].transitions;
            const auto edge = transitions.find(static_cast<char>(byte));
            if (edge == transitions.end()) break;
            state = edge->second;
            const int rule = dfa_.states[state].rule;
            if (rule >= 0) {
                accepted_rule = rule;
                accepted_length = examined.size();
            }
            byte = readCharacter();
        }

        if (accepted_rule < 0) {
            error_offsets_.push_back(offset_++); // Discard one byte to guarantee progress.
            unread(examined, 1);
            continue;
        }

        unread(examined, accepted_length);
        ScannedToken token{rules_[accepted_rule].name,
                           examined.substr(0, accepted_length), -1, offset_};
        if (token.type == "id") {
            const auto found = std::find(symbols_.begin(), symbols_.end(), token.lexeme);
            if (found == symbols_.end()) {
                token.symbol = static_cast<int>(symbols_.size());
                symbols_.push_back(token.lexeme);
            } else {
                token.symbol = static_cast<int>(found - symbols_.begin());
            }
        }
        offset_ += accepted_length;
        return token;
    }
}
