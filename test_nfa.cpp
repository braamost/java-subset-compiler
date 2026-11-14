#include "automata.hpp"
#include <iostream>
#include <set>
#include <queue>
#include <string>
#include <vector>
#include <stdexcept>

static std::set<int> eps_closure(const Nfa& nfa, std::set<int> states) {
    std::queue<int> q;
    for (int s : states) q.push(s);
    while (!q.empty()) {
        int cur = q.front(); q.pop();
        for (int nb : nfa.states[cur].epsilon) {
            if (!states.count(nb)) {
                states.insert(nb);
                q.push(nb);
            }
        }
    }
    return states;
}

static std::set<int> move(const Nfa& nfa, const std::set<int>& states, char c) {
    std::set<int> result;
    for (int s : states) {
        auto it = nfa.states[s].transitions.find(c);
        if (it != nfa.states[s].transitions.end()) {
            for (int t : it->second) result.insert(t);
        }
    }
    return result;
}

static bool nfa_accepts(const Nfa& nfa, const std::string& input) {
    std::set<int> current = eps_closure(nfa, {nfa.start});
    for (char c : input) {
        current = eps_closure(nfa, move(nfa, current, c));
        if (current.empty()) return false;
    }
    for (int s : current) {
        if (nfa.states[s].rule != -1) return true;
    }
    return false;
}

static int pass_count = 0;
static int fail_count = 0;

static void check(const std::string& label, bool cond) {
    if (cond) {
        std::cout << "  PASS: " << label << "\n";
        ++pass_count;
    } else {
        std::cout << "  FAIL: " << label << "\n";
        ++fail_count;
    }
}

static void section(const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
}

int main() {
    NfaBuilder builder;

    section("Test 1: id = (a-z|A-Z)((a-z|A-Z)|(0-9))*");
    {
        std::vector<Rule> rules = {{"id", "(a-z|A-Z)((a-z|A-Z)|(0-9))*", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts 'sum'",    nfa_accepts(nfa, "sum"));
            check("accepts 'x1'",     nfa_accepts(nfa, "x1"));
            check("accepts 'a'",      nfa_accepts(nfa, "a"));
            check("rejects '1x'",    !nfa_accepts(nfa, "1x"));
            check("rejects ''",      !nfa_accepts(nfa, ""));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 5;
        }
    }

    section("Test 2: num = (0-9)+|(0-9)+.(0-9)+(\\L|E(0-9)+)");
    {
        std::vector<Rule> rules = {{"num", "(0-9)+|(0-9)+\\.(0-9)+(\\L|E(0-9)+)", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts '10'",      nfa_accepts(nfa, "10"));
            check("accepts '3.14'",    nfa_accepts(nfa, "3.14"));
            check("accepts '3.14E5'",  nfa_accepts(nfa, "3.14E5"));
            check("rejects '3.'",     !nfa_accepts(nfa, "3."));
            check("rejects '.5'",     !nfa_accepts(nfa, ".5"));
            check("rejects '12E3'",   !nfa_accepts(nfa, "12E3"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 6;
        }
    }

    section("Test 3: relop = \\=\\=|!\\=|>|>\\=|<|<\\=");
    {
        std::vector<Rule> rules = {{"relop", "\\=\\=|!\\=|>|>\\=|<|<\\=", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts '=='", nfa_accepts(nfa, "=="));
            check("accepts '!='", nfa_accepts(nfa, "!="));
            check("accepts '>='", nfa_accepts(nfa, ">="));
            check("accepts '<='", nfa_accepts(nfa, "<="));
            check("accepts '<'",  nfa_accepts(nfa, "<"));
            check("accepts '>'",  nfa_accepts(nfa, ">"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 6;
        }
    }

    section("Test 4: addop = \\+|\\-");
    {
        std::vector<Rule> rules = {{"addop", "\\+|\\-", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts '+'", nfa_accepts(nfa, "+"));
            check("accepts '-'", nfa_accepts(nfa, "-"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 2;
        }
    }

    section("Test 5: mulop = \\*|/");
    {
        std::vector<Rule> rules = {{"mulop", "\\*|/", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts '*'", nfa_accepts(nfa, "*"));
            check("accepts '/'", nfa_accepts(nfa, "/"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 2;
        }
    }

    section("Test 6: Range with spaces '0 - 9'");
    {
        std::vector<Rule> rules = {{"digit", "0 - 9", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts '5'", nfa_accepts(nfa, "5"));
            check("accepts '0'", nfa_accepts(nfa, "0"));
            check("accepts '9'", nfa_accepts(nfa, "9"));
            check("rejects 'a'", !nfa_accepts(nfa, "a"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 4;
        }
    }

    section("Test 7: Precedence a|bc*");
    {
        std::vector<Rule> rules = {{"prec", "a|bc*", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts 'a'",   nfa_accepts(nfa, "a"));
            check("accepts 'b'",   nfa_accepts(nfa, "b"));
            check("accepts 'bcc'", nfa_accepts(nfa, "bcc"));
            check("rejects 'ac'",  !nfa_accepts(nfa, "ac"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 4;
        }
    }

    section("Test 8: Plus correctness a+");
    {
        std::vector<Rule> rules = {{"aplus", "a+", 0}};
        try {
            auto nfa = builder.build(rules);
            check("accepts 'a'",   nfa_accepts(nfa, "a"));
            check("accepts 'aaa'", nfa_accepts(nfa, "aaa"));
            check("rejects ''",   !nfa_accepts(nfa, ""));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 3;
        }
    }

    section("Test 9: Combined NFA structure");
    {
        std::vector<Rule> rules = {
            {"r0", "a", 0},
            {"r1", "b", 1}
        };
        try {
            auto nfa = builder.build(rules);
            int gs = nfa.start;
            check("global start has >=2 epsilon edges", (int)nfa.states[gs].epsilon.size() >= 2);
            int r0_start = nfa.states[gs].epsilon[0];
            int r1_start = nfa.states[gs].epsilon[1];
            (void)r0_start; (void)r1_start;
            bool found_r0 = false, found_r1 = false;
            for (const auto& st : nfa.states) {
                if (st.rule == 0) found_r0 = true;
                if (st.rule == 1) found_r1 = true;
            }
            check("accept state for rule 0 exists", found_r0);
            check("accept state for rule 1 exists", found_r1);
            check("accepts 'a' (rule 0)", nfa_accepts(nfa, "a"));
            check("accepts 'b' (rule 1)", nfa_accepts(nfa, "b"));
            check("rejects 'ab'", !nfa_accepts(nfa, "ab"));
        } catch (const std::exception& e) {
            std::cout << "  FAIL (exception): " << e.what() << "\n";
            fail_count += 6;
        }
    }

    section("Test 10: Error cases");

    auto expect_throw = [&](const std::string& label, const std::vector<Rule>& rules) {
        try {
            builder.build(rules);
            std::cout << "  FAIL: " << label << " (no exception thrown)\n";
            ++fail_count;
        } catch (const std::runtime_error&) {
            std::cout << "  PASS: " << label << " (threw runtime_error)\n";
            ++pass_count;
        } catch (...) {
            std::cout << "  FAIL: " << label << " (wrong exception type)\n";
            ++fail_count;
        }
    };

    expect_throw("unbalanced paren '(ab'",         {{"e", "(ab", 0}});
    expect_throw("empty alt 'a||b'",               {{"e", "a||b", 0}});
    expect_throw("dangling op at start '*a'",      {{"e", "*a", 0}});
    expect_throw("bad range 'z-a'",                {{"e", "z-a", 0}});
    expect_throw("trailing backslash 'ab\\'",       {{"e", "ab\\", 0}});
    expect_throw("empty-match 'a*'",               {{"e", "a*", 0}});
    expect_throw("empty-match '\\L'",              {{"e", "\\L", 0}});

    std::cout << "\n========================================\n";
    std::cout << "Results: " << pass_count << " passed, " << fail_count << " failed\n";
    std::cout << "========================================\n";

    return fail_count == 0 ? 0 : 1;
}
