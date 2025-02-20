#include <iostream>
#include <sstream>
#include <vector>
#include <stack>
#include <memory>
#include <string>
#include <cctype>
#include <queue>
#include <set>
#include <map>
using namespace std;

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

    bool GetAccepted() const {
        return accepted;
    }

    string GetAcceptedLexeme() const {
        return accepted_lexeme;
    }

    bool Move(char c) {
        if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
            current_state = Dtran[current_state][c];
            accepted_lexeme += c;
            cout << "Moved to state " << current_state << " on symbol " << c << endl;
            if (fin_states.count(current_state)) {
                accepted = true;
            }
            return true; // Transition successful
        } else {
            accepted = false;
            return false; // Transition failed
        }
    }

    map<int, map<char, int>> GetTransitions() const {
        return Dtran;
    }

    int GetCurrentState() const {
        return current_state;
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

// Check if a character is an operand (alpha or digit)
bool IsOperand(char c) {
    return isalpha(c) || isdigit(c);
}

// Function to convert infix regular expression to postfix
int Precedence(char op) {
    if (op == '*') return 3; // highest precedence for Kleene star (*)
    if (op == '.') return 2; // concatenation (.) has lower precedence than *
    if (op == '|') return 1; // alternation (|) has the lowest precedence
    return 0;
}

string InfixToPostfix(const string &infix) {
    stack<char> ops; // stack for operators
    string postfix = ""; // resulting postfix expression 'queue'
    for (size_t i = 0; i < infix.size(); i++) {
        char c = infix[i];
        if (IsOperand(c)) {
            postfix += c;
        } else if (c == '(') {
            ops.push(c);
        } else if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                postfix += ops.top();
                ops.pop();
            }
            ops.pop(); // pop the '('
        } else {
            while (!ops.empty() && ops.top() != '(' && Precedence(ops.top()) >= Precedence(c)) {
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
            // Create NFA for individual character
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({c, accept});
            nfaStack.push({start, accept});
            cout << "Created NFA for operand " << c << " with start state " << start->id << " and accept state " << accept->id << endl;
        } else if (c == '.') {
            // Concatenation: pop two NFAs and combine
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            nfa1.accept->transitions.push_back({'\0', nfa2.start});
            nfaStack.push({nfa1.start, nfa2.accept});
            cout << "Concatenated NFAs with new start state " << nfa1.start->id << " and new accept state " << nfa2.accept->id << endl;
        } else if (c == '|') {
            // Alternation: pop two NFAs and combine
            NFA nfa2 = nfaStack.top(); nfaStack.pop();
            NFA nfa1 = nfaStack.top(); nfaStack.pop();
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({'\0', nfa1.start});
            start->transitions.push_back({'\0', nfa2.start});
            nfa1.accept->transitions.push_back({'\0', accept});
            nfa2.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
            cout << "Created alternation NFA with new start state " << start->id << " and new accept state " << accept->id << endl;
        } else if (c == '*') {
            // Kleene Star: pop one NFA and apply star
            NFA nfa = nfaStack.top(); nfaStack.pop();
            auto start = make_shared<State>(State{stateId++});
            auto accept = make_shared<State>(State{stateId++});
            start->transitions.push_back({'\0', nfa.start});
            start->transitions.push_back({'\0', accept});
            nfa.accept->transitions.push_back({'\0', nfa.start});
            nfa.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
            cout << "Applied Kleene star to NFA with new start state " << start->id << " and new accept state " << accept->id << endl;
        }
    }

    NFA result = nfaStack.top();
    cout << "Final NFA start state: " << result.start->id << ", accept state: " << result.accept->id << endl;
    return result;
}

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
    stack<int> stateStack;
    for (int state : states) {
        stateStack.push(state);
    }
    while (!stateStack.empty()) {
        int currentState = stateStack.top();
        stateStack.pop();
        shared_ptr<State> nfaState = GetStateById(nfa, currentState);
        if (nfaState == nullptr) {
            continue;
        }
        for (const auto &transition : nfaState->transitions) {
            if (transition.first == '\0') {
                int targetState = transition.second->id;
                if (closure.find(targetState) == closure.end()) {
                    closure.insert(targetState);
                    stateStack.push(targetState);
                }
            }
        }
    }
    return closure;
}

// Subset construction to convert NFA to DFA
DFA NFAtoDFA(const NFA &nfa) {
    set<char> alphabet; // Define the alphabet dynamically
    for (char c = 'a'; c <= 'z'; c++) {
        alphabet.insert(c);
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        alphabet.insert(c);
    }
    for (char c = '0'; c <= '9'; c++) {
        alphabet.insert(c);
    }

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
            cout << "DFA transition from state " << stateMapping[currentSet] << " to state " << stateMapping[targetSet] << " on symbol " << symbol << endl;
        }
    }

    // Define DFA final states
    for (const auto &stateSetPair : stateMapping) {
        const set<int> &stateSet = stateSetPair.first;
        int dfaStateId = stateSetPair.second;
        if (stateSet.find(nfa.accept->id) != stateSet.end()) {
            dfa.AddFinalState(dfaStateId);
            cout << "DFA final state: " << dfaStateId << " for NFA accept state: " << nfa.accept->id << endl;
        }
    }

    return dfa;
}

// Function to trim leading and trailing whitespace from a string
string Trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) {
        return ""; // String is all whitespace
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Function to split a string by a delimiter and trim each part
vector<string> SplitAndTrim(const string& str, char delimiter) {
    vector<string> result;
    stringstream ss(str);
    string item;
    while (getline(ss, item, delimiter)) {
        result.push_back(Trim(item));
    }
    return result;
}

// Main lexer function
int main() {
    // Read input from stdin
    string line;
    getline(cin, line);

    // Parse token definitions
    vector<pair<string, string>> tokens;
    vector<string> tokenDefs = SplitAndTrim(line, ',');
    for (const string& tokenDef : tokenDefs) {
        size_t spacePos = tokenDef.find(' ');
        if (spacePos != string::npos) {
            string tokenName = tokenDef.substr(0, spacePos);
            string tokenRegex = tokenDef.substr(spacePos + 1);
            tokens.push_back({tokenName, tokenRegex});
        }
    }
    
    // Remove the trailing '#' from the last token's regular expression
    if (!tokens.empty()) {
        string& lastTokenRegex = tokens.back().second;
        if (!lastTokenRegex.empty() && lastTokenRegex.back() == '#') {
            lastTokenRegex.pop_back();
        }
    }

    // Debug: Print parsed token definitions
    cout << "Parsed Token Definitions:" << endl;
    for (const auto &token : tokens) {
        cout << "Token Name: " << token.first << ", Regex: " << token.second << endl;
    }

    // Read input string
    getline(cin, line);
    string inputString = Trim(line);
    inputString = inputString.substr(1, inputString.size() - 2); // Remove surrounding quotes

    // Convert token definitions to NFAs and then DFAs
    vector<DFA> dfas;
    for (const auto &token : tokens) {
        string postfix = InfixToPostfix(token.second);
        // Debug: Print postfix expression
        cout << "Infix: " << token.second << " -> Postfix: " << postfix << endl;

        NFA nfa = PostfixToNFA(postfix);
        DFA dfa = NFAtoDFA(nfa);
        dfas.push_back(dfa);
    }

    // Perform lexical analysis
    cout << "Lexical Analysis Output:" << endl;
    size_t index = 0;
    while (index < inputString.length()) {
        while (index < inputString.length() && isspace(inputString[index])) {
            index++;
        }
        if (index >= inputString.length()) break;

        bool matched = false;
        size_t longestMatchLength = 0;
        size_t longestMatchIndex = 0;
        string longestMatchToken;

        for (size_t i = 0; i < dfas.size(); i++) {
            DFA dfa = dfas[i];
            dfa.Reset();
            size_t j = index;
            cout << "Testing DFA for token: " << tokens[i].first << endl;
            while (j < inputString.length() && dfa.Move(inputString[j])) {
                cout << "Moved to state " << dfa.GetCurrentState() << " on symbol " << inputString[j] << endl;
                j++;
            }
            if (dfa.GetAccepted() && j - index > longestMatchLength) {
                longestMatchLength = j - index;
                longestMatchIndex = i;
                longestMatchToken = dfa.GetAcceptedLexeme();
                matched = true;
                cout << "Accepted token: " << tokens[i].first << " with lexeme: " << longestMatchToken << endl;
            }
        }

        if (matched) {
            cout << tokens[longestMatchIndex].first << " , \"" << longestMatchToken << "\"" << endl;
            index += longestMatchLength;
        } else {
            cout << "ERROR , \"" << inputString[index] << "\"" << endl;
            index++;
        }
    }

    return 0;
}
