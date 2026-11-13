#include "automata.hpp"
#include <stdexcept>
#include <set>
#include <map>
#include <queue>

std::set<int> compute_epsilon_closure(const Nfa& nfa, const std::set<int>& start_states) {
    std::set<int> closure = start_states;
    std::queue<int> q;
    
    for (int state : start_states) {
        q.push(state);
    }

    while (!q.empty()) {
        int current = q.front();
        q.pop();
        for (int next_state : nfa.states[current].epsilon) {
            if (closure.find(next_state) == closure.end()) {
                closure.insert(next_state);
                q.push(next_state);
            }
        }
    }
    return closure;
}

Dfa DfaBuilder::build(const Nfa& nfa, const std::vector<Rule>& rules) {
    Dfa dfa;
    std::map<std::set<int>, int> dfa_state_map;
    std::queue<std::set<int>> worklist;

    std::set<int> nfa_start_set = { nfa.start };
    std::set<int> dfa_start_subset = compute_epsilon_closure(nfa, nfa_start_set);

    dfa.start = 0;
    dfa_state_map[dfa_start_subset] = 0;
    dfa.states.push_back(DfaState{}); 
    worklist.push(dfa_start_subset);

    while(!worklist.empty()){
        std::set<int> current_subset = worklist.front();
        worklist.pop();
        int current_dfa_id = dfa_state_map[current_subset];

        int best_rule = -1;
        int best_priority = -1;

        for (int nfa_id : current_subset) {
            int r = nfa.states[nfa_id].rule;
            if (r != -1) { // If it's an accepting state
                // Smaller priority wins. Original vector index breaks ties.
                if (best_rule == -1 || rules[r].priority < best_priority || 
                   (rules[r].priority == best_priority && r < best_rule)) {
                    best_rule = r;
                    best_priority = rules[r].priority;
                }
            }
        }
        dfa.states[current_dfa_id].rule = best_rule;

        std::set<char> alphabet;
        for (int nfa_id : current_subset) {
            for (const auto& transition : nfa.states[nfa_id].transitions) {
                alphabet.insert(transition.first);
            }
        }

        for (char c : alphabet) {
            std::set<int> move_result;
            for (int nfa_id : current_subset) {
                if (nfa.states[nfa_id].transitions.count(c)) {
                    for (int target : nfa.states[nfa_id].transitions.at(c)) {
                        move_result.insert(target);
                    }
                }
            }

            std::set<int> next_dfa_subset = compute_epsilon_closure(nfa, move_result);
            if (dfa_state_map.find(next_dfa_subset) == dfa_state_map.end()) {
                int new_id = dfa.states.size();
                dfa_state_map[next_dfa_subset] = new_id; 
                dfa.states.push_back(DfaState{}); 
                worklist.push(next_dfa_subset);   
            }
            dfa.states[current_dfa_id].transitions[c] = dfa_state_map[next_dfa_subset];
        }
    }
    return dfa;
}
Dfa DfaBuilder::minimize(const Dfa& dfa) {
    if (dfa.states.empty()) return dfa;

    std::map<int, std::set<int>> initial_partitions;
    for (int i = 0; i < dfa.states.size(); ++i) {
        initial_partitions[dfa.states[i].rule].insert(i);
    }
    std::vector<std::set<int>> partitions;
    for (const auto& pair : initial_partitions) {
        partitions.push_back(pair.second);
    }
    
    std::set<char> alphabet;
    for (const auto& state : dfa.states) {
        for (const auto& transition : state.transitions) {
            alphabet.insert(transition.first);
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;

        std::vector<int> state_to_partition(dfa.states.size(), -1);
        for (int i = 0; i < partitions.size(); ++i) {
            for (int state_id : partitions[i]) {
                state_to_partition[state_id] = i;
            }
        }

        std::vector<std::set<int>> new_partitions;
        for (const auto& group : partitions) {
            std::map<std::map<char, int>, std::set<int>> splits;
            for (int state_id : group) {
                std::map<char, int> signature;
                for (char c : alphabet) {
                    if (dfa.states[state_id].transitions.count(c)) {
                        int target_state = dfa.states[state_id].transitions.at(c);
                        signature[c] = state_to_partition[target_state];
                    } else {
                        signature[c] = -1; // Missing transition implies failure
                    }
                }
                splits[signature].insert(state_id);
            }
            for (const auto& split_pair : splits) {
                new_partitions.push_back(split_pair.second);
            }
            if (splits.size() > 1) {
                changed = true;
            }
        }
        partitions = new_partitions;
    }
    Dfa min_dfa;
    std::vector<int> state_to_new_id(dfa.states.size(), -1);

    for (int i = 0; i < partitions.size(); ++i) {
        min_dfa.states.push_back(DfaState{});
        for (int old_state : partitions[i]) {
            state_to_new_id[old_state] = i;
        }
    }

    for (int i = 0; i < partitions.size(); ++i) {
    
        int rep_state = *partitions[i].begin(); 
        min_dfa.states[i].rule = dfa.states[rep_state].rule;
        if (partitions[i].count(dfa.start)) {
            min_dfa.start = i;
        }
        // Remap all outgoing transitions
        for (const auto& transition : dfa.states[rep_state].transitions) {
            char c = transition.first;
            int old_target = transition.second;
            int new_target = state_to_new_id[old_target];
            min_dfa.states[i].transitions[c] = new_target;
        }
    }

    return min_dfa;

}
void DfaBuilder::write_table(const Dfa& dfa, std::ostream& output) {
   if (dfa.states.empty()) {
        output << "DFA is empty.\n";
        return;
    }
    std::set<char> alphabet;
    for (const auto& state : dfa.states) {
        for (const auto& transition : state.transitions) {
            alphabet.insert(transition.first);
        }
    }
    output << "State\t";
    for (char c : alphabet) {
        if (c == ' ') output << "' '\t";
        else output << c << "\t";
    }
    output << "Rule\n";
    output << std::string(15 + alphabet.size() * 4, '-') << "\n";
    for (int i = 0; i < dfa.states.size(); ++i) {
        
        // Mark the starting state visually
        if (i == dfa.start) {
            output << "->" << i << "\t";
        } else {
            output << i << "\t";
        }
        for (char c : alphabet) {
            if (dfa.states[i].transitions.count(c)) {
                output << dfa.states[i].transitions.at(c) << "\t";
            } else {
                output << "-\t"; // '-' indicates a missing edge/failure
            }
        }
        // Print the accepting rule ID
        if (dfa.states[i].rule != -1) {
            output << dfa.states[i].rule;
        } else {
            output << "None";
        }
        output << "\n";
    }
}
