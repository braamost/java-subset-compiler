#include "rules.hpp"
#include <algorithm>
#include <cctype>
#include <map>
#include <stdexcept>

namespace {

enum class ExpansionState { Unexpanded, Expanding, Expanded };

struct Definition {
    std::string expression;
    std::size_t line;
    ExpansionState state = ExpansionState::Unexpanded;
};

struct RuleEntry {
    Rule rule;
    std::size_t line;
    bool needs_expansion;
};

struct ParsedRuleFile {
    std::map<std::string, Definition> definitions;
    std::vector<RuleEntry> entries;
};

[[noreturn]] void report_rule_error(std::size_t line, const std::string& message) {
    throw std::runtime_error("line " + std::to_string(line) + ": " + message);
}

bool is_space(char character) {
    return std::isspace(static_cast<unsigned char>(character)) != 0;
}

bool is_name_start(char character) {
    return std::isalpha(static_cast<unsigned char>(character)) || character == '_';
}

bool is_name_character(char character) {
    return is_name_start(character) || std::isdigit(static_cast<unsigned char>(character));
}

bool is_valid_name(const std::string& name) {
    return !name.empty() && is_name_start(name.front()) &&
           std::all_of(name.begin() + 1, name.end(), is_name_character);
}

bool is_escaped_character(const std::string& text, std::size_t position) {
    std::size_t backslashes = 0;
    while (position > 0 && text[position - 1] == '\\') {
        ++backslashes;
        --position;
    }
    return backslashes % 2 != 0;
}

std::string trim_whitespace(const std::string& text) {
    std::size_t start = 0;
    std::size_t end = text.size();
    while (start < end && is_space(text[start])) ++start;
    while (end > start && is_space(text[end - 1])) {
        // An escaped trailing space is part of the expression, not formatting.
        if (is_escaped_character(text, end - 1)) break;
        --end;
    }
    return text.substr(start, end - start);
}

std::string escape_literal_expression(const std::string& literal) {
    const std::string reserved = "\\|*+-(){}[]=:";
    std::string expression;
    for (char character : literal) {
        if (is_space(character) || reserved.find(character) != std::string::npos) {
            expression += '\\';
        }
        expression += character;
    }
    return expression;
}

std::vector<std::string> read_group_literals(const std::string& contents, std::size_t line) {
    std::vector<std::string> literals;
    std::size_t position = 0;
    while (position < contents.size()) {
        if (is_space(contents[position])) {
            ++position;
            continue;
        }
        std::string literal;
        while (position < contents.size() && !is_space(contents[position])) {
            if (contents[position] == '\\') {
                ++position;
                if (position == contents.size()) report_rule_error(line, "trailing backslash in group");
            }
            literal += contents[position++];
        }
        literals.push_back(literal);
    }
    if (literals.empty()) report_rule_error(line, "empty keyword or punctuation group");
    return literals;
}

void parse_literal_group(const std::string& text, std::size_t line, ParsedRuleFile& parsed) {
    bool keywords = text.front() == '{';
    char closing = keywords ? '}' : ']';
    if (text.back() != closing || is_escaped_character(text, text.size() - 1)) {
        report_rule_error(line, std::string("group must end with '") + closing + "'");
    }
    auto literals = read_group_literals(text.substr(1, text.size() - 2), line);
    for (const auto& literal : literals) {
        if (keywords && !is_valid_name(literal)) {
            report_rule_error(line, "invalid keyword '" + literal + "'");
        }
        // Keywords win ties; equal priorities use the original vector order.
        parsed.entries.push_back({{literal, escape_literal_expression(literal), keywords ? 0 : 1},
                                  line, false});
    }
}

void parse_named_entry(const std::string& text, std::size_t line, ParsedRuleFile& parsed) {
    std::size_t separator = text.find_first_of("=:");
    if (separator == std::string::npos) {
        report_rule_error(line, "expected '=' or ':' after a name");
    }
    std::string name = trim_whitespace(text.substr(0, separator));
    if (!is_valid_name(name)) report_rule_error(line, "invalid definition or token name");
    std::string expression = trim_whitespace(text.substr(separator + 1));
    if (expression.empty()) report_rule_error(line, "missing expression for '" + name + "'");

    if (text[separator] == '=') {
        auto inserted = parsed.definitions.emplace(name, Definition{expression, line});
        if (!inserted.second) report_rule_error(line, "duplicate definition '" + name + "'");
    } else {
        parsed.entries.push_back({{name, expression, 1}, line, true});
    }
}

ParsedRuleFile parse_rule_file(std::istream& input) {
    ParsedRuleFile parsed;
    std::string text;
    std::size_t line = 0;
    while (std::getline(input, text)) {
        ++line;
        text = trim_whitespace(text);
        if (text.empty()) continue;
        if (text.front() == '{' || text.front() == '[') {
            parse_literal_group(text, line, parsed);
        } else {
            parse_named_entry(text, line, parsed);
        }
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        throw std::runtime_error("could not read rule file");
    }
    return parsed;
}

// A range endpoint is a character even if a definition has that same name.
bool is_range_endpoint(const std::string& expression, std::size_t start, std::size_t end) {
    if (end - start != 1) return false;
    while (end < expression.size() && is_space(expression[end])) ++end;
    if (end < expression.size() && expression[end] == '-') return true;
    while (start > 0 && is_space(expression[start - 1])) --start;
    return start > 0 && expression[start - 1] == '-' &&
           !is_escaped_character(expression, start - 1);
}

class DefinitionExpander {
public:
    explicit DefinitionExpander(std::map<std::string, Definition>& definitions)
        : definitions_(definitions) {}

    void validate_definitions() {
        for (const auto& item : definitions_) {
            expand_definition(item.first, item.second.line);
        }
    }

    std::string expand_expression(const std::string& expression, std::size_t line) {
        std::string expanded;
        std::size_t position = 0;
        while (position < expression.size()) {
            if (expression[position] == '\\') {
                if (position + 1 == expression.size()) report_rule_error(line, "trailing backslash");
                expanded += expression.substr(position, 2);
                position += 2;
                continue;
            }
            if (!is_name_start(expression[position])) {
                expanded += expression[position++];
                continue;
            }

            std::size_t start = position++;
            while (position < expression.size() && is_name_character(expression[position])) ++position;
            std::string name = expression.substr(start, position - start);
            expanded += is_range_endpoint(expression, start, position) ? name : expand_name(name, line);
        }
        return expanded;
    }

private:
    std::string expand_name(const std::string& name, std::size_t line) {
        if (definitions_.count(name)) {
            return '(' + expand_definition(name, line) + ')';
        }
        if (name.size() == 1) return name;
        report_rule_error(line, "undefined definition '" + name +
                                "' (write literal words as escaped or space-separated characters)");
    }

    const std::string& expand_definition(const std::string& name, std::size_t reference_line) {
        auto& definition = definitions_.at(name);
        if (definition.state == ExpansionState::Expanded) return definition.expression;
        if (definition.state == ExpansionState::Expanding) {
            report_rule_error(reference_line, "cyclic definition involving '" + name + "'");
        }
        definition.state = ExpansionState::Expanding;
        definition.expression = expand_expression(definition.expression, definition.line);
        definition.state = ExpansionState::Expanded;
        return definition.expression;
    }

    std::map<std::string, Definition>& definitions_;
};

} // namespace

std::vector<Rule> RuleReader::read(std::istream& input) {
    ParsedRuleFile parsed = parse_rule_file(input);
    DefinitionExpander expander(parsed.definitions);
    expander.validate_definitions();

    std::vector<Rule> rules;
    for (auto& entry : parsed.entries) {
        if (entry.needs_expansion) {
            entry.rule.expression = expander.expand_expression(entry.rule.expression, entry.line);
        }
        rules.push_back(entry.rule);
    }
    return rules;
}
