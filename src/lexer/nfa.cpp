#include "automata.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <set>
#include <ostream>

enum class TokenKind { Literal, Epsilon, LParen, RParen, Pipe, Star, Plus, Range, Dash, EndOfInput };

struct Token {
    TokenKind kind;
    char lo = 0;
    char hi = 0;
};

static std::vector<Token> tokenize(const std::string& expr, const std::string& rule_name) {
    std::vector<Token> tokens;
    size_t i = 0;
    while (i < expr.size()) {
        char c = expr[i];
        if (c == ' ' || c == '\t') { ++i; continue; }
        if (c == '\\') {
            if (i + 1 >= expr.size()) throw std::runtime_error("rule '" + rule_name + "': trailing backslash");
            char next = expr[i + 1];
            if (next == 'L') {
                tokens.push_back({TokenKind::Epsilon, 0, 0});
            } else {
                tokens.push_back({TokenKind::Literal, next, next});
            }
            i += 2;
        } else if (c == '(') {
            tokens.push_back({TokenKind::LParen, 0, 0});
            ++i;
        } else if (c == ')') {
            tokens.push_back({TokenKind::RParen, 0, 0});
            ++i;
        } else if (c == '|') {
            tokens.push_back({TokenKind::Pipe, 0, 0});
            ++i;
        } else if (c == '*') {
            tokens.push_back({TokenKind::Star, 0, 0});
            ++i;
        } else if (c == '+') {
            tokens.push_back({TokenKind::Plus, 0, 0});
            ++i;
        } else if (c == '-') {
            tokens.push_back({TokenKind::Dash, 0, 0});
            ++i;
        } else {
            tokens.push_back({TokenKind::Literal, c, c});
            ++i;
        }
    }
    tokens.push_back({TokenKind::EndOfInput, 0, 0});
    return tokens;
}

enum class NodeKind { Literal, Epsilon, Range, Concat, Alt, Star, Plus };

struct AstNode {
    NodeKind kind;
    char lo = 0;
    char hi = 0;
    std::unique_ptr<AstNode> left;
    std::unique_ptr<AstNode> right;
};

static std::unique_ptr<AstNode> make_literal(char c) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Literal;
    n->lo = n->hi = c;
    return n;
}
static std::unique_ptr<AstNode> make_range(char lo, char hi) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Range;
    n->lo = lo;
    n->hi = hi;
    return n;
}
static std::unique_ptr<AstNode> make_eps() {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Epsilon;
    return n;
}
static std::unique_ptr<AstNode> make_concat(std::unique_ptr<AstNode> l, std::unique_ptr<AstNode> r) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Concat;
    n->left = std::move(l);
    n->right = std::move(r);
    return n;
}
static std::unique_ptr<AstNode> make_alt(std::unique_ptr<AstNode> l, std::unique_ptr<AstNode> r) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Alt;
    n->left = std::move(l);
    n->right = std::move(r);
    return n;
}
static std::unique_ptr<AstNode> make_star(std::unique_ptr<AstNode> child) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Star;
    n->left = std::move(child);
    return n;
}
static std::unique_ptr<AstNode> make_plus(std::unique_ptr<AstNode> child) {
    auto n = std::make_unique<AstNode>();
    n->kind = NodeKind::Plus;
    n->left = std::move(child);
    return n;
}

struct Parser {
    const std::vector<Token>& toks;
    size_t pos = 0;
    const std::string& rule_name;

    Token& cur() { return const_cast<Token&>(toks[pos]); }
    Token& peek(size_t offset = 0) { return const_cast<Token&>(toks[pos + offset]); }
    bool at_end() { return toks[pos].kind == TokenKind::EndOfInput; }

    std::unique_ptr<AstNode> parseAlt();
    std::unique_ptr<AstNode> parseConcat();
    std::unique_ptr<AstNode> parsePostfix();
    std::unique_ptr<AstNode> parseAtom();
};

std::unique_ptr<AstNode> Parser::parseAtom() {
    if (cur().kind == TokenKind::LParen) {
        ++pos;
        auto inner = parseAlt();
        if (cur().kind != TokenKind::RParen)
            throw std::runtime_error("rule '" + rule_name + "': unmatched parenthesis");
        ++pos;
        return inner;
    }
    if (cur().kind == TokenKind::Epsilon) {
        ++pos;
        return make_eps();
    }
    if (cur().kind == TokenKind::Literal) {
        char lo = cur().lo;
        ++pos;
        if (pos + 1 < toks.size() &&
            toks[pos].kind == TokenKind::Dash &&
            toks[pos + 1].kind == TokenKind::Literal) {
            char hi = toks[pos + 1].lo;
            pos += 2;
            if (lo > hi)
                throw std::runtime_error("rule '" + rule_name + "': invalid range '" +
                    lo + "'-'" + hi + "'");
            return make_range(lo, hi);
        }
        return make_literal(lo);
    }
    if (cur().kind == TokenKind::RParen)
        throw std::runtime_error("rule '" + rule_name + "': unmatched parenthesis");
    if (cur().kind == TokenKind::Star || cur().kind == TokenKind::Plus)
        throw std::runtime_error("rule '" + rule_name + "': dangling operator");
    if (cur().kind == TokenKind::Pipe)
        throw std::runtime_error("rule '" + rule_name + "': empty alternative");
    throw std::runtime_error("rule '" + rule_name + "': unexpected token");
}

std::unique_ptr<AstNode> Parser::parsePostfix() {
    auto node = parseAtom();
    while (cur().kind == TokenKind::Star || cur().kind == TokenKind::Plus) {
        if (cur().kind == TokenKind::Star) {
            ++pos;
            node = make_star(std::move(node));
        } else {
            ++pos;
            node = make_plus(std::move(node));
        }
    }
    return node;
}

std::unique_ptr<AstNode> Parser::parseConcat() {
    auto left = parsePostfix();
    while (!at_end() &&
           cur().kind != TokenKind::RParen &&
           cur().kind != TokenKind::Pipe) {
        auto right = parsePostfix();
        left = make_concat(std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<AstNode> Parser::parseAlt() {
    auto left = parseConcat();
    while (cur().kind == TokenKind::Pipe) {
        ++pos;
        if (cur().kind == TokenKind::Pipe || cur().kind == TokenKind::RParen || at_end())
            throw std::runtime_error("rule '" + rule_name + "': empty alternative");
        auto right = parseConcat();
        left = make_alt(std::move(left), std::move(right));
    }
    return left;
}

struct Fragment {
    int start;
    int accept;
};

static int new_state(Nfa& nfa) {
    nfa.states.push_back(NfaState{});
    return static_cast<int>(nfa.states.size()) - 1;
}

static Fragment build_fragment(const AstNode* node, Nfa& nfa) {
    switch (node->kind) {
        case NodeKind::Literal: {
            int s = new_state(nfa);
            int e = new_state(nfa);
            nfa.states[s].transitions[node->lo].push_back(e);
            return {s, e};
        }
        case NodeKind::Epsilon: {
            int s = new_state(nfa);
            int e = new_state(nfa);
            nfa.states[s].epsilon.push_back(e);
            return {s, e};
        }
        case NodeKind::Range: {
            int s = new_state(nfa);
            int e = new_state(nfa);
            for (char c = node->lo; ; ++c) {
                nfa.states[s].transitions[c].push_back(e);
                if (c == node->hi) break;
            }
            return {s, e};
        }
        case NodeKind::Concat: {
            Fragment a = build_fragment(node->left.get(), nfa);
            Fragment b = build_fragment(node->right.get(), nfa);
            nfa.states[a.accept].epsilon.push_back(b.start);
            return {a.start, b.accept};
        }
        case NodeKind::Alt: {
            Fragment a = build_fragment(node->left.get(), nfa);
            Fragment b = build_fragment(node->right.get(), nfa);
            int s = new_state(nfa);
            int e = new_state(nfa);
            nfa.states[s].epsilon.push_back(a.start);
            nfa.states[s].epsilon.push_back(b.start);
            nfa.states[a.accept].epsilon.push_back(e);
            nfa.states[b.accept].epsilon.push_back(e);
            return {s, e};
        }
        case NodeKind::Star: {
            Fragment a = build_fragment(node->left.get(), nfa);
            int s = new_state(nfa);
            int e = new_state(nfa);
            nfa.states[s].epsilon.push_back(a.start);
            nfa.states[s].epsilon.push_back(e);
            nfa.states[a.accept].epsilon.push_back(a.start);
            nfa.states[a.accept].epsilon.push_back(e);
            return {s, e};
        }
        case NodeKind::Plus: {
            Fragment a = build_fragment(node->left.get(), nfa);
            int s = new_state(nfa);
            int e = new_state(nfa);
            nfa.states[s].epsilon.push_back(a.start);
            nfa.states[a.accept].epsilon.push_back(a.start);
            nfa.states[a.accept].epsilon.push_back(e);
            return {s, e};
        }
    }
    throw std::runtime_error("unknown node kind");
}

static bool can_reach_eps(const Nfa& nfa, int start, int target) {
    std::set<int> visited;
    std::queue<int> q;
    q.push(start);
    visited.insert(start);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        if (cur == target) return true;
        for (int nb : nfa.states[cur].epsilon) {
            if (!visited.count(nb)) {
                visited.insert(nb);
                q.push(nb);
            }
        }
    }
    return false;
}

Nfa NfaBuilder::build(const std::vector<Rule>& rules) {
    Nfa nfa;
    std::vector<Fragment> frags;

    for (size_t i = 0; i < rules.size(); ++i) {
        const Rule& rule = rules[i];
        auto tokens = tokenize(rule.expression, rule.name);

        if (tokens.size() == 1 && tokens[0].kind == TokenKind::EndOfInput)
            throw std::runtime_error("rule '" + rule.name + "': empty alternative");

        if (tokens[0].kind == TokenKind::Pipe)
            throw std::runtime_error("rule '" + rule.name + "': empty alternative");

        Parser parser{tokens, 0, rule.name};
        auto ast = parser.parseAlt();

        if (!parser.at_end())
            throw std::runtime_error("rule '" + rule.name + "': unmatched parenthesis");

        Fragment frag = build_fragment(ast.get(), nfa);

        if (can_reach_eps(nfa, frag.start, frag.accept))
            throw std::runtime_error("rule '" + rule.name + "': expression can match the empty string");

        nfa.states[frag.accept].rule = static_cast<int>(i);
        frags.push_back(frag);
    }

    int global_start = new_state(nfa);
    for (auto& f : frags) {
        nfa.states[global_start].epsilon.push_back(f.start);
    }
    nfa.start = global_start;

    return nfa;
}

void dump_nfa(const Nfa& nfa, std::ostream& out) {
    for (size_t i = 0; i < nfa.states.size(); ++i) {
        out << "State " << i;
        if (static_cast<int>(i) == nfa.start) out << " [START]";
        if (nfa.states[i].rule != -1) out << " [ACCEPT rule=" << nfa.states[i].rule << "]";
        out << "\n";
        for (const auto& [c, targets] : nfa.states[i].transitions) {
            for (int t : targets) {
                out << "  --'" << c << "'--> " << t << "\n";
            }
        }
        for (int t : nfa.states[i].epsilon) {
            out << "  --EPS--> " << t << "\n";
        }
    }
}
