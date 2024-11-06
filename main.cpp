#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>

struct NFA {
    std::set<int> states;
    std::set<char> alphabet;
    int startState;
    std::set<int> finalStates;
    std::map<int, std::map<char, std::set<int>>> transitions;
};

struct DFA {
    std::set<std::set<int>> states;
    std::set<char> alphabet;
    std::set<int> finalStates;
    std::map<std::set<int>, std::map<char, std::set<int>>> transitions;
    std::set<int> startState;
};

NFA readNFA(const std::string& filename) {
    NFA nfa;
    std::ifstream input(filename);

    int numStates, numFinalStates, state;
    input >> numStates;

    for (int i = 0; i < numStates; i++) {
        input >> state;
        nfa.states.insert(state);
    }

    input >> nfa.startState;
    input >> numFinalStates;

    for (int i = 0; i < numFinalStates; i++) {
        input >> state;
        nfa.finalStates.insert(state);
    }

    int fromState;
    char symbol;
    int toState;
    while (input >> fromState >> symbol >> toState) {
        nfa.alphabet.insert(symbol);
        nfa.transitions[fromState][symbol].insert(toState);
    }

    input.close();
    return nfa;
}

DFA convertToDFA(const NFA& nfa) {
    DFA dfa;
    dfa.alphabet = nfa.alphabet;

    std::queue<std::set<int>> queue;
    std::map<std::set<int>, std::set<int>> stateMap;

    dfa.startState.insert(nfa.startState);
    queue.push(dfa.startState);
    dfa.states.insert(dfa.startState);

    while (!queue.empty()) {
        auto currentState = queue.front();
        queue.pop();

        for (char symbol : nfa.alphabet) {
            std::set<int> newState;
            for (int s : currentState) {
                if (nfa.transitions.count(s) && nfa.transitions.at(s).count(symbol)) {
                    newState.insert(nfa.transitions.at(s).at(symbol).begin(), nfa.transitions.at(s).at(symbol).end());
                }
            }

            if (!newState.empty()) {
                dfa.transitions[currentState][symbol] = newState;
                if (dfa.states.find(newState) == dfa.states.end()) {
                    dfa.states.insert(newState);
                    queue.push(newState);
                }
            }
        }
    }

    for (auto& stateSet : dfa.states) {
        for (int s : stateSet) {
            if (nfa.finalStates.count(s)) {
                dfa.finalStates.insert(*stateSet.begin());
                break;
            }
        }
    }

    return dfa;
}

void writeDFA(const DFA& dfa, const std::string& filename) {
    std::ofstream output(filename);

    output << dfa.states.size() << "\n";
    for (auto& stateSet : dfa.states) {
        output << *stateSet.begin() << " ";
    }
    output << "\n" << *dfa.startState.begin() << "\n";
    output << dfa.finalStates.size() << " ";
    for (int state : dfa.finalStates) {
        output << state << " ";
    }
    output << "\n";

    for (auto& [fromState, transitions] : dfa.transitions) {
        for (auto& [symbol, toState] : transitions) {
            output << *fromState.begin() << " " << symbol << " " << *toState.begin() << "\n";
        }
    }

    output.close();
}

int main() {
    NFA nfa = readNFA("nfa.txt");  
    DFA dfa = convertToDFA(nfa);   
    writeDFA(dfa, "D:\\Lab\\output.txt");  

    std::cout << "Conversion complete. Output written to D:\\Lab\\output.txt" << std::endl;
    return 0;
}

