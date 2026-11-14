#include "automata.hpp"
#include <iostream>
#include <vector>

int main() {

    std::vector<Rule> rules;
    rules.push_back({"KEYWORD_IF", "if", 1});     // Priority 1 (Wins ties)
    rules.push_back({"ID", "(i|f)+", 2});         // Priority 2

    Nfa mock_nfa;
    mock_nfa.start = 0;
    
    for (int i = 0; i < 8; ++i) {
        mock_nfa.states.push_back(NfaState{});
    }

    // --- State 0: Unified Start State ---
    // Instantly branches to the NFA for Rule 0 and the NFA for Rule 1
    mock_nfa.states[0].epsilon = {1, 4};

    // --- NFA Path for Rule 0: "if" ---
    mock_nfa.states[1].transitions['i'] = {2};
    mock_nfa.states[2].transitions['f'] = {3};
    mock_nfa.states[3].rule = 0; // Accept Rule 0 (Keyword)

    // --- NFA Path for Rule 1: "(i|f)+" ---
    // State 4 branches on either 'i' or 'f'
    mock_nfa.states[4].transitions['i'] = {5};
    mock_nfa.states[4].transitions['f'] = {5};
    
    // State 5 uses epsilon to instantly reach an accepting state 
    // AND loop back to state 4 (simulating the '+' one-or-more operator)
    mock_nfa.states[5].epsilon = {6, 7};
    mock_nfa.states[6].rule = 1; // Accept Rule 1 (Identifier)
    mock_nfa.states[7].epsilon = {4}; // Loop back to allow more characters

    // ==========================================
    // 3. Test YOUR Algorithms (Student 3)
    // ==========================================
    DfaBuilder builder;
    
    std::cout << "==========================================\n";
    std::cout << "          INITIAL DFA (Subset Const.)     \n";
    std::cout << "==========================================\n";
    Dfa dfa = builder.build(mock_nfa, rules);
    builder.write_table(dfa, std::cout);

    std::cout << "\n==========================================\n";
    std::cout << "          MINIMIZED DFA (Hopcroft)        \n";
    std::cout << "==========================================\n";
    Dfa min_dfa = builder.minimize(dfa);
    builder.write_table(min_dfa, std::cout);

    return 0;
}