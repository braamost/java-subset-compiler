#include "lexer.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace {
int run(int argc, char** argv) {
    if (argc == 2 && std::string(argv[1]) == "--help") {
        std::cout << "Usage: lexer RULES_FILE TABLE_FILE SOURCE_FILE [SOURCE_FILE ...]\n"
                     "Builds the rules and minimized DFA once, writes TABLE_FILE,\n"
                     "then prints token classes for each source file.\n";
        return 0;
    }
    if (argc < 4) {
        std::cerr << "Usage: lexer RULES_FILE TABLE_FILE SOURCE_FILE [SOURCE_FILE ...]\n";
        return 2;
    }

    std::ifstream rules_input(argv[1]);
    if (!rules_input) throw std::runtime_error("Cannot open rules file: " + std::string(argv[1]));
    LexerDefinition language(rules_input);

    std::ofstream table_output(argv[2]);
    if (!table_output) throw std::runtime_error("Cannot create transition table: " + std::string(argv[2]));
    language.writeTransitionTable(table_output);
    table_output.close();
    if (!table_output) throw std::runtime_error("Failed writing transition table");

    for (int index = 3; index < argc; ++index) {
        std::ifstream source(argv[index], std::ios::binary);
        if (!source) throw std::runtime_error("Cannot open source file: " + std::string(argv[index]));
        auto scanner = language.scanner(source);
        while (auto token = scanner.nextToken()) std::cout << token->type << '\n';
        for (std::size_t offset : scanner.errorOffsets())
            std::cerr << "Lexical error in " << argv[index] << " at byte " << offset << '\n';
        if (!std::cout) throw std::runtime_error("Failed writing token output");
    }
    return 0;
}
}

int main(int argc, char** argv) {
    try {
        return run(argc, argv);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 2;
    }
}
