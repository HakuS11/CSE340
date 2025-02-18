#include <iostream>
#include <stack>
#include <set>
#include <map>
using namespace std;

//--------------------------------------------------------------
// NFA State Representation
//--------------------------------------------------------------
using NFAState = int;
using NFATransition = map<char, set<NFAState>>; // Transition function for NFA
using NFAStates = set<NFAState>; // Set of NFA states

//--------------------------------------------------------------
// NFA Fragment
//--------------------------------------------------------------
struct NFAFragment {
    NFAState startState;
    NFAStates acceptStates;
    map<NFAState, NFATransition> transitions;
};

//--------------------------------------------------------------
// Regex to NFA using Thompson's Construction
//--------------------------------------------------------------
class RegexToNFA {
public:
    RegexToNFA() : stateCounter(0) {}

    // Convert a regular expression (postfix notation) to an NFA
    NFAFragment regex2NFA(const string& postfixRegex) {
        stack<NFAFragment> nfaStack;

        for (char c : postfixRegex) {
            switch (c) {
                case '.': // Concatenation
                    concatenate(nfaStack);
                    break;
                case '|': // Union
                    unionOp(nfaStack);
                    break;
                case '*': // Kleene Star
                    kleeneStar(nfaStack);
                    break;
                default: // Single character
                    nfaStack.push(singleChar(c));
                    break;
            }
        }

        if (nfaStack.size() != 1) {
            throw runtime_error("Invalid regular expression");
        }

        return nfaStack.top();
    }

private:
    int stateCounter;

    // Create a new NFA state
    NFAState newState() {
        return stateCounter++;
    }

    // Create an NFA fragment for a single character
    NFAFragment singleChar(char c) {
        NFAFragment fragment;
        fragment.startState = newState();
        NFAState acceptState = newState();
        fragment.acceptStates = {acceptState};
        fragment.transitions[fragment.startState][c] = {acceptState};
        return fragment;
    }

    // Create an NFA fragment for epsilon (ε)
    NFAFragment epsilon() {
        NFAFragment fragment;
        fragment.startState = newState();
        NFAState acceptState = newState();
        fragment.acceptStates = {acceptState};
        fragment.transitions[fragment.startState]['\0'] = {acceptState};
        return fragment;
    }

    // Concatenate two NFA fragments
    void concatenate(stack<NFAFragment>& nfaStack) {
        NFAFragment fragment2 = nfaStack.top();
        nfaStack.pop();
        NFAFragment fragment1 = nfaStack.top();
        nfaStack.pop();

        // Connect the accept states of fragment1 to the start state of fragment2
        for (NFAState state : fragment1.acceptStates) {
            fragment1.transitions[state]['\0'].insert(fragment2.startState);
        }

        // Combine transitions
        fragment1.acceptStates = fragment2.acceptStates;
        fragment1.transitions.insert(fragment2.transitions.begin(), fragment2.transitions.end());

        nfaStack.push(fragment1);
    }

    // Union of two NFA fragments
    void unionOp(stack<NFAFragment>& nfaStack) {
        NFAFragment fragment2 = nfaStack.top();
        nfaStack.pop();
        NFAFragment fragment1 = nfaStack.top();
        nfaStack.pop();

        NFAFragment fragment;
        fragment.startState = newState();
        fragment.acceptStates = {newState()};

        // Add epsilon transitions from the new start state to the start states of fragment1 and fragment2
        fragment.transitions[fragment.startState]['\0'] = {fragment1.startState, fragment2.startState};

        // Add epsilon transitions from the accept states of fragment1 and fragment2 to the new accept state
        for (NFAState state : fragment1.acceptStates) {
            fragment.transitions[state]['\0'].insert(fragment.acceptStates.begin(), fragment.acceptStates.end());
        }
        for (NFAState state : fragment2.acceptStates) {
            fragment.transitions[state]['\0'].insert(fragment.acceptStates.begin(), fragment.acceptStates.end());
        }

        // Combine transitions
        fragment.transitions.insert(fragment1.transitions.begin(), fragment1.transitions.end());
        fragment.transitions.insert(fragment2.transitions.begin(), fragment2.transitions.end());

        nfaStack.push(fragment);
    }

    // Kleene Star of an NFA fragment
    void kleeneStar(stack<NFAFragment>& nfaStack) {
        NFAFragment fragment = nfaStack.top();
        nfaStack.pop();

        NFAFragment newFragment;
        newFragment.startState = newState();
        newFragment.acceptStates = {newState()};

        // Add epsilon transitions from the new start state to the start state of the fragment and to the new accept state
        newFragment.transitions[newFragment.startState]['\0'] = {fragment.startState, *newFragment.acceptStates.begin()};

        // Add epsilon transitions from the accept states of the fragment to the start state of the fragment and to the new accept state
        for (NFAState state : fragment.acceptStates) {
            newFragment.transitions[state]['\0'].insert(fragment.startState);
            newFragment.transitions[state]['\0'].insert(*newFragment.acceptStates.begin());
        }

        // Combine transitions
        newFragment.transitions.insert(fragment.transitions.begin(), fragment.transitions.end());

        nfaStack.push(newFragment);
    }
};

//--------------------------------------------------------------
// Main Function
//--------------------------------------------------------------
int main() {
    // Example: Convert the regular expression "ab|c*" to NFA
    string postfixRegex = "ab.c*|"; // Postfix notation for "ab|c*"

    RegexToNFA converter;
    NFAFragment nfa = converter.regex2NFA(postfixRegex);

    // Print NFA
    cout << "NFA Start State: " << nfa.startState << endl;
    cout << "NFA Accept States: ";
    for (NFAState state : nfa.acceptStates) {
        cout << state << " ";
    }
    cout << endl;

    cout << "NFA Transitions:\n";
    for (const auto& [state, transition] : nfa.transitions) {
        for (const auto& [symbol, nextStates] : transition) {
            for (NFAState nextState : nextStates) {
                cout << state << " --" << (symbol == '\0' ? "ε" : string(1, symbol)) << "--> " << nextState << endl;
            }
        }
    }

    return 0;
}
