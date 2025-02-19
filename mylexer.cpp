#include <iostream>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <vector>
using namespace std;

// Token types
enum TokenType {
    TOK_A, TOK_B, TOK_C, TOK_D, TOK_E, TOK_END, TOK_INVALID
};

// Token structure
struct Token {
    TokenType type;
    char value;
};

// DFA class definition
class DFA {
public:
    DFA(set<char> A, set<int> I, set<int> F) : alpha(A), init_states(I), fin_states(F) { Reset(); }

    void Reset() {
        status = START;
        current_state = *init_states.begin();
        accepted = false;
        lexeme.clear();
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

    void Move(char c) {
        if (Dtran[current_state].find(c) != Dtran[current_state].end()) {
            current_state = Dtran[current_state][c];
            lexeme += c;
            if (fin_states.count(current_state) > 0) {
                status = ACCEPT;
                accepted = true;
                accepted_lexeme = lexeme;
            } else {
                status = POTENTIAL;
            }
        } else {
            status = FAIL;
        }
    }

    void Print() {
        cout << "DFA Transitions:\n";
        for (const auto& dfa_row : Dtran) {
            cout << dfa_row.first << ":\t";
            for (const auto& transition : dfa_row.second) {
                cout << transition.first << ": " << transition.second << " ";
            }
            cout << endl;
        }
    }

private:
    enum Status { START, POTENTIAL, ACCEPT, FAIL };
    set<char> alpha;
    set<int> init_states;
    set<int> fin_states;
    map<int, map<char, int>> Dtran;
    Status status;
    int current_state;
    bool accepted;
    string lexeme;
    string accepted_lexeme;
};

// Lexer class to tokenize the input
class Lexer {
public:
    Lexer(const string& input) : input(input), index(0) {}
    Token getToken() {
        while (index < input.length() && isspace(input[index]))
            index++;
        if (index >= input.length())
            return {TOK_END, '\0'};
        char currentChar = input[index++];
        switch (currentChar) {
            case 'a': return {TOK_A, 'a'};
            case 'b': return {TOK_B, 'b'};
            case 'c': return {TOK_C, 'c'};
            case 'd': return {TOK_D, 'd'};
            case 'e': return {TOK_E, 'e'};
            default: return {TOK_INVALID, currentChar};
        }
    }
private:
    string input;
    int index;
};

// Recursive Descent Parser class
class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer), currentToken(lexer.getToken()) {}
    bool parseS();
private:
    void match(TokenType expectedType);
    bool parseA();
    Lexer& lexer;
    Token currentToken;
};

// match consumes a token if it matches the expected type
void Parser::match(TokenType expectedType) {
    if (currentToken.type == expectedType)
        currentToken = lexer.getToken();
    else {
        cerr << "Syntax Error: Expected token of type " << expectedType << ", but got " << currentToken.type << endl;
        exit(1);
    }
}

// S -> aAe
bool Parser::parseS() {
    if (currentToken.type == TOK_A) {
        match(TOK_A); // consume 'a'
        if (!parseA()) return false;
        if (currentToken.type == TOK_E) {
            match(TOK_E); // consume 'e'
            return true;
        }
    }
    return false;
}

// A -> bAd | c
bool Parser::parseA() {
    if (currentToken.type == TOK_B) {
        match(TOK_B); // consume 'b'
        if (!parseA()) return false;
        if (currentToken.type == TOK_D) {
            match(TOK_D); // consume 'd'
            return true;
        }
        return false;
    } else if (currentToken.type == TOK_C) {
        match(TOK_C); // consume 'c'
        return true;
    }
    return false;
}

// define precedence and associativity of operators
int Precedence(char op) {
    if (op == '*') return 3; // highest precedence for Kleene star (*)
    if (op == '.') return 2; // concatenation (.) has lower precedence than *
    if (op == '|') return 1; // alternation (|) has the lowest precedence
    return 0;
}

// check if a character is an operand (alpha)
bool IsOperand(char c) {
    return isalpha(c);
}

// convert an infix regular expression to postfix
string InfixToPostfix(const string &infix) {
    stack<char> ops; // stack for operators
    string postfix = ""; // resulting postfix expression 'queue'
    for (size_t i = 0; i < infix.size(); i++) {
        char c = infix[i];
        if (IsOperand(c)) {
            postfix += c; // if c is an operand, put it on the output queue
        } else if (c == '(') {
            ops.push(c); // if it's an opening parenthesis, push it onto the stack
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
            ops.push(c); // push the operator onto the stack
        }
    }
    while (!ops.empty()) {
        postfix += ops.top();
        ops.pop();
    }
    return postfix;
}

// NFA State
struct State {
    int id;
    vector<pair<char, State*>> transitions; // transitions are labeled with characters
};

// NFA Structure
struct NFA {
    State* start;
    State* accept;
};

// Thompson's Construction: Create NFA from postfix regex
NFA PostfixToNFA(const string &postfix) {
    stack<NFA> nfaStack;
    int stateId = 0;

    for (char c : postfix) {
        if (IsOperand(c)) {
            State* start = new State{stateId++};
            State* accept = new State{stateId++};
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
            State* start = new State{stateId++};
            State* accept = new State{stateId++};
            start->transitions.push_back({'\0', nfa1.start});
            start->transitions.push_back({'\0', nfa2.start});
            nfa1.accept->transitions.push_back({'\0', accept});
            nfa2.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
        } else if (c == '*') {
            NFA nfa = nfaStack.top(); nfaStack.pop();
            State* start = new State{stateId++};
            State* accept = new State{stateId++};
            start->transitions.push_back({'\0', nfa.start});
            start->transitions.push_back({'\0', accept});
            nfa.accept->transitions.push_back({'\0', nfa.start});
            nfa.accept->transitions.push_back({'\0', accept});
            nfaStack.push({start, accept});
        }
    }

    return nfaStack.top();
}

// Function to retrieve NFA state by ID
State* GetStateById(const NFA &nfa, int id) {
    // Note: Implement your own logic to find the state by ID in your NFA structure.
    return nullptr;
}
// Subset construction to convert NFA to DFA
DFA NFAtoDFA(const NFA &nfa) {
    set<char> alphabet = {'a', 'b'}; // Define the alphabet here
    set<int> startSet = {nfa.start->id};
    DFA dfa(alphabet, startSet, {}); // Initial DFA with defined alphabet and states

    map<set<int>, int> stateMapping; // Map NFA state sets to DFA state IDs
    map<int, set<int>> dfaStateSets; // Map DFA state IDs to NFA state sets
    queue<set<int>> stateQueue;
    set<int> currentSet = startSet;
    int stateId = 0;

    stateMapping[currentSet] = stateId++;
    dfaStateSets[stateMapping[currentSet]] = currentSet;
    stateQueue.push(currentSet);

    while (!stateQueue.empty()) {
        currentSet = stateQueue.front();
        stateQueue.pop();

        map<char, set<int>> transitions;

        for (int nfaStateId : currentSet) {
            State* nfaState = GetStateById(nfa, nfaStateId);
            for (const auto &transition : nfaState->transitions) {
                char symbol = transition.first;
                State* targetState = transition.second;
                transitions[symbol].insert(targetState->id);
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

int main() {
    string tokenDefs;
    string inputString;

    // Read token definitions and input string from stdin
    getline(cin, tokenDefs);
    getline(cin, inputString);

    // Process token definitions
    vector<pair<string, string>> tokens;
    size_t pos = 0;
    while ((pos = tokenDefs.find(",")) != string::npos) {
        string token = tokenDefs.substr(0, pos);
        size_t spacePos = token.find(" ");
        string tokenName = token.substr(0, spacePos);
        string tokenRegex = token.substr(spacePos + 1);
        tokens.push_back({tokenName, tokenRegex});
        tokenDefs.erase(0, pos + 1);
    }
    tokenDefs.pop_back(); // Remove the trailing #
    size_t spacePos = tokenDefs.find(" ");
    string tokenName = tokenDefs.substr(0, spacePos);
    string tokenRegex = tokenDefs.substr(spacePos + 1);
    tokens.push_back({tokenName, tokenRegex});

    // Convert each regex to postfix and create NFA for each token
    vector<NFA> nfas;
    for (const auto& token : tokens) {
        string postfix = InfixToPostfix(token.second);
        NFA nfa = PostfixToNFA(postfix);
        nfas.push_back(nfa);
    }

    // Perform lexical analysis on the input string using the DFAs
    vector<pair<string, string>> results;
    size_t index = 0;
    while (index < inputString.length()) {
        bool matched = false;
        for (size_t i = 0; i < nfas.size(); ++i) {
            DFA dfa = NFAtoDFA(nfas[i]);
            dfa.Reset();

            size_t j = index;
            while (j < inputString.length() && !isspace(inputString[j])) {
                dfa.Move(inputString[j]);
                if (dfa.GetAccepted()) {
                    results.push_back({tokens[i].first, inputString.substr(index, j - index + 1)});
                    index = j + 1;
                    matched = true;
                    break;
                }
                ++j;
            }
            if (matched) break;
        }
        if (!matched) {
            results.push_back({"ERROR", string(1, inputString[index])});
            ++index;
        }
    }

    // Output the results
    for (const auto& result : results) {
        cout << result.first << " , \"" << result.second << "\"" << endl;
    }

    return 0;
}
