#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <unordered_set>
#include <cctype>
#include <stdexcept>
using namespace std;

//--------------------------------------------------------------
// Token Types
//--------------------------------------------------------------
enum TokenType {
    ID,
    NUM,
    OP,
    KEYWORD,
    EOS,     // end of string
    INVALID
};
const string tokenNames[] = { "ID", "NUM", "OP", "KEYWORD", "EOS", "INVALID" };

//--------------------------------------------------------------
// Token Structure
//--------------------------------------------------------------
struct Token {
    TokenType type;
    string value;
};

//--------------------------------------------------------------
// RegexParser
//--------------------------------------------------------------
class RegexParser {
public:
    static string InfixToPostfix(const string& infix);
private:
    static int Precedence(char op);
    static bool IsOperand(char c);
    static string InsertConcatenation(const string& regex);
};

//--------------------------------------------------------------
// Regex2NFA
//--------------------------------------------------------------
class RegexToNFA {
public:
    struct NFAFragment {
        int startState;
        set<int> acceptStates;
        map<int, map<char, set<int>>> transitions;
    };

    RegexToNFA() : stateCounter(0) {}
    NFAFragment regex2NFA(const string& postfixRegex);

private:
    int stateCounter;
    int newState();
    NFAFragment singleChar(char c);
    NFAFragment epsilon();
    void concatenate(stack<NFAFragment>& nfaStack);
    void unionOp(stack<NFAFragment>& nfaStack);
    void kleeneStar(stack<NFAFragment>& nfaStack);
};

//--------------------------------------------------------------
// NFA2DFA
//--------------------------------------------------------------
class NFA2DFA {
public:
    struct DFA {
        set<set<int>> states;
        set<int> startState;
        set<set<int>> acceptStates;
        map<set<int>, map<char, set<int>>> transitions;
    };

    static DFA convert(const RegexToNFA::NFAFragment& nfa);
};

//--------------------------------------------------------------
// Lexer
//--------------------------------------------------------------
class Lexer {
public:
    Lexer(const string& input, const map<string, NFA2DFA::DFA>& dfas) : input(input), dfas(dfas), pos(0) {}
    Token getToken();

private:
    string input;
    map<string, NFA2DFA::DFA> dfas;
    size_t pos;
};

//--------------------------------------------------------------
// Lexer Implementation
//--------------------------------------------------------------
Token Lexer::getToken() {
    // Skip whitespace
    while (pos < input.size() && isspace(input[pos]))
        pos++;

    // Check for end of string
    if (pos == input.size())
        return Token{EOS, ""};

    // Try to match tokens using DFAs
    for (const auto& [tokenName, dfa] : dfas) {
        set<int> currentStates = dfa.startState;
        size_t startPos = pos;
        while (pos < input.size()) {
            char nextChar = input[pos];
            set<int> nextStates;
            for (int state : currentStates) {
                if (dfa.transitions[state].count(nextChar)) {
                    nextStates.insert(dfa.transitions[state][nextChar].begin(), dfa.transitions[state][nextChar].end());
                }
            }
            if (nextStates.empty())
                break;
            currentStates = nextStates;
            pos++;
        }

        // Check if the current states include an accept state
        for (int state : currentStates) {
            if (dfa.acceptStates.count(state)) {
                string lexeme = input.substr(startPos, pos - startPos);
                return Token{ID, lexeme}; // Replace ID with the appropriate token type
            }
        }

        // Reset position if no match
        pos = startPos;
    }

    // No match found
    return Token{INVALID, string(1, input[pos++])};
}

//--------------------------------------------------------------
// Main Function
//--------------------------------------------------------------
int main() {
    // Read input from stdin
    string tokenDefs, inputStr;
    getline(cin, tokenDefs);
    getline(cin, inputStr);

    // Parse token definitions and build DFAs
    map<string, NFA2DFA::DFA> dfas;
    // (Implementation of parsing token definitions and building DFAs goes here)

    // Lexer
    Lexer lexer(inputStr, dfas);
    Token token;
    while ((token = lexer.getToken()).type != EOS) {
        cout << "Token: " << tokenNames[token.type] << ", Lexeme: \"" << token.value << "\"" << endl;
    }

    return 0;
}
