#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cctype>
using namespace std;

// Function to trim leading and trailing whitespace from a string
string Trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) {
        return ""; // String is all whitespace
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// DFA class definition
class DFA {
public:
    DFA(set<char> A, set<int> I, set<int> F) : alpha(A), init_states(I), fin_states(F) { Reset(); }

    void Reset() {
        current_state = *init_states.begin();
        accepted = false;
        accepted_lexeme.clear();
    }

    void AddTransition(int from, int to, char symbol) {
        Dtran[from][symbol] = to;
    }

    void AddFinalState(int state) {
        fin_states.insert(state);
    }

    bool GetAccepted() {
        return accepted;
    }

    string GetAcceptedLexeme() {
        return accepted_lexeme;
    }

    bool Move(char c) {
        if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
            current_state = Dtran[current_state][c];
            accepted_lexeme += c;
            if (fin_states.count(current_state)) {
                accepted = true;
            }
            return true; // Transition successful
        } else {
            accepted = false;
            return false; // Transition failed
        }
    }

private:
    set<char> alpha;
    set<int> init_states;
    set<int> fin_states;
    map<int, map<char, int>> Dtran;
    int current_state;
    bool accepted;
    string accepted_lexeme;
};

// NFA State
struct State {
    int id;
    vector<pair<char, shared_ptr<State>>> transitions; // transitions are labeled with characters
};

// NFA Structure
struct NFA {
    shared_ptr<State> start;
    shared_ptr<State> accept;
};

// Function to retrieve NFA state by ID
shared_ptr<State> GetStateById(const NFA &nfa, int id) {
    queue<shared_ptr<State>> q;
    q.push(nfa.start);
    while (!q.empty()) {
        auto state = q.front();
        q.pop();
        if (state->id == id) {
            return state;
        }
        for (const auto &transition : state->transitions) {
            q.push(transition.second);
        }
    }
    return nullptr;
}

// Epsilon closure function
set<int> EpsilonClosure(const NFA &nfa, const set<int> &states) {
    set<int> closure = states;
    queue<int> stateQueue;
    for (int state : states) {
        stateQueue.push(state);
    }
    while (!stateQueue.empty()) {
        int currentState = stateQueue.front();
        stateQueue.pop();
        shared_ptr<State> nfaState = GetStateById(nfa, currentState);
        if (nfaState == nullptr) {
            continue;
        }
        for (const auto &transition : nfaState->transitions) {
            if (transition.first == '\0') {
                int targetState = transition.second->id;
                if (closure.find(targetState) == closure.end()) {
                    closure.insert(targetState);
                    stateQueue.push(targetState);
                }
            }
        }
    }
    return closure;
}

// Subset construction to convert NFA to DFA
DFA NFAtoDFA(const NFA &nfa) {
    set<char> alphabet = {'a', 'b', 'c', 'd', 'e'}; // Define the alphabet dynamically
    set<int> startSet = {nfa.start->id};
    DFA dfa(alphabet, startSet, {}); // Initial DFA with defined alphabet and states

    map<set<int>, int> stateMapping; // Map NFA state sets to DFA state IDs
    map<int, set<int>> dfaStateSets; // Map DFA state IDs to NFA state sets
    queue<set<int>> stateQueue;
    set<int> currentSet = EpsilonClosure(nfa, startSet);
    int stateId = 0;

    stateMapping[currentSet] = stateId++;
    dfaStateSets[stateMapping[currentSet]] = currentSet;
    stateQueue.push(currentSet);

    while (!stateQueue.empty()) {
        currentSet = stateQueue.front();
        stateQueue.pop();

        map<char, set<int>> transitions;

        for (int nfaStateId : currentSet) {
            shared_ptr<State> nfaState = GetStateById(nfa, nfaStateId);
            for (const auto &transition : nfaState->transitions) {
                char symbol = transition.first;
                if (symbol != '\0') {
                    set<int> targetSet = EpsilonClosure(nfa, {transition.second->id});
                    transitions[symbol].insert(targetSet.begin(), targetSet.end());
                }
            }
        }

        for (const auto &trans : transitions) {
            char symbol = trans.first;
            set<int> targetSet = trans.second;

            if (stateMapping.find(targetSet) == stateMapping.end()) {
                stateMapping[targetSet] = stateId++;
                dfaStateSets[stateMapping[targetSet]] = targetSet;
                stateQueue.push(targetSet);
            }

            dfa.AddTransition(stateMapping[currentSet], stateMapping[targetSet], symbol);
        }
    }

    // Define DFA final states
    for (const auto &stateSetPair : stateMapping) {
        const set<int> &stateSet = stateSetPair.first;
        int dfaStateId = stateSetPair.second;
        if (stateSet.find(nfa.accept->id) != stateSet.end()) {
            dfa.AddFinalState(dfaStateId);
        }
    }

    return dfa;
}

// Check if a character is an operand (alpha)
bool IsOperand(char c) {
    return isalpha(c);
}

// Define precedence and associativity of operators
int Precedence(char op) {
    if (op == '*') return 3; // highest precedence for Kleene star (*)
    if (op == '.') return 2; // concatenation (.) has lower precedence than *
    if (op == '|') return 1; // alternation (|) has the lowest precedence
    return 0;
}

// Convert an infix regular expression to postfix
string InfixToPostfix(const string &infix) {
    stack<char> ops;
    string postfix;
    for (char c : infix) {
        if (isalnum(c)) {
            postfix += c;
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop();
        } else {
            while (!ops.empty() && Precedence(ops.top()) >= Precedence(c)) {
                postfix += ops.top();
                ops.pop();
            }
            ops.push(c);
        }
    }
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

// Thompson's Construction: Create NFA from postfix regex
NFA PostfixToNFA(const string &postfix) {
    stack<NFA> nfaStack;
    int stateId = 0;

    for (char c : postfix) {
        if (IsOperand(c)) {
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({c, accept});
            nfaStack.push({start, accept});
        } else if (c == '.') {
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            nfa1.accept->transitions.push_back({'\0', nfa2.start});
            nfaStack.push({nfa1.start, nfa2.accept});
        } else if (c == '|') {
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({'\0', nfa1.start});
            start->transitions.push_back({'\0', nfa2.start});
            nfa1.accept->transitions.push_back({'\0', accept});
            nfa2.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
        } else if (c == '*') {
            NFA nfa = nfaStack.top(); nfaStack.pop();
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({'\0', nfa.start});
            start->transitions.push_back({'\0', accept});
            nfa.accept->transitions.push_back({'\0', nfa.start});
            nfa.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
        }
    }

    return nfaStack.top();
}

int main() {
    // Read input
    string tokenDefs, inputString;
    getline(cin, tokenDefs);
    getline(cin, inputString);

    // Trim input string
    inputString = Trim(inputString);

    // Parse token definitions
    vector<pair<string, string>> tokens;
    size_t hashPos = tokenDefs.find('#');
    if (hashPos != string::npos) {
        tokenDefs = tokenDefs.substr(0, hashPos);
    }
    size_t pos = 0;
    while ((pos = tokenDefs.find(",")) != string::npos) {
        string tokenDef = tokenDefs.substr(0, pos);
        size_t spacePos = tokenDef.find(" ");
        if (spacePos != string::npos) {
            string tokenName = tokenDef.substr(0, spacePos);
            string tokenRegex = tokenDef.substr(spacePos + 1);
            tokens.push_back({tokenName, tokenRegex});
        }
        tokenDefs.erase(0, pos + 1);
    }

    // Process each token definition
    vector<DFA> dfas;
    for (const auto &token : tokens) {
        string postfix = InfixToPostfix(token.second);
        NFA nfa = PostfixToNFA(postfix);
        DFA dfa = NFAtoDFA(nfa);
        dfas.push_back(dfa);
    }

    // Perform lexical analysis
    size_t index = 0;
    while (index < inputString.length()) {
        while (index < inputString.length() && isspace(inputString[index])) {
            index++;
        }
        if (index >= inputString.length()) break;

        bool matched = false;
        for (size_t i = 0; i < dfas.size(); i++) {
            DFA dfa = dfas[i];
            dfa.Reset();
            size_t j = index;
            while (j < inputString.length() && dfa.Move(inputString[j])) {
                j++;
            }
            if (dfa.GetAccepted()) {
                string lexeme = inputString.substr(index, j - index);
                cout << tokens[i].first << " , \"" << lexeme << "\"" << endl;
                index = j;
                matched = true;
                break;
            }
        }
        if (!matched) {
            cout << "ERROR , \"" << inputString[index] << "\"" << endl;
            index++;
        }
    }

    return 0;
}
