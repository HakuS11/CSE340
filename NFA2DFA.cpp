#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

//--------------------------------------------------------------
// NFA State Representation
//--------------------------------------------------------------
using NFAState = int;
using NFATransition = map<char, set<NFAState>>; // Transition function for NFA
using NFAStates = set<NFAState>; // Set of NFA states

//--------------------------------------------------------------
// NFA Class
//--------------------------------------------------------------
class NFA {
public:
    NFAStates states; // Set of states
    NFAState startState; // Start state
    NFAStates acceptStates; // Accept states
    map<NFAState, NFATransition> transitions; // Transition function

    // Compute epsilon-closure of a set of states
    NFAStates epsilonClosure(const NFAStates& states) {
        NFAStates closure = states;
        queue<NFAState> q;
        for (NFAState state : states) {
            q.push(state);
        }
        while (!q.empty()) {
            NFAState current = q.front();
            q.pop();
            // Add all states reachable via epsilon transitions
            if (transitions[current].count('\0')) {
                for (NFAState nextState : transitions[current]['\0']) {
                    if (closure.find(nextState) == closure.end()) {
                        closure.insert(nextState);
                        q.push(nextState);
                    }
                }
            }
        }
        return closure;
    }

    // Compute the set of states reachable from a set of states on a given input symbol
    NFAStates move(const NFAStates& states, char symbol) {
        NFAStates result;
        for (NFAState state : states) {
            if (transitions[state].count(symbol)) {
                for (NFAState nextState : transitions[state][symbol]) {
                    result.insert(nextState);
                }
            }
        }
        return result;
    }
};

//--------------------------------------------------------------
// DFA State Representation
//--------------------------------------------------------------
using DFAState = set<NFAState>; // DFA state is a set of NFA states
using DFATransition = map<char, DFAState>; // Transition function for DFA

//--------------------------------------------------------------
// DFA Class
//--------------------------------------------------------------
class DFA {
public:
    set<DFAState> states; // Set of states
    DFAState startState; // Start state
    set<DFAState> acceptStates; // Accept states
    map<DFAState, DFATransition> transitions; // Transition function
};

//--------------------------------------------------------------
// NFA to DFA Conversion using Subset Construction
//--------------------------------------------------------------
DFA NFA2DFA(const NFA& nfa) {
    DFA dfa;
    // Compute the epsilon-closure of the NFA's start state
    dfa.startState = nfa.epsilonClosure({nfa.startState});
    dfa.states.insert(dfa.startState);

    queue<DFAState> q;
    q.push(dfa.startState);

    while (!q.empty()) {
        DFAState currentState = q.front();
        q.pop();

        // Check if the current state is an accept state
        for (NFAState state : currentState) {
            if (nfa.acceptStates.find(state) != nfa.acceptStates.end()) {
                dfa.acceptStates.insert(currentState);
                break;
            }
        }

        // Compute transitions for each input symbol
        for (char symbol : {'a', 'b'}) { // Assuming alphabet is {a, b}
            NFAStates nextNFAStates = nfa.move(currentState, symbol);
            DFAState nextDFAState = nfa.epsilonClosure(nextNFAStates);

            if (!nextDFAState.empty()) {
                dfa.transitions[currentState][symbol] = nextDFAState;
                if (dfa.states.find(nextDFAState) == dfa.states.end()) {
                    dfa.states.insert(nextDFAState);
                    q.push(nextDFAState);
                }
            }
        }
    }

    return dfa;
}

//--------------------------------------------------------------
// Main Function
//--------------------------------------------------------------
int main() {
    // Example NFA
    NFA nfa;
    nfa.states = {0, 1, 2};
    nfa.startState = 0;
    nfa.acceptStates = {2};
    nfa.transitions[0]['\0'] = {1}; // Epsilon transition from 0 to 1
    nfa.transitions[0]['a'] = {0}; // Transition on 'a' from 0 to 0
    nfa.transitions[1]['b'] = {2}; // Transition on 'b' from 1 to 2
    nfa.transitions[2]['b'] = {2}; // Transition on 'b' from 2 to 2

    // Convert NFA to DFA
    DFA dfa = NFA2DFA(nfa);

    // Print DFA states and transitions
    cout << "DFA States:\n";
    for (const DFAState& state : dfa.states) {
        cout << "{ ";
        for (NFAState s : state) {
            cout << s << " ";
        }
        cout << "}\n";
    }

    cout << "\nDFA Transitions:\n";
    for (const auto& [state, transition] : dfa.transitions) {
        for (const auto& [symbol, nextState] : transition) {
            cout << "{ ";
            for (NFAState s : state) {
                cout << s << " ";
            }
            cout << "} --" << symbol << "--> { ";
            for (NFAState s : nextState) {
                cout << s << " ";
            }
            cout << "}\n";
        }
    }

    cout << "\nDFA Accept States:\n";
    for (const DFAState& state : dfa.acceptStates) {
        cout << "{ ";
        for (NFAState s : state) {
            cout << s << " ";
        }
        cout << "}\n";
    }

    return 0;
}
